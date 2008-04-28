/* draw.h
 * prototypes for actual drawing functions
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 1999-2005 Matthew Hiller, Adam Tee
 */

#include <gtk/gtk.h>
#include <denemo/denemo.h> 

gint
scorearea_configure_event (GtkWidget * widget, GdkEventConfigure * event,
			    DenemoGUI *si);

gint
scorearea_expose_event (GtkWidget * widget, GdkEventExpose * event,
			gpointer data);

void
draw_measures (GtkWidget * widget, DenemoGUI *si);

void
display_string (gchar *str,  DenemoGUI *si);
