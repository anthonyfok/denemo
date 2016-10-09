#ifndef __MARKUP_VIEW_H__
#define __MARKUP_VIEW_H__

#include <denemo/denemo.h>
#include "export/print.h"
void
install_markup_preview (GtkWidget * top_vbox, gchar *tooltip);
void
markupview_finished (G_GNUC_UNUSED GPid pid, gint status, gboolean print);
gchar *get_markup_from_user (gchar *instruction, gchar *prior_context, gchar *post_context, gchar *initial_markup);
#endif
