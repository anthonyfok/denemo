/* exportmudela.h
 * Header file for mudela exportation

 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 2000-2005 Matthew Hiller, Adam Tee
 */
#ifndef EXPORTMUDELAH
#define EXPORTMUDELAH

#include <gtk/gtk.h>
#include <stdio.h>

void exportlilypond (gchar * thefilename,  DenemoGUI *gui, gboolean all_movements);

void export_lilypond_parts(char *filename, DenemoGUI *gui);
void export_lilypond_part(char *filename, DenemoGUI *gui, gboolean all_movements);

/* generate lilypond text for the object passed in - the string should
   be g_freed by the caller when finished with*/
gchar *generate_lily (objnode *obj);
void refresh_lily_cb (GtkAction *action, DenemoGUI *gui);
void force_lily_refresh(DenemoGUI *gui);
void toggle_lily_visible_cb (GtkAction *action, gpointer param);

void custom_lily_cb (GtkAction *action, gpointer param);

void delete_lily_cb (GtkAction *action, gpointer param);
void set_lily_error(gint line, gint column, DenemoGUI *gui);
void highlight_lily_error(DenemoGUI *gui);
#endif
