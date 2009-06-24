/* lyric.c
 *
 * Functions for the manipulations of lyrics
 *
 * for Denemo, a gtk+ frontend for GNU Lilypond
 * (c)2009 Richard Shann
 */
#include "lyric.h"
#include "objops.h"
#include "staffops.h"
#include "utils.h"

static GtkWidget *DummyVerse;/* a non-existent verse */

static gboolean lyric_keypress(GtkWidget *w, GdkEventKey *event) {
DenemoGUI *gui = Denemo.gui;
 if(gtk_text_buffer_get_modified(gtk_text_view_get_buffer(GTK_TEXT_VIEW(w)))) {
   score_status(Denemo.gui, TRUE);
   gtk_widget_queue_draw (gui->scorearea);
   gtk_text_buffer_set_modified(gtk_text_view_get_buffer(GTK_TEXT_VIEW(w)), FALSE);
 }
 return FALSE;
}
gboolean lyric_change(GtkTextBuffer *buffer) {
  DenemoGUI *gui = Denemo.gui;
  score_status(gui, TRUE);
  gtk_widget_queue_draw (gui->scorearea);
  return FALSE;
}

static GtkWidget *new_lyric_editor(void) {
 GtkWidget *view = gtk_text_view_new ();
 // g_signal_connect (G_OBJECT (view), "key-press-event", G_CALLBACK (lyric_keypress), NULL);
 //g_signal_connect (G_OBJECT (gtk_text_view_get_buffer(view)), "changed", G_CALLBACK (lyric_keypress), NULL);

 return view;
}

DenemoObject *
newlyric (gint baseduration, gint numdots, gchar *lys) {
  g_warning("Not implemented");
  return NULL;
}

static void
switch_page (GtkNotebook *notebook, GtkNotebookPage *page,  guint pagenum, DenemoStaff *staff) {
    gtk_widget_queue_draw (Denemo.gui->scorearea);
    staff->currentverse = g_list_nth(staff->verses, pagenum);
}
GtkWidget * add_verse_to_staff(DenemoScore *si, DenemoStaff *staff) {
  GtkWidget *notebook, *textview;
  if(staff->verses==NULL) {
    notebook = gtk_notebook_new();
    gtk_widget_show(notebook);
    g_signal_connect (G_OBJECT(notebook), "switch_page", G_CALLBACK(switch_page), staff);
    if(si->lyricsbox==NULL)
      install_lyrics_preview(si, gtk_widget_get_parent(gtk_widget_get_parent(Denemo.gui->scorearea)));//FIXME we need a proper way of getting to the top vbox, that will not break when scorearea is moved in the widget hierarchy.
    gtk_box_pack_start(GTK_BOX(si->lyricsbox), notebook, TRUE, TRUE, 0);
  } else {
    GtkWidget *w = staff->verses->data;
    notebook = gtk_widget_get_parent(w);
  }
  // if(staff->currentverse)    
  //  gtk_widget_hide(staff->currentverse->data); 
  //if(si->currentstaff && si->currentstaff->data == staff)
  //  gtk_widget_show(staff->currentverse->data);
  textview = new_lyric_editor();
  gtk_widget_show(textview);
  staff->verses = g_list_append(staff->verses, textview);
  staff->currentverse = g_list_last(staff->verses);
  //  g_print("Setting verse to %p\n", staff->currentverse);
  gint pagenum = gtk_notebook_append_page (GTK_NOTEBOOK (notebook), textview, NULL);
  gtk_notebook_set_current_page (GTK_NOTEBOOK(notebook), pagenum);
  gchar *tablabel = g_strdup_printf("Verse %d", pagenum+1);
  gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(notebook), textview, tablabel);
  g_free(tablabel);
  if(pagenum)
    gtk_notebook_set_show_tabs (GTK_NOTEBOOK(notebook), TRUE);
  return textview;
}

void add_verse(GtkAction *action, gpointer param) {
DenemoGUI *gui = Denemo.gui;
DenemoScore *si = gui->si;
 if(gui->si->currentstaff) {
   DenemoStaff *staff = si->currentstaff->data;
   add_verse_to_staff(si, staff);
   gtk_widget_show(staff->currentverse->data);
   g_signal_connect (G_OBJECT (gtk_text_view_get_buffer (staff->currentverse->data)), "changed", G_CALLBACK (lyric_change), NULL);
 }
}

gchar *get_text_from_view(GtkWidget *textview) {
  GtkTextIter startiter, enditer; 
  GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(textview));    
  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(buffer), &startiter);
  gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(buffer), &enditer);
  return gtk_text_buffer_get_text (GTK_TEXT_BUFFER(buffer), &startiter, &enditer, FALSE);
}



static gchar *lyric_iterator(GtkWidget *textview, gint count) {
  static  const gchar *next;
  static gchar *lyrics;
  static GString *gs;
  static start_count;
  if(gs==NULL)
    gs = g_string_new("");
  if(textview==NULL) {
    pango_scan_string(&next, gs);
    if(gs->len)
      return gs->str;
    else
      return NULL;
  }
  if(textview != DummyVerse) {
    if(lyrics) g_free(lyrics);
    lyrics = get_text_from_view(textview);
    next = lyrics;
    start_count = count;
    while(count--)
      pango_scan_string(&next, gs);
 }
  return NULL;
}


gchar *next_syllable(gint count) {
  return lyric_iterator(NULL, count);
}


/* rename reset_lyrics */
void reset_lyrics(DenemoStaff *staff, gint count) {
  if(DummyVerse==NULL)
    DummyVerse = gtk_text_view_new();
  if(staff && staff->currentverse)
    lyric_iterator(staff->currentverse->data, count);
  else lyric_iterator(DummyVerse, count);
}



void install_lyrics_preview(DenemoScore *si, GtkWidget *top_vbox){ 
  if(si->lyricsbox==NULL)
    si->lyricsbox = gtk_vbox_new (FALSE, 1);//box to hold notebook of textview widgets
  gtk_box_pack_start (GTK_BOX (top_vbox), si->lyricsbox, FALSE, TRUE, 0);
  gtk_widget_show(si->lyricsbox);
}

/* hide the notebook of verses for the current staff */
void hide_lyrics(void) {
DenemoGUI *gui = Denemo.gui;
DenemoScore *si = gui->si;
 if(gui->si->currentstaff && ((DenemoStaff *)gui->si->currentstaff->data)->currentverse)
   gtk_widget_hide(gtk_widget_get_parent(((DenemoStaff *)gui->si->currentstaff->data)->currentverse->data));
}

/* show the notebook of verses for the current staff */
void show_lyrics(void) {
DenemoGUI *gui = Denemo.gui;
DenemoScore *si = gui->si;
 if(si->currentstaff && ((DenemoStaff *)si->currentstaff->data)->currentverse)
   gtk_widget_show(gtk_widget_get_parent(((DenemoStaff *)si->currentstaff->data)->currentverse->data));
}

/* hide the notebooks of verses for the non-current staffs */
void select_lyrics(void) {
DenemoGUI *gui = Denemo.gui;
DenemoScore *si = gui->si;
 GList *current = si->thescore;
 for(;current;current = current->next) {
   if(current != si->currentstaff && ((DenemoStaff *)current->data)->verses)
       gtk_widget_hide(gtk_widget_get_parent(((DenemoStaff *)current->data)->verses->data));
 }
}
