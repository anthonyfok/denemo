/* help.cpp
 * implements the stuff under Help in the menubar
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 2000-2005 Matthew Hiller
 */

#include <denemo/denemo.h>
#include "config.h"
#include "utils.h"
#include <string.h>		/* for strlen */
/* The tutorial mentioned that the actual gchar * held within a
 * GtkText widget needs to be freed.  I don't do such a free, though,
 * so I think this function has a memory leak in it. */

/** 
 * Create the about dialog
 *
 */
void
about (GtkAction * action, gpointer callback_data)
{
  GtkWidget *dialog;
  DenemoGUI *gui = (DenemoGUI *)callback_data;
  const char *authors[] = { "Richard Shann", "Jeremiah Benham", "Matthew Hiller", "Adam Tee", NULL };

  dialog = gtk_about_dialog_new ();
  gtk_about_dialog_set_name (GTK_ABOUT_DIALOG (dialog), _("Denemo"));
  gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (dialog),
				 _("The GNU graphical score editor"));
  gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (dialog), VERSION);
  gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog),
				"http://denemo.org");
  gtk_about_dialog_set_website_label (GTK_ABOUT_DIALOG (dialog),
				      _("Denemo website"));
  gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (dialog),
				_
				("(c) 1999 - 2008 Matthew Hiller, Adam Tee, and others.\n\n\
http://www.denemo.org\n\n\
This program is licensed under the terms of the GNU\n\
General Public License and is provided with absolutely\n\
NO WARRANTY; see the file COPYING for details."));


  gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);
  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(gui->window));
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}


/** 
 * Function to allow browsing the user manual
 * uses the given web browser to display the manual
 */
void
browse_manual (GtkAction * action, DenemoGUI * gui)
{
  gboolean retval;
  GError *error = NULL;

  /* get the uri to the manual */
  gchar *manualpath = g_build_filename (get_data_dir (), "manual",
                                        "denemo-manual.html", NULL);
  gchar *manualuri = g_filename_to_uri (manualpath, NULL, NULL);

  /* check that the browser exists */
  gchar *browserpath = g_find_program_in_path (Denemo.prefs.browser->str);
  if (browserpath == NULL)
    {
      /* show a warning dialog */
      GtkWidget *dialog =
        gtk_message_dialog_new (GTK_WINDOW (gui->window),
                                GTK_DIALOG_DESTROY_WITH_PARENT,
                                GTK_MESSAGE_WARNING,
                                GTK_BUTTONS_OK,
                                _("Could not find %s in the path"),
                                Denemo.prefs.browser->str);
      gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
                                                _("Please edit the chosen "
                                                  "browser in the "
                                                  "preferences."));
      gtk_dialog_run (GTK_DIALOG (dialog));

      /* free the memory and return */
      gtk_widget_destroy (dialog);
      g_free (manualpath);
      g_free (manualuri);
      return;
    }

  /* spawn the process to show the manual */
  gchar *argv[] = {Denemo.prefs.browser->str,
                   manualuri,
                   NULL};
  retval = g_spawn_async (NULL, argv, NULL,
			  G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error);
  if (!retval)
    {
      g_message (_("Could not execute specified web browser: %s\n"),
		 error->message);
      g_error_free (error);
    }

  /* free the memory */
  g_free (browserpath);
  g_free (manualpath);
  g_free (manualuri);
}
