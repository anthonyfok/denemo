/*
 * jackbackend.c
 * JACK audio and MIDI backends.
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * Copyright (C) 2011  Dominic Sacré
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "jackbackend.h"
#include "midi.h"
#include "fluid.h"

#include <jack/jack.h>
#include <jack/midiport.h>
#include <glib.h>
#include <string.h>
#include <assert.h>


typedef jack_default_audio_sample_t sample_t;
typedef jack_nframes_t nframes_t;


static char const *JACK_CLIENT_NAME = "denemo";

static jack_client_t *client = NULL;

static jack_port_t **audio_in_ports = NULL;
static jack_port_t **audio_out_ports = NULL;
static jack_port_t **midi_in_ports = NULL;
static jack_port_t **midi_out_ports = NULL;

static size_t num_audio_in_ports;
static size_t num_audio_out_ports;
static size_t num_midi_in_ports;
static size_t num_midi_out_ports;

static gboolean audio_initialized = FALSE;
static gboolean midi_initialized = FALSE;

static nframes_t playback_frame = 0;
static gboolean reset_midi = FALSE;


static double nframes_to_seconds(nframes_t nframes) {
  return nframes / (double)jack_get_sample_rate(client);
}

static nframes_t seconds_to_nframes(double seconds) {
  return (nframes_t)(jack_get_sample_rate(client) * seconds);
}


static void process_audio(nframes_t nframes) {
  size_t i;
  sample_t *port_buffers[num_audio_out_ports];

  for (i = 0; i < num_audio_out_ports; ++i) {
    port_buffers[i] = jack_port_get_buffer(audio_out_ports[i], nframes);
    memset(port_buffers[i], 0, nframes * sizeof(sample_t));
  }

#ifdef _HAVE_FLUIDSYNTH_
  unsigned char event_data[3];
  size_t event_length;
  double event_time;

  double until_time = nframes_to_seconds(playback_frame + nframes);

  while (read_event_from_queue(AUDIO_BACKEND, event_data, &event_length, &event_time, until_time)) {
    // FIXME: we can't pass an exact frame/time to fluidsynth, can we...?
    fluidsynth_feed_midi(event_data, event_length);
  }


  assert(num_audio_out_ports >= 2);

  fluidsynth_render_audio(nframes, port_buffers[0], port_buffers[1]);
#endif
}


static void process_midi(nframes_t nframes) {
  size_t i;
  void *port_buffers[num_midi_out_ports];

  for (i = 0; i < num_midi_out_ports; ++i) {
    port_buffers[i] = jack_port_get_buffer(midi_out_ports[i], nframes);
    jack_midi_clear_buffer(port_buffers[i]);
  }

  if (reset_midi) {
    // send all-notes-off on all ports and on all channels
    for (i = 0; i < num_midi_out_ports; ++i) {
      int n;
      for (n = 0; n < 16; ++n) {
        unsigned char event_data[] = { CONTROL_CHANGE | n, 123, 0 };

        jack_midi_event_write(port_buffers[i], 0, event_data, sizeof(event_data));
      }
    }

    reset_midi = FALSE;

    return;
  }

  unsigned char event_data[3];
  size_t event_length;
  double event_time;

  double until_time = nframes_to_seconds(playback_frame + nframes);

  while (read_event_from_queue(MIDI_BACKEND, event_data, &event_length, &event_time, until_time)) {
    nframes_t frame = seconds_to_nframes(event_time) - playback_frame;

    // FIXME: use correct port
    jack_midi_event_write(port_buffers[0], frame, event_data, event_length);
  }
}




static int process_callback(nframes_t nframes, void *arg) {
  if (g_atomic_int_get(&audio_initialized)) {
    process_audio(nframes);
  }
  if (g_atomic_int_get(&midi_initialized)) {
    process_midi(nframes);
  }

  playback_frame += nframes;

  update_playback_time(AUDIO_BACKEND, nframes_to_seconds(playback_frame));

  return 0;
}


static void shutdown_callback(void *arg) {
  g_warning("*** shut down by JACK! ***\n");

  g_atomic_int_set(&audio_initialized, FALSE);
  g_atomic_int_set(&midi_initialized, FALSE);
}



static int initialize_client(char const *name) {
  if (client) {
    // already initialized
    return 0;
  }

  if ((client = jack_client_open(name, JackNullOption, NULL)) == NULL) {
    g_warning("can't connect to jack server\n");
    return -1;
  }

  jack_set_process_callback(client, &process_callback, NULL);
  jack_on_shutdown(client, &shutdown_callback, NULL);

  if (jack_activate(client)) {
    g_warning("can't activate jack client\n");
    return -1;
  }

  return 0;
}


static int destroy_client() {
  if (audio_initialized || midi_initialized) {
    // don't destroy client if someone's still using it
    return 0;
  }

  if (client) {
    jack_deactivate(client);
    jack_client_close(client);
    client = NULL;
  }

  return 0;
}


static int unregister_audio_ports() {
  jack_port_t **p;

  for (p = audio_in_ports; *p != NULL; ++p) {
    jack_port_unregister(client, *p);
  }

  for (p = audio_out_ports; *p != NULL; ++p) {
    jack_port_unregister(client, *p);
  }

  g_free(audio_in_ports);
  g_free(audio_out_ports);

  audio_in_ports = NULL;
  audio_out_ports = NULL;

  return 0;
}


static int register_audio_ports(int num_in_ports, char const *in_portnames[],
                                int num_out_ports, char const *out_portnames[]) {
  int n;

  // allocate one more item as an end-of-array marker
  audio_in_ports = g_new0(jack_port_t*, num_in_ports + 1);
  audio_out_ports = g_new0(jack_port_t*, num_out_ports + 1);
  num_audio_in_ports = num_in_ports;
  num_audio_out_ports = num_out_ports;

  for (n = 0; n < num_in_ports; ++n) {
    audio_in_ports[n] = jack_port_register(client, in_portnames[n], JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    if (audio_in_ports[n] == NULL) {
      goto err;
    }
  }
  for (n = 0; n < num_out_ports; ++n) {
    audio_out_ports[n] = jack_port_register(client, out_portnames[n], JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    if (audio_out_ports[n] == NULL) {
      goto err;
    }
  }

  return 0;

err:
  unregister_audio_ports();

  return -1;
}


static int unregister_midi_ports() {
  jack_port_t **p;

  for (p = midi_in_ports; *p != NULL; ++p) {
    jack_port_unregister(client, *p);
  }

  for (p = midi_out_ports; *p != NULL; ++p) {
    jack_port_unregister(client, *p);
  }

  g_free(midi_in_ports);
  g_free(midi_out_ports);

  midi_in_ports = NULL;
  midi_out_ports = NULL;

  return 0;
}


static int register_midi_ports(int num_in_ports, char const *in_portnames[],
                               int num_out_ports, char const *out_portnames[]) {
  int n;

  // allocate one more item as an end-of-array marker
  midi_in_ports = g_new0(jack_port_t*, num_in_ports + 1);
  midi_out_ports = g_new0(jack_port_t*, num_out_ports + 1);
  num_midi_in_ports = num_in_ports;
  num_midi_out_ports = num_out_ports;

  for (n = 0; n < num_in_ports; ++n) {
    midi_in_ports[n] = jack_port_register(client, in_portnames[n], JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    if (midi_in_ports[n] == NULL) {
      goto err;
    }
  }
  for (n = 0; n < num_out_ports; ++n) {
    midi_out_ports[n] = jack_port_register(client, out_portnames[n], JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
    if (midi_out_ports[n] == NULL) {
      goto err;
    }
  }

  return 0;

err:
  unregister_midi_ports();

  return -1;
}



static int jack_audio_initialize(DenemoPrefs *config) {
  g_print("initializing JACK audio backend\n");

  if (initialize_client(JACK_CLIENT_NAME)) {
    return -1;
  }

#ifdef _HAVE_FLUIDSYNTH_
  if (fluidsynth_init(config, jack_get_sample_rate(client))) {
    return -1;
  }
#endif

  // mono input, stereo output
  char const *in_portnames[] = { "in_1" };
  char const *out_portnames[] = { "out_1", "out_2" };

  if (register_audio_ports(1, in_portnames, 2, out_portnames)) {
    return -1;
  }

  g_atomic_int_set(&audio_initialized, TRUE);

  return 0;
}


static int jack_audio_destroy() {
  g_print("destroying JACK audio backend\n");

  if (audio_initialized) {
    g_atomic_int_set(&audio_initialized, FALSE);

    unregister_audio_ports();
  }

  destroy_client();

#ifdef _HAVE_FLUIDSYNTH_
  fluidsynth_shutdown();
#endif

  return 0;
}


static int jack_audio_reconfigure(DenemoPrefs *config) {
  g_print("reconfiguring JACK audio backend\n");

  jack_audio_destroy();
  jack_audio_initialize(config);

  return 0;
}


static int jack_audio_start_playing() {
  playback_frame = 0;
  return 0;
}


static int jack_audio_stop_playing() {
  return 0;
}


static int jack_audio_play_midi_event(int port, unsigned char *buffer) {
  int channel = buffer[0] & 0x0f;
  int type = (buffer[0] & 0xf0) >> 4;
  g_print("playing midi event: port=%d, channel=%d, type=%x\n", port, channel, type);
  return 0;
}


static int jack_audio_panic() {
  g_print("panicking\n");
  return 0;
}



static int jack_midi_initialize(DenemoPrefs *config) {
  g_print("initializing JACK MIDI backend\n");

  if (initialize_client(JACK_CLIENT_NAME)) {
    return -1;
  }

  // FIXME: get port names from config
  char const *in_portnames[] = { "midi_in_1" };
  char const *out_portnames[] = { "midi_out_1" };

  if (register_midi_ports(1, in_portnames, 1, out_portnames)) {
    return -1;
  }

  g_atomic_int_set(&midi_initialized, TRUE);

  return 0;
}


static int jack_midi_destroy() {
  g_print("destroying JACK MIDI backend\n");

  if (midi_initialized) {
    g_atomic_int_set(&midi_initialized, FALSE);

    unregister_midi_ports();
  }

  destroy_client();

  return 0;
}


static int jack_midi_reconfigure(DenemoPrefs *config) {
  g_print("reconfiguring JACK MIDI backend\n");

  jack_midi_destroy();
  jack_midi_initialize(config);

  return 0;
}


static int jack_midi_start_playing() {
  playback_frame = 0;
  return 0;
}


static int jack_midi_stop_playing() {
  reset_midi = TRUE;
  return 0;
}


static int jack_midi_play_midi_event(int port, unsigned char *buffer) {
  int channel = buffer[0] & 0x0f;
  int type = (buffer[0] & 0xf0) >> 4;
  g_print("playing midi event: port=%d, channel=%d, type=%x\n", port, channel, type);
  return 0;
}


static int jack_midi_panic() {
  g_print("panicking\n");
  reset_midi = TRUE;
  return 0;
}


backend_t jack_audio_backend = {
  jack_audio_initialize,
  jack_audio_destroy,
  jack_audio_reconfigure,
  jack_audio_start_playing,
  jack_audio_stop_playing,
  jack_audio_play_midi_event,
  jack_audio_panic,
};

backend_t jack_midi_backend = {
  jack_midi_initialize,
  jack_midi_destroy,
  jack_midi_reconfigure,
  jack_midi_start_playing,
  jack_midi_stop_playing,
  jack_midi_play_midi_event,
  jack_midi_panic,
};
