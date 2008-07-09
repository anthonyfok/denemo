/**
 * bookmarks.cpp
 *
 * Implementation of scorebookmarks
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 2005 Adam Tee
 */

#include "bookmarks.h"
#include "moveviewport.h"



/**
 *  Find the specified bookmark and go to it.
 * 
 * @param gui pointer to the DenemoGUI structure
 * @param bmbar the bookmarks measure
 * @param bmstaff the bookmarks staff
 */
static void
findbookmark (DenemoGUI * gui, gint bmbar, gint bmstaff)
{
  g_assert (gui != NULL);
  set_currentmeasurenum (gui, bmbar);
  set_currentstaffnum (gui, bmstaff);
  //FIXME make these return success or failure & inform of deleted bookmark positions
}

/**
 *  Callback to add a new book mark to the list
 *  
 *  Uses the current measure and staff
 */
void
addbookmark (GtkAction * action)
{
  DenemoGUI *gui = Denemo.gui;
  g_assert (gui != NULL);
  DenemoScore *si = gui->si;
  Bookmark *bm = (Bookmark *) g_malloc0 (sizeof (Bookmark));
  bm->bar = si->currentmeasurenum;
  bm->staff = si->currentstaffnum;
  if(si->bookmarks)
    bm->id = g_list_length(si->bookmarks);

  si->bookmarks = g_list_append (si->bookmarks, bm);

  //g_print ("Bar %d, Staff %d, \n", bm->bar, bm->staff);
}


void
deletebookmarks (GtkAction * action)
{
  DenemoGUI *gui = Denemo.gui;
  GList *g;
  DenemoScore *si = gui->si;
  for(g=si->bookmarks;g;g=g->next) {
    g_free(g->data);
  }
  si->bookmarks = NULL;
}
void
nextbookmark (GtkAction * action)
{
  DenemoGUI *gui = Denemo.gui;
 if(gui->si->bookmarks){
   GList *g = g_list_nth(gui->si->bookmarks, gui->si->currentbookmark+1);
   if(g == NULL) {
     gdk_beep();//wrap around
     g = gui->si->bookmarks;
     gui->si->currentbookmark=-1;
   }
   Bookmark *bm = g->data;
   findbookmark (gui, bm->bar, bm->staff);
   gui->si->currentbookmark++;
 }
}
void
prevbookmark (GtkAction * action)
{
  DenemoGUI *gui = Denemo.gui;
 if(gui->si->bookmarks){
   GList *g = g_list_nth(gui->si->bookmarks, gui->si->currentbookmark-1);
   if(g == NULL) {
     gdk_beep();//wrap around
     g = g_list_last(gui->si->bookmarks);
     gui->si->currentbookmark=g_list_length(gui->si->bookmarks)+1;
   }
   Bookmark *bm = g->data;
   findbookmark (gui, bm->bar, bm->staff);
   gui->si->currentbookmark--;
 }
}

/**
 * Dialog to go to a specific bookmark
 *
 */
void
gotobookmark (GtkAction * action)
{
  DenemoGUI *gui = Denemo.gui;
  g_assert (gui != NULL);
  if(gui->si->bookmarks==NULL) {
    warningdialog("No bookmarks are present in this movement");
    return;
  }
  GtkWidget *dialog;
  GtkWidget *combobox;
  GList *strings = NULL, *tmp;
  dialog = gtk_dialog_new_with_buttons (_("Goto Bookmark"),
					GTK_WINDOW (Denemo.window),
					(GtkDialogFlags) (GTK_DIALOG_MODAL |
							  GTK_DIALOG_DESTROY_WITH_PARENT),
					GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
					GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
					NULL);


  combobox = gtk_combo_new ();
  //g_print ("List length %d\n", g_list_length (gui->si->bookmarks));
#define FORMAT "%d : Staff %d : Bar %d"
  for (tmp = gui->si->bookmarks; tmp; tmp = tmp->next)
    {
      Bookmark *bm = (Bookmark *) tmp->data;
      char *tmpstring = g_strdup_printf(FORMAT,//FIXME insane use of sscanf see below
		bm->id, bm->staff, bm->bar);

      strings = g_list_append (strings, tmpstring);
    }

  gchar *text = g_strdup ((gchar *) g_list_nth_data (strings, 0));

  gtk_combo_set_popdown_strings (GTK_COMBO (combobox), strings);
  gtk_entry_set_text (GTK_ENTRY (GTK_COMBO (combobox)->entry), text);
  g_free (text);

  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
		      combobox, TRUE, TRUE, 0);
  gtk_widget_show (combobox);
  gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_MOUSE);
  gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
  gtk_widget_show_all (dialog);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      gint bmid, bms, bmb;
      gchar *tmp =
	(gchar *)
	gtk_entry_get_text (GTK_ENTRY (GTK_COMBO (combobox)->entry));
      sscanf (tmp, FORMAT, &bmid, &bms, &bmb);
      //g_print ("bmid %d\n", bmid);
      findbookmark (gui, bmb, bms);

    }
  // g_list_free(strings);
  gtk_widget_destroy (dialog);

}
