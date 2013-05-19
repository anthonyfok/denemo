#ifdef _HAVE_PORTAUDIO_
/*
 * portaudiobackend.c
 * PortAudio backend.
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * Copyright (C) 2011  Dominic Sacré
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "portaudiobackend.h"
#include "portaudioutil.h"
#include "midi.h"
#include "fluid.h"
#include "audiointerface.h"

#include <portaudio.h>
#include <glib.h>
#include <string.h>
#include "audiofile.h"


static PaStream *stream;
static unsigned long sample_rate;

static unsigned long playback_frame = 0;

static gboolean reset_audio = FALSE;

static gint ready = FALSE;

static double
nframes_to_seconds (unsigned long nframes)
{
  return nframes / (double) sample_rate;
}

static unsigned long
seconds_to_nframes (double seconds)
{
  return (unsigned long) (sample_rate * seconds);
}

#define MAX_MESSAGE_LENGTH (255)        //Allow single sysex blocks, ie 0xF0, length, data...0xF7  where length is one byte.

static int
stream_callback (const void *input_buffer, void *output_buffer, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo * time_info, PaStreamCallbackFlags status_flags, void *user_data)
{
  float **buffers = (float **) output_buffer;
#ifdef HALF_TEMPO
  static gboolean even = TRUE;
  even = !even;
  if (even)
    {
      return paContinue;
    }
#endif

  if(Denemo.prefs.maxrecordingtime)
    {static FILE *fp=NULL;
      if (Denemo.keyboard_state == (GDK_SHIFT_MASK) || Denemo.keyboard_state == (GDK_LOCK_MASK))
        {static guint recorded_frames;
        if (fp==NULL)
          {
            extern const gchar *locatedotdenemo(void);
            const gchar *filename = recorded_audio_filename();
            fp=fopen(filename, "wb");
            recorded_frames=0;
            if (fp==NULL)
                g_warning("Could not open denemo-output");
            else
                g_print("Opened output file");
          }
        if (fp) {
          if (recorded_frames/44100 < Denemo.prefs.maxrecordingtime)
            {
              fwrite(buffers[0], sizeof(float), frames_per_buffer, fp);
              recorded_frames += frames_per_buffer;
            }
          else
            { //only warn once, don't spew out warnings...
            if (recorded_frames<G_MAXINT)
              {
                recorded_frames=G_MAXINT;
                g_warning("Recording length exceeded preference (%d seconds); use the Change Preferences dialog to alter this", Denemo.prefs.maxrecordingtime);
              }
            }
          }
        }
    else
    {
      if(fp)
        {
          fclose(fp);
          fp=NULL;
          g_print("File closed samples are raw data, Little Endian (? or architecture dependent), mono");
        }
    }
  }

  size_t i;
  for (i = 0; i < 2; ++i)
    {
      memset (buffers[i], 0, frames_per_buffer * sizeof (float));
    }

  if (!ready)
    return paContinue;

#ifdef _HAVE_FLUIDSYNTH_
  if (reset_audio)
    {
      fluidsynth_all_notes_off ();
      reset_audio = FALSE;
      return paContinue;
    }

  unsigned char event_data[MAX_MESSAGE_LENGTH]; //needs to be long enough for variable length messages...
  size_t event_length = MAX_MESSAGE_LENGTH;
  double event_time;

  double until_time = nframes_to_seconds (playback_frame + frames_per_buffer);


  while (read_event_from_queue (AUDIO_BACKEND, event_data, &event_length, &event_time, until_time))
    {
      fluidsynth_feed_midi (event_data, event_length);  //in fluid.c note fluidsynth api ues fluid_synth_xxx these naming conventions are a bit too similar
    }

  fluidsynth_render_audio (frames_per_buffer, buffers[0], buffers[1]);  //in fluid.c calls fluid_synth_write_float()

// Now get any audio to mix - dump it in the left hand channel for now
  event_length = frames_per_buffer;
  read_event_from_mixer_queue (AUDIO_BACKEND, (void*) buffers[1], &event_length, &event_time, until_time);


  if (until_time < get_playuntil ())
    {
#endif
      playback_frame += frames_per_buffer;
      update_playback_time (TIMEBASE_PRIO_AUDIO, nframes_to_seconds (playback_frame));
#ifdef _HAVE_FLUIDSYNTH_
    }
#endif

  return paContinue;
}

static int
actual_portaudio_initialize (DenemoPrefs * config)
{

  sample_rate = config->portaudio_sample_rate;

#ifdef _HAVE_FLUIDSYNTH_
  g_print ("Initializing Fluidsynth\n");
  if (fluidsynth_init (config, sample_rate))
    {
      g_warning ("Initializing Fluidsynth FAILED!\n");
      return -1;
    }
#endif


  g_print ("initializing PortAudio backend\n");

  PaStreamParameters output_parameters;
  PaError err;

  err = Pa_Initialize ();
  if (err != paNoError)
    {
      g_warning ("initializing PortAudio failed\n");
      return -1;
    }

  output_parameters.device = get_portaudio_device_index (config->portaudio_device->str);

  if (output_parameters.device == paNoDevice)
    {
      return -1;
    }

  PaDeviceInfo const *info = Pa_GetDeviceInfo (output_parameters.device);

  if (!info)
    {
      g_warning ("invalid device '%s\n'", config->portaudio_device->str);
      return -1;
    }

  char const *api_name = Pa_GetHostApiInfo (info->hostApi)->name;
  g_print ("opening output device '%s: %s'\n", api_name, info->name);

  output_parameters.channelCount = 2;
  output_parameters.sampleFormat = paFloat32 | paNonInterleaved;
  output_parameters.suggestedLatency = Pa_GetDeviceInfo (output_parameters.device)->defaultLowOutputLatency;
  output_parameters.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream (&stream, NULL, &output_parameters, config->portaudio_sample_rate, config->portaudio_period_size, paNoFlag /* make this a pref??? paClipOff */ , stream_callback, NULL);
  if (err != paNoError)
    {
      g_warning ("couldn't open output stream\n");
      return -1;
    }



  err = Pa_StartStream (stream);
  if (err != paNoError)
    {
      g_warning ("couldn't start output stream\n");
      return -1;
    }

  return 0;
}

static int
ready_now ()
{
  ready = TRUE;
  return FALSE;
}

static int
portaudio_initialize (DenemoPrefs * config)
{
  g_idle_add ((GSourceFunc) ready_now, NULL);
  return actual_portaudio_initialize (config);
}

static int
portaudio_destroy ()
{
  g_print ("destroying PortAudio backend\n");
  ready = FALSE;
  PaError err;

  err = Pa_CloseStream (stream);
  if (err != paNoError)
    {
      g_warning ("closing stream failed: %d, %s\n", err, Pa_GetErrorText (err));
      return -1;
    }

  Pa_Terminate ();

#ifdef _HAVE_FLUIDSYNTH_
  fluidsynth_shutdown ();
#endif

  return 0;
}


static int
portaudio_reconfigure (DenemoPrefs * config)
{
  portaudio_destroy ();
  return portaudio_initialize (config);
}


static int
portaudio_start_playing ()
{
  playback_frame = seconds_to_nframes (get_playback_time ());
  return 0;
}


static int
portaudio_stop_playing ()
{
  reset_audio = TRUE;
  return 0;
}


static int
portaudio_panic ()
{
  reset_audio = TRUE;
  return 0;
}


backend_t portaudio_backend = {
  portaudio_initialize,
  portaudio_destroy,
  portaudio_reconfigure,
  portaudio_start_playing,
  portaudio_stop_playing,
  portaudio_panic,
};
#endif //_HAVE_PORTAUDIO_
