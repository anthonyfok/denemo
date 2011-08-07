/* prefdialog.cpp
 * functions for a preferences dialog
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 1999-2005 Matthew Hiller, Adam Tee
 * (c) 2011 Dominic Sacré
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <denemo/denemo.h>
#include "prefops.h"
#include "utils.h"
#include "playback.h"
#include "fluid.h"

struct callbackdata
{
  DenemoPrefs *prefs;
  GtkWidget *lilypath;
  GtkWidget *immediateplayback;
  GtkWidget *pitchspellingchannel;
  GtkWidget *pitchspellingprogram;
  GtkWidget *modal;
  GtkWidget *cursor_highlight;
  GtkWidget *persistence;
  GtkWidget *startmidiin;
  GtkWidget *applytoselection;
  GtkWidget *quickshortcuts;
  GtkWidget *saveparts;
  GtkWidget *autosave;
  GtkWidget *toolbar;
  GtkWidget *notation_palette;
  GtkWidget *rhythm_palette;
  GtkWidget *object_palette;
  GtkWidget *articulation_palette;
  GtkWidget *midi_in_controls;
  GtkWidget *playback_controls;

  GtkWidget *console_pane;
  GtkWidget *lyrics_pane;
  GtkWidget *visible_directive_buttons;
  GtkWidget *autoupdate;

  GtkWidget *autosave_timeout;
  GtkWidget *maxhistory;
  GtkWidget *browser;
  GtkWidget *pdfviewer;
  GtkWidget *imageviewer;
  GtkWidget *username;
  GtkWidget *password;
  GtkWidget *dynamic_compression;

  GtkWidget *zoom;
  GtkWidget *system_height;


  GtkWidget *audio_driver;
  GtkWidget *midi_driver;

#ifdef _HAVE_JACK_
  GtkWidget *jacktransport;
  GtkWidget *jacktransport_start_stopped;
#endif
#ifdef _HAVE_PORTAUDIO_
  GtkWidget *portaudio_sample_rate;
  GtkWidget *portaudio_period_size;
#endif
#ifdef _HAVE_FLUIDSYNTH_
  GtkWidget *fluidsynth_soundfont;
  GtkWidget *fluidsynth_reverb;
  GtkWidget *fluidsynth_chorus;
#endif

  GtkWidget *display_refresh;
  GtkWidget *animation_steps;
  GtkWidget *profile;
  GtkWidget *midiplayer;
  GtkWidget *audioplayer;
  GtkWidget *denemopath;
  GtkWidget *temperament;
  GtkWidget *strictshortcuts;
  GtkWidget *resolution;
  GtkWidget *overlays;
  GtkWidget *enable_thumbnails;
  GtkWidget *continuous;
};

struct audio_callback_data
{
  GtkWidget *audio_driver;
  GtkWidget *midi_driver;
#ifdef _HAVE_PORTAUDIO_
  GtkWidget *portaudio_settings;
#endif
};


static GList *audio_backend_list = NULL;
static GList *audio_driver_option_list = NULL;
static GList *midi_backend_list = NULL;
static GList *midi_driver_option_list = NULL;


/**
 * Callback to enable/disable the autosave entry when the auto save button is
 * clicked
 */
static void
toggle_autosave (GtkToggleButton * togglebutton, GtkWidget * autosave_timeout)
{
  g_debug("autosave now %d\n",
     gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(autosave_timeout)));
  gtk_widget_set_sensitive (autosave_timeout,
                            gtk_toggle_button_get_active (togglebutton));
}

static void
set_preferences (struct callbackdata *cbdata)
{
  DenemoPrefs *prefs = cbdata->prefs;

#define ASSIGNTEXT(field) \
  g_string_assign (prefs->field,\
    gtk_entry_get_text (GTK_ENTRY (cbdata->field)));

#define ASSIGNBOOLEAN(field) \
  prefs->field =\
    gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(cbdata->field));

#define ASSIGNINT(field) \
   prefs->field =\
    gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(cbdata->field));

#define ASSIGNDOUBLE(field) \
   prefs->field =\
     gtk_spin_button_get_value (GTK_SPIN_BUTTON(cbdata->field));

#define ASSIGNCOMBO(field) \
  g_string_assign (prefs->field,\
    (gchar *) gtk_entry_get_text (GTK_ENTRY (GTK_COMBO (cbdata->field)->entry)));

  ASSIGNTEXT(lilypath)
  ASSIGNTEXT(browser)
  ASSIGNTEXT(pdfviewer)
  ASSIGNTEXT(imageviewer)
  ASSIGNTEXT(username)
  ASSIGNTEXT(password)
  ASSIGNTEXT(profile)
  ASSIGNTEXT(audioplayer)
  ASSIGNTEXT(denemopath)

//  ASSIGNCOMBO(audio_driver)
//  ASSIGNCOMBO(midi_driver)

  gchar const *text = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(cbdata->audio_driver)->entry));
  GList *item = g_list_find_custom(audio_driver_option_list, text, (GCompareFunc)strcmp);
  gint index = g_list_position(audio_driver_option_list, item);
  gchar *backend = g_list_nth_data(audio_backend_list, index);
  g_string_assign(prefs->audio_driver, backend);

  text = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(cbdata->midi_driver)->entry));
  item = g_list_find_custom(midi_driver_option_list, text, (GCompareFunc)strcmp);
  index = g_list_position(midi_driver_option_list, item);
  backend = g_list_nth_data(midi_backend_list, index);
  g_string_assign(prefs->midi_driver, backend);


#ifdef _HAVE_JACK_
//  ASSIGNBOOLEAN(jacktransport)
//  ASSIGNBOOLEAN(jacktransport_start_stopped)
#endif

#ifdef _HAVE_PORTAUDIO_
  ASSIGNINT(portaudio_sample_rate)
  ASSIGNINT(portaudio_period_size)
#endif

#ifdef _HAVE_FLUIDSYNTH_
  ASSIGNTEXT(fluidsynth_soundfont)
  ASSIGNBOOLEAN(fluidsynth_reverb)
  ASSIGNBOOLEAN(fluidsynth_chorus)
#endif

  ASSIGNDOUBLE(display_refresh)
  ASSIGNINT(animation_steps)
  ASSIGNTEXT(temperament)
  ASSIGNBOOLEAN(strictshortcuts)
  ASSIGNBOOLEAN(overlays)
  ASSIGNBOOLEAN(enable_thumbnails)
  ASSIGNBOOLEAN(continuous)
  ASSIGNINT(resolution)
  ASSIGNINT(maxhistory)
  ASSIGNINT(dynamic_compression)
  ASSIGNINT(zoom)
  ASSIGNINT(system_height)


  ASSIGNBOOLEAN(immediateplayback)
  ASSIGNINT(pitchspellingchannel)
  ASSIGNINT(pitchspellingprogram)
  ASSIGNBOOLEAN(modal)
  ASSIGNBOOLEAN(persistence)
  ASSIGNBOOLEAN(cursor_highlight)
  ASSIGNBOOLEAN(startmidiin)
  ASSIGNBOOLEAN(applytoselection)
  ASSIGNBOOLEAN(quickshortcuts)
  ASSIGNBOOLEAN(autosave)
  ASSIGNINT(autosave_timeout)
  ASSIGNBOOLEAN(articulation_palette)
  ASSIGNBOOLEAN(midi_in_controls)
  ASSIGNBOOLEAN(playback_controls)
  ASSIGNBOOLEAN(console_pane)
  ASSIGNBOOLEAN(lyrics_pane)
  ASSIGNBOOLEAN(visible_directive_buttons)
  ASSIGNBOOLEAN(autoupdate)
  ASSIGNBOOLEAN(toolbar)
  ASSIGNBOOLEAN(notation_palette)
  ASSIGNBOOLEAN(rhythm_palette)
  ASSIGNBOOLEAN(object_palette)
  ASSIGNBOOLEAN(saveparts)
  //g_print ("Timeout %d \n", prefs->autosave_timeout);

  /* Now write it all to denemorc */
  writeXMLPrefs (prefs);
}

static void
midi_audio_tab_update(GtkWidget *box, gpointer data)
{
  struct audio_callback_data *cbdata = (struct audio_callback_data *) data;

  gchar const *audio_driver = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(cbdata->audio_driver)->entry));
  gchar const *midi_driver = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(cbdata->midi_driver)->entry));

#ifdef _HAVE_PORTAUDIO_
  gtk_widget_set_visible(cbdata->portaudio_settings, strcmp(audio_driver, "PortAudio") == 0);
#endif
}

void
preferences_change (GtkAction *action, gpointer param)
{
  DenemoGUI *gui = Denemo.gui;
  GtkWidget *dialog;
  GtkWidget *label;
  GtkWidget *main_vbox;
  GtkWidget *autosave;
  GtkWidget *autosave_timeout;
  GtkWidget *maxhistory;
  GtkWidget *notebook;
  GtkWidget *hbox;
  GtkWidget *portaudio_settings;

  static struct callbackdata cbdata;
  g_assert (gui != NULL);


  // these lists need to be initialized the first time this function is called
  // FIXME: the lists are never freed...
  if (!audio_backend_list) {
    audio_backend_list = g_list_append(audio_backend_list, (gpointer)"dummy");
    audio_driver_option_list = g_list_append(audio_driver_option_list, (gpointer)"none");
#ifdef _HAVE_JACK_
    audio_backend_list = g_list_append(audio_backend_list, (gpointer)"jack");
    audio_driver_option_list = g_list_append(audio_driver_option_list, (gpointer)"JACK");
#endif
#ifdef _HAVE_PORTAUDIO_
    audio_backend_list = g_list_append(audio_backend_list, (gpointer)"portaudio");
    audio_driver_option_list = g_list_append(audio_driver_option_list, (gpointer)"PortAudio");
#endif

    midi_backend_list = g_list_append(midi_backend_list, (gpointer)"dummy");
    midi_driver_option_list = g_list_append(midi_driver_option_list, (gpointer)"none");
#ifdef _HAVE_JACK_
    midi_backend_list = g_list_append(midi_backend_list, (gpointer)"jack");
    midi_driver_option_list = g_list_append(midi_driver_option_list, (gpointer)"JACK");
#endif
#ifdef _HAVE_ALSA_
    midi_backend_list = g_list_append(midi_backend_list, (gpointer)"alsa");
    midi_driver_option_list = g_list_append(midi_driver_option_list, (gpointer)"ALSA Sequencer");
#endif
#ifdef _HAVE_PORTAUDIO_
    midi_backend_list = g_list_append(midi_backend_list, (gpointer)"portmidi");
    midi_driver_option_list = g_list_append(midi_driver_option_list, (gpointer)"PortMidi");
#endif
  }


  dialog = gtk_dialog_new_with_buttons (_("Preferences - Denemo"),
                                        GTK_WINDOW (Denemo.window),
                                        (GtkDialogFlags) (GTK_DIALOG_MODAL |
                                                          GTK_DIALOG_DESTROY_WITH_PARENT),
                                        GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                        GTK_STOCK_CANCEL, GTK_STOCK_CANCEL,
                                        NULL);

  gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);

  notebook = gtk_notebook_new ();
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), notebook, TRUE,
                      TRUE, 0);
#define VBOX main_vbox

#define NEWPAGE(thelabel) \
    main_vbox = gtk_vbox_new (FALSE, 1);\
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), main_vbox, NULL);\
    gtk_notebook_set_tab_label_text (GTK_NOTEBOOK (notebook), main_vbox, _(thelabel));

#define BOOLEANENTRY(thelabel, field) \
  GtkWidget *field =\
    gtk_check_button_new_with_label (thelabel); \
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (field),\
                                (gboolean)Denemo.prefs.field);\
  gtk_box_pack_start (GTK_BOX (VBOX), field, FALSE, TRUE, 0);\
  cbdata.field = field;

#define TEXTENTRY(thelabel, field) \
  hbox = gtk_hbox_new (FALSE, 8);\
  gtk_box_pack_start (GTK_BOX (VBOX), hbox, FALSE, TRUE, 0);\
  label = gtk_label_new (_(thelabel));\
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);\
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);\
  GtkWidget *field = gtk_entry_new ();\
  gtk_entry_set_text (GTK_ENTRY (field), Denemo.prefs.field->str);\
  gtk_box_pack_start (GTK_BOX (hbox), field, TRUE, TRUE, 0);\
  cbdata.field = field;

#define PASSWORDENTRY(thelabel, field) \
  hbox = gtk_hbox_new (FALSE, 8);\
  gtk_box_pack_start (GTK_BOX (VBOX), hbox, FALSE, TRUE, 0);\
  label = gtk_label_new (_(thelabel));\
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);\
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);\
  GtkWidget *field = gtk_entry_new ();\
  gtk_entry_set_visibility(GTK_ENTRY(field), FALSE);\
  gtk_entry_set_invisible_char(GTK_ENTRY(field), '*');\
  gtk_entry_set_text (GTK_ENTRY (field), Denemo.prefs.field->str);\
  gtk_box_pack_start (GTK_BOX (hbox), field, TRUE, TRUE, 0);\
  cbdata.field = field;

#define INTENTRY(thelabel, field) \
  hbox = gtk_hbox_new (FALSE, 8);\
  gtk_box_pack_start (GTK_BOX (VBOX), hbox, FALSE, TRUE, 0);\
  label = gtk_label_new (thelabel);\
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);\
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);\
  field = gtk_spin_button_new_with_range (1, 50, 1.0);\
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (field), Denemo.prefs.field);\
  gtk_box_pack_start (GTK_BOX (hbox), field, FALSE, FALSE, 0);\
  cbdata.field = field;

#define ENTRY_LIMITS(thelabel, field, min, max, step)   \
  hbox = gtk_hbox_new (FALSE, 8);\
  gtk_box_pack_start (GTK_BOX (VBOX), hbox, FALSE, TRUE, 0);\
  label = gtk_label_new (thelabel);\
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);\
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);\
  GtkWidget *field = gtk_spin_button_new_with_range (min, max, step);\
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (field), Denemo.prefs.field);\
  gtk_box_pack_start (GTK_BOX (hbox), field, FALSE, FALSE, 0);\
  cbdata.field = field;

#define INTENTRY_LIMITS(thelabel, field, min, max) ENTRY_LIMITS(thelabel, field, min, max, 0.1)

#define DOUBLEENTRY_LIMITS  ENTRY_LIMITS

#define BUTTON(thelabel, field, thecallback, data) \
  hbox = gtk_hbox_new (FALSE, 8);\
  gtk_box_pack_start (GTK_BOX (vbox1), hbox, FALSE, FALSE, 0);\
  GtkWidget *field = gtk_button_new_with_label(thelabel);\
  gtk_box_pack_start (GTK_BOX (vbox1), field, FALSE, FALSE, 0);\
  g_signal_connect (G_OBJECT (field), "clicked",\
  G_CALLBACK (thecallback), (gpointer) data);

#define COMBOBOX(thelable, field, thelist, settext)\
  hbox = gtk_hbox_new (FALSE, 8);\
  gtk_box_pack_start (GTK_BOX (VBOX), hbox, FALSE, TRUE, 0);\
  label = gtk_label_new (thelable);\
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);\
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);\
  hbox = gtk_hbox_new (FALSE, 8);\
  gtk_box_pack_start (GTK_BOX (VBOX), hbox, FALSE, TRUE, 0);\
  GtkWidget *field = gtk_combo_new ();\
  gtk_combo_set_popdown_strings (GTK_COMBO (field), thelist);\
  gtk_box_pack_start (GTK_BOX (hbox), field, FALSE, FALSE, 0);\
  gtk_entry_set_text\
    (GTK_ENTRY (GTK_COMBO (field)->entry),  settext);\
  gtk_entry_set_editable\
    (GTK_ENTRY (GTK_COMBO (field)->entry), FALSE);\
  gtk_widget_show (field);\
  cbdata.field = field;


  /*
   * Note entry settings
   */

  NEWPAGE("View");
  BOOLEANENTRY("Highlight the cursor", cursor_highlight);

  BOOLEANENTRY("Display general toolbar", toolbar);
  BOOLEANENTRY("Display Note/Rest entry toolbar", notation_palette);
  BOOLEANENTRY("Display articulation palette", articulation_palette);
  BOOLEANENTRY("Display Controls for Incoming MIDI signals", midi_in_controls);
  BOOLEANENTRY("Display Controls for Playback", playback_controls);

  BOOLEANENTRY("Display console pane", console_pane);
  BOOLEANENTRY("Display lyrics pane", lyrics_pane);
  BOOLEANENTRY("Display Titles. Controls etc", visible_directive_buttons);

  BOOLEANENTRY("Display Music Snippets", rhythm_palette);
  BOOLEANENTRY("Display menu of objects toolbar", object_palette);
  INTENTRY_LIMITS(_("% Zoom"), zoom, 1, 100);
  INTENTRY_LIMITS(_("% of display height per system"), system_height, 1, 100);

  /*
   * Pitch Entry Parameters
   */
  NEWPAGE("Pitch Entry");

  TEXTENTRY("Temperament", temperament)
  BOOLEANENTRY("Use Overlays", overlays);
  BOOLEANENTRY("Continuous Entry", continuous);

  /*
   * Preferences to do with commands
   */
  NEWPAGE("Command Behavior");
  TEXTENTRY("Profile", profile)
  //  TEXTENTRY("Strict", strictshortcuts)
  BOOLEANENTRY("Apply commands to selection if present", applytoselection);
  BOOLEANENTRY("Allow Quick Setting of Shortcuts", quickshortcuts);


  BOOLEANENTRY("Strict Shortcuts", strictshortcuts);

  /*
   * External (Helper) Programs
   */
  NEWPAGE("Externals");

  TEXTENTRY("Path to Lilypond", lilypath)
  TEXTENTRY("Pdf Viewer", pdfviewer)
  TEXTENTRY("File/Internet Browser", browser)

  TEXTENTRY("Image Viewer", imageviewer)

  TEXTENTRY("Audio Player", audioplayer)

  TEXTENTRY("Default Save Path", denemopath)
  BOOLEANENTRY("Update the command set on startup", autoupdate);
   /*
   * Misc Menu
   */
  NEWPAGE("Misc");
  BOOLEANENTRY("Use Denemo modally", modal);
  BOOLEANENTRY("Re-use last settings on startup", persistence);
  DOUBLEENTRY_LIMITS(_("Playback Display Refresh"), display_refresh, 0.001, 0.5, 0.002);
  INTENTRY_LIMITS(_("Page Turn Steps"), animation_steps, 1, 200);

  INTENTRY_LIMITS(_("Excerpt Resolution"), resolution, 72, 600);
  BOOLEANENTRY("Enable Thumbnails", enable_thumbnails);
  INTENTRY(_("Max recent files"), maxhistory)
  TEXTENTRY("User Name", username)
  PASSWORDENTRY("Password for Denemo.org", password)


  hbox = gtk_hbox_new (FALSE, 8);
  gtk_box_pack_start (GTK_BOX (main_vbox), hbox, FALSE, TRUE, 0);
  autosave = gtk_check_button_new_with_label (_("Autosave every"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (autosave),
                                Denemo.prefs.autosave);
  gtk_box_pack_start (GTK_BOX (hbox), autosave, FALSE, FALSE, 0);

  autosave_timeout = gtk_spin_button_new_with_range (1, 50, 1.0);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (autosave_timeout),
                             Denemo.prefs.autosave_timeout);
  gtk_widget_set_sensitive (autosave_timeout, Denemo.prefs.autosave);
  gtk_box_pack_start (GTK_BOX (hbox), autosave_timeout, FALSE, FALSE, 0);
  g_debug("autosave %p\n", autosave);
  label = gtk_label_new (_("minute(s)"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  g_signal_connect (GTK_OBJECT (autosave),
                    "toggled", G_CALLBACK (toggle_autosave), autosave_timeout);


  BOOLEANENTRY("Autosave Parts", saveparts);

  static struct audio_callback_data audio_cbdata;

  NEWPAGE("Audio/MIDI");

  BOOLEANENTRY("Play back entered notes immediately", immediateplayback);
  INTENTRY_LIMITS(_("Pitch Spelling Channel"), pitchspellingchannel, 0, 15);
  INTENTRY_LIMITS(_("Pitch Spelling Program"), pitchspellingprogram, 0, 127);

  BOOLEANENTRY("Auto-start midi in", startmidiin);

  INTENTRY_LIMITS(_("% MIDI-in Dynamic Compression"), dynamic_compression, 1, 100);


  GList *item = g_list_find_custom(audio_backend_list, Denemo.prefs.audio_driver->str, (GCompareFunc)strcmp);
  gint index = g_list_position(audio_backend_list, item);
  gchar *driver = g_list_nth_data(audio_driver_option_list, index);

  COMBOBOX("Audio backend", audio_driver, audio_driver_option_list, driver);
  g_signal_connect(G_OBJECT(GTK_COMBO(audio_driver)->entry), "changed", G_CALLBACK(GTK_SIGNAL_FUNC(midi_audio_tab_update)), &audio_cbdata);

  /*
   * JACK settings
   */
  // TODO

  /**
   * PortAudio settings
   */
#ifdef _HAVE_PORTAUDIO_

#undef VBOX
#define VBOX portaudio_settings
  portaudio_settings = gtk_vbox_new(FALSE, 8);
  gtk_box_pack_start(GTK_BOX (main_vbox), portaudio_settings, FALSE, TRUE, 0);

  INTENTRY_LIMITS(_("Sample rate"), portaudio_sample_rate, 0, 96000);
  INTENTRY_LIMITS(_("Period size"), portaudio_period_size, 0, 2048);

#undef VBOX
#define VBOX main_vbox

#endif


  item = g_list_find_custom(midi_backend_list, Denemo.prefs.midi_driver->str, (GCompareFunc)strcmp);
  index = g_list_position(midi_backend_list, item);
  driver = g_list_nth_data(midi_driver_option_list, index);

  COMBOBOX("MIDI backend", midi_driver, midi_driver_option_list, driver);
  g_signal_connect(G_OBJECT(GTK_COMBO(midi_driver)->entry), "changed", G_CALLBACK(GTK_SIGNAL_FUNC(midi_audio_tab_update)), &audio_cbdata);


  /*
   * FluidSynth settings
   */
  TEXTENTRY("Soundfont", fluidsynth_soundfont)

  hbox = gtk_hbox_new(FALSE, 8);
  gtk_box_pack_start(GTK_BOX(VBOX), hbox, FALSE, TRUE, 0);
  GtkWidget *button = gtk_button_new_with_label(_("Choose Soundfont"));
  gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);

  // FIXME
//  gtk_signal_connect (GTK_OBJECT (button), "clicked",
//    GTK_SIGNAL_FUNC (choose_sound_font), fluidsynth_soundfont);
  gtk_widget_show(button);

  BOOLEANENTRY("Enable Reverb on soundfont", fluidsynth_reverb)
  BOOLEANENTRY("Enable Chorus on soundfont", fluidsynth_chorus)


  gtk_widget_show_all (dialog);

  audio_cbdata.audio_driver = cbdata.audio_driver;
  audio_cbdata.midi_driver = cbdata.midi_driver;
  audio_cbdata.portaudio_settings = portaudio_settings;

  midi_audio_tab_update(NULL, (gpointer*) &audio_cbdata);


#define SETCALLBACKDATA(field) \
  cbdata.field = field;

  cbdata.prefs = &Denemo.prefs;
  SETCALLBACKDATA(autosave);
  SETCALLBACKDATA(autosave_timeout);
  SETCALLBACKDATA(maxhistory);


  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      set_preferences (&cbdata);

      // stop playback and restart audio subsystem
      // FIXME: only do this when audio settings actually changed
      midi_stop();
      audio_shutdown();
      audio_initialize(cbdata.prefs);
    }

  gtk_widget_destroy (dialog);
}

