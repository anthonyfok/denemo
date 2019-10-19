//gcc -Wall pageswitcher.c -o pageswitcher `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags --libs evince-view-3.0`
//Tested on debian 3.16.0 and GTK3.14.5
//      pageswitcher.c
//
//      Copyright 2019 Richard Shann
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 3 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include<gtk/gtk.h>
#include <evince-view.h>

#if !((GTK_MAJOR_VERSION==3) && (GTK_MINOR_VERSION>=18))
#define gtk_overlay_reorder_overlay(a,b,c) g_critical("Must be Gtk version 3.18 or greater"), exit(-1)
#endif


#define SPOT_SIZE (10) //size of spot for picking out an annotation
typedef struct Location {
     gint  adj;
     gint x, y;
} Location;
typedef struct Annotation {
     gchar *annotation;
     gint page;
     gint x, y;
     double r,g,b,a;
     gchar *font;
} Annotation;

typedef struct Page {
     EvDocumentModel *model;
     gint pnum; //page num starting from 0
} Page;

typedef enum Status { Forward, Backward, Skip} Status;

Status status = Forward;

static const gchar *music_home;//the directory where your scores are stored
static GtkWidget *score_window;//the window containing the two halves of the score
static GList *repeat_locations = NULL;// a list of locations for use when a repeat starts in the middle/lower half of a page
static GList *annotations = NULL;// a list of annotations to be displayed on the score
static GtkWidget *lh_view, *rh_view;//the two pages of the score - the one that is not currently being read is switched to the next page to play 
static GtkWidget *eventbox1, *eventbox2;
static Page page1, page2;//the two Page objects
static Page *lh_page, *rh_page;//pointers to the two Page objects.
static gint num_pages;

static  gint x=0;//Window size and position on screen: 0 0 is top left corner of screen
static  gint y=0;
static  gint width=0;
static  gint height=0;
static gdouble aspect_ratio=1.414; //A4 page size default
static  EvDocumentModel *model1, *model2;

static void next_page (void);//turn to the next page
static void previous_page (void);//turn back one page
static void goto_page (gint page);//page number starting at 1

static gchar *markings_file = NULL;//full path to file for storing repeat marks and annotations for currently loaded score
static gboolean markings_unsaved = FALSE; //TRUE when user has created or deleted markings in the current score 
static gint page_on = 'c', page_back = 'a', skip_page = 'b';
static guint64 last_pedal_time = 0;

static void goto_page (gint page);


static gchar *help_text =
"The music page-turner allows hands-free turning of pages as you play from a musical score."
"\nIt also allows for annotations to be added to the score - anything from reminder accidentals"
" to the spectacles O^O that warn of a tricky passage to take special care with."
" The screen is devoted to two pages so the one you are going to play next is always kept in view"
"\nControl of page turning is via foot pedals which send key press signals just like those of a normal keyboard."
" For example, when you are on the right page you press the right foot pedal to go on"
" and the left changes to the next page."
"The left pedal is for repeats: it moves backwards instead of forwards "
" to be ready for the start of a repeat.\n"
" The center pedal moves the next-to-play page onwards for the case where you need to skip forward over a page or pages."
"\nTo mark up the score for reminders or proof-reading right click with the mouse at the point where you want the annotation to be placed"
" and choose \"Annotate here\" from the menu. You can drag an annotation if it is misplaced."
"\nThe menu also lets you delete an annotation, navigate the score etc."
"\n Currently Keypresses '%c' to go one page on, '%c' to go back for a repeat and '%c' to skip forward."
"\nTo change the defaults you can pass values on the command line - type pageturner --help to see the command line usage."
;
static void show_help (void)
{
   GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW (gtk_window_new(GTK_WINDOW_TOPLEVEL)), (GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT), GTK_MESSAGE_INFO, GTK_BUTTONS_OK, 
   help_text,
   page_on, page_back, skip_page);
   gtk_widget_show_all (dialog);
   g_signal_connect (dialog, "response", G_CALLBACK (gtk_widget_destroy), NULL);
}

static guint64 elapsed_time ()
{
   gint64 thistime = g_get_monotonic_time ();
   guint64 elapsed = 
   (thistime - last_pedal_time) / 1000; //in milliseconds
   last_pedal_time = g_get_monotonic_time ();
   return elapsed;
}

static void free_annotation (Annotation *a)
{
   g_free (a->annotation);
   g_free (a->font);
   g_free (a);
}
//load repeat marks and annotations from the file associated with the opened pdf score
static void load_markings (gchar *pdfname)
{
   if (markings_file) 
      g_free (markings_file);
   markings_file = g_strdup_printf ("%s%s", pdfname, ".marks");
   FILE *fp = fopen (markings_file, "r");
   if (fp)
      {
         gint window_width, window_height;
         if (2==fscanf (fp, "%d%d", &window_width, &window_height))
            {
               if ((width != window_width) || (height != window_height))
                  g_warning ("Height and Width of markings file does not match current window - expect misplaced marks %d %d %d %d\n", width, height, window_width, window_height);
               gint page, x, y;  
               gchar text1[100], text2[100];
               *text1 = *text2 = 0;
               double r,g,b,a;   
               while ( 7 == fscanf (fp, "%d%d%d%lf%lf%lf%lf\n", &page, &x, &y, &r, &g, &b, &a))
                                 {
                                    Annotation *ann = (Annotation*)g_malloc (sizeof (Annotation));
                                    ann->x = x;
                                    ann->y = y;
                              
                                    ann->page = page;
                                    ann->r = r;
                                    ann->g = g;
                                    ann->b = b;
                                    ann->a = a;
                                    if (fgets (text1, 100, fp))
                                       ann->font = g_strdup (text1);
                                    else
                                       ann->font = g_strdup ("Times 12\n");  
                                    
                                    if (fgets (text2, 100, fp))
                                       ann->annotation = g_strdup (text2);
                                    else
                                       ann->annotation = g_strdup ("???\n"); 
                                    *(ann->font + strlen (ann->font) - 1) = 0;   
                                    *(ann->annotation + strlen (ann->annotation) - 1) = 0;   
                                          
                                    annotations = g_list_append (annotations, ann);
                                 } 
            }
         else  g_warning ("Corrupt markings file");
        fclose (fp);
      }
   markings_unsaved = FALSE;
}
static void
load_score (gchar *pdfname, GError ** err)
{
  GFile *file;
  file = g_file_new_for_commandline_arg (pdfname);
  gchar *uri = g_file_get_uri (file);
  EvDocument *doc = ev_document_factory_get_document (uri, err);
  if (*err)
    {
      g_warning ("Trying to read the pdf file %s gave an error: %s", uri, (*err)->message);
      gtk_widget_queue_draw (lh_view);
      gtk_widget_queue_draw (rh_view);
    }
  else
   {
     if (model1 == NULL)
       {
         model1 = ev_document_model_new_with_document (doc);
         ev_view_set_model ((EvView *) lh_view, model1);
      }
     else
      {
         ev_document_model_set_document (model1, doc);
       }
     if (model2 == NULL)
       {          
         model2 = ev_document_model_new_with_document (doc);
         ev_view_set_model ((EvView *) rh_view, model2);
       }
     else
       {
         ev_document_model_set_document (model2, doc);
       }

       for (num_pages=1;;num_pages++)
         {
            if (!ev_view_next_page ((EvView*)lh_view))
               break;
         }
      g_print ("Number of pages %d\n", num_pages);
      page1.model = model1;
      page1.pnum = 0;
      lh_page = &page1;
      ev_document_model_set_page (model1, 0); 
      
      page2.model = model2;
      page2.pnum = 1;
      rh_page = &page2;
      ev_document_model_set_page (model2, 1);
      
      g_list_free_full (repeat_locations, g_free);
      repeat_locations = NULL;
      g_list_free_full (annotations, (GDestroyNotify)free_annotation);
      annotations = NULL;
      load_markings (pdfname);
      goto_page (1);
   }

  return;
}

static void set_page (Page *p, gint num)
{
   if ((num >= num_pages) || (num <0))
      {  static gboolean once = FALSE;
         if (once)
            {
               if(num>0)
                  goto_page (1);
               else
                  goto_page (num_pages - 1);
               once = FALSE;
            }
         else
            {
               g_warning ("Attempt to go off page %d\n", num);// two presses of forward wraps round
               once = TRUE;
            }
         return;
      }
  p->pnum = num;
  ev_document_model_set_page (p->model, num);
  gtk_widget_queue_draw (lh_view);
  gtk_widget_queue_draw (rh_view);
}
static void next_page (void)
{
  switch (status)
   {
      case Forward:
         (rh_page->pnum >= lh_page->pnum)? 
         set_page (lh_page, rh_page->pnum + 1):
         set_page (rh_page, lh_page->pnum + 1);
         break;
      case Backward:
         (rh_page->pnum >= lh_page->pnum)? 
         set_page (rh_page, lh_page->pnum + 1):
         set_page (lh_page, rh_page->pnum + 1);
         break;         
      case Skip:
         (rh_page->pnum >= lh_page->pnum)? 
         set_page (lh_page, rh_page->pnum + 1):
         set_page (rh_page, lh_page->pnum + 1);
         break;         
   }
   status = Forward;
}


static void previous_page (void)
{
  switch (status)
   {
      case Forward:
      case Backward:     
         (rh_page->pnum >= lh_page->pnum)? 
         set_page (lh_page, lh_page->pnum - 1):
         set_page (rh_page, rh_page->pnum - 1);
         break;
      case Skip:
         (rh_page->pnum >= lh_page->pnum)? 
            set_page (rh_page, rh_page->pnum - 1):
            set_page (lh_page, lh_page->pnum - 1);
      break;
   }
   status = Backward;
}

static void do_skip (void)
{
  switch (status)
   {
      case Forward:
         (rh_page->pnum >= lh_page->pnum)? 
         set_page (lh_page, rh_page->pnum + 1):
         set_page (rh_page, lh_page->pnum + 1);
         break;
     case Backward: 
     case Skip:
         (rh_page->pnum >= lh_page->pnum)? 
            set_page (rh_page, rh_page->pnum + 1):
            set_page (lh_page, lh_page->pnum + 1);
     break;   
   }      
   status = Skip;
}
static void goto_page (gint page)
{
   rh_page->pnum = page;
   lh_page->pnum = page-1;
   ev_document_model_set_page (lh_page->model, lh_page->pnum);
   ev_document_model_set_page (rh_page->model, rh_page->pnum);
   
}
static void change_page (GtkSpinButton * widget)
{
   goto_page ((gint)gtk_spin_button_get_value (widget));
}

static gboolean keypress (GtkWidget *eventbox, GdkEventKey * event)
{
  guint keyval = event->keyval; 
  
 if (elapsed_time()>100)
      {
      if (keyval==page_on)
       {
         next_page();
       }

      if (keyval==skip_page)
       {
         do_skip ();
       }
      if (keyval==page_back)
       {
         previous_page();
       }
      }
 
  return FALSE;
}

static gboolean choose (gchar *filename)
{
   GtkWidget *top_window=gtk_window_new(GTK_WINDOW_TOPLEVEL);

   GtkWidget *chooser = gtk_file_chooser_dialog_new ("Choose Score",
                        GTK_WINDOW (top_window),
                        GTK_FILE_CHOOSER_ACTION_OPEN,
                        "Ok", GTK_RESPONSE_ACCEPT,
                         NULL);

   GtkFileFilter *filter = gtk_file_filter_new ();
   gtk_file_filter_set_name (filter, "PDF files");
   gtk_file_filter_add_pattern (filter, "*.pdf");
   gtk_file_filter_add_pattern (filter, "*.PDF");
   gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (chooser), filter);

   if (filename && g_file_test(filename, G_FILE_TEST_IS_DIR))
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (chooser), filename);
   if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_ACCEPT)
       {
         gchar *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
         GError *err = NULL;
         load_score (filename, &err);
      }
   gtk_widget_destroy (chooser);
  return FALSE; //remove this callback
}

static void back_to_main_window ()
{
   gtk_widget_grab_focus (score_window);
   gtk_window_present (GTK_WINDOW (score_window)); 
}

static void navigate (void)
{
   static GtkWidget *window = NULL;
   if (window) gtk_widget_destroy (window);

   window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   g_signal_connect (window, "delete-event", G_CALLBACK(gtk_widget_hide), NULL);
   GtkWidget *label = gtk_label_new ("Set Page Number:");
   GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
   gtk_container_add (GTK_CONTAINER (window), box);
   gtk_box_pack_start (GTK_BOX (box), label, FALSE, TRUE, 0);
   GtkWidget *spinner_adj = (GtkWidget *) gtk_adjustment_new (1.0, 1.0, num_pages, 1.0, 1.0, 1.0);
   GtkWidget *spinner = (GtkWidget *) gtk_spin_button_new (GTK_ADJUSTMENT(spinner_adj), 100.0, 0);
   gtk_box_pack_start (GTK_BOX (box), spinner, FALSE, TRUE, 0);
   g_signal_connect (G_OBJECT (spinner), "value-changed", G_CALLBACK (change_page), NULL);
   g_signal_connect_after (G_OBJECT (spinner),  "value-changed", G_CALLBACK (back_to_main_window), NULL);

   gtk_widget_show_all (window);
}


static Annotation *create_annotation (gchar *text, gint page, gdouble x, gdouble y, gchar *font, GdkRGBA *color)
{
    Annotation *ann;
    markings_unsaved = TRUE;
    ann = g_malloc (sizeof (Annotation));
    ann->annotation = text;
    ann->page = page;
    ann->x = x;
    ann->y = y;
    ann->r = color->red;
    ann->g = color->green;
    ann->b = color->blue;
    ann->a = color->alpha;
    ann->font = font;
    return ann;
}

static void delete_annotation (GList *annlink)
{
   
             markings_unsaved = TRUE;
             free_annotation (annlink->data);
             annotations = g_list_delete_link (annotations, annlink);
             gtk_widget_queue_draw (lh_view);
             gtk_widget_queue_draw (rh_view);
}
static gchar *fontdesc=NULL;
static void font_chosen (GtkWidget *fontchooser)
{
   fontdesc = gtk_font_chooser_get_font (GTK_FONT_CHOOSER(fontchooser));
}

static GdkRGBA colordesc = {1.0,0.0,0.0,1.0};
#if 1 //!((GTK_MAJOR_VERSION>=3) && (GTK_MINOR_VERSION>=4)) bizarrely gtk_color_button_get_rgba is declared deprecated but gtk_color_chooser_get_color is not available.
static void color_chosen (GtkWidget *colorchooser)
   {
   G_GNUC_BEGIN_IGNORE_DEPRECATIONS    
      gtk_color_button_get_rgba (GTK_COLOR_BUTTON(colorchooser), &colordesc);
   G_GNUC_END_IGNORE_DEPRECATIONS
   }                    
#else
static void color_chosen (GtkWidget *colorchooser)
   {
      gtk_color_chooser_get_color (GTK_COLOR_CHOOSER(colorchooser), &colordesc);
   }
#endif

static void insert_button_text (GtkWidget *button, GtkWidget *entry)
{
    gchar *text = g_strdup_printf ("%s%s", gtk_entry_get_text (GTK_ENTRY (entry)), gtk_button_get_label(GTK_BUTTON(button)));
    gtk_entry_set_text (GTK_ENTRY (entry), text);
    g_free (text);
}

static void create_button (GtkWidget *hbox, GtkWidget *entry, gchar *text)
{
  GtkWidget *widget = gtk_button_new_with_label (text);
  g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (insert_button_text), entry);
  gtk_box_pack_start (GTK_BOX (hbox), widget, FALSE, FALSE,2); 

}
Annotation *get_annotation_from_user (gint page, gint x, gint y)
{
  GtkWidget *dialog;
  GtkWidget *entry;
  GtkWidget *label;
  gchar *entry_string = NULL;
  entry = gtk_entry_new ();

  dialog = gtk_dialog_new_with_buttons ("Annotation", GTK_WINDOW (gtk_window_new(GTK_WINDOW_TOPLEVEL)), (GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT), "OK", GTK_RESPONSE_ACCEPT, "Cancel", GTK_RESPONSE_REJECT, NULL);
  label = gtk_label_new ("Give annotation");
  GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  gtk_container_add (GTK_CONTAINER (content_area), label);
  GtkWidget *widget = gtk_font_button_new_with_font (fontdesc?fontdesc:"Sans 16");
  g_signal_connect (G_OBJECT (widget), "font-set", G_CALLBACK (font_chosen), NULL);
  gtk_container_add (GTK_CONTAINER (content_area), widget);
  widget = gtk_color_button_new ();
  G_GNUC_BEGIN_IGNORE_DEPRECATIONS
 gtk_color_button_set_rgba (GTK_COLOR_BUTTON(widget), &colordesc); 
 G_GNUC_END_IGNORE_DEPRECATIONS

  g_signal_connect (G_OBJECT (widget), "color-set", G_CALLBACK (color_chosen), NULL);
  gtk_container_add (GTK_CONTAINER (content_area), widget); 
  GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_container_add (GTK_CONTAINER (content_area), hbox); 
  
  create_button (hbox, entry, "O^O");
  create_button (hbox, entry, "♯");
  create_button (hbox, entry, "♭");
  create_button (hbox, entry, "♮");
  create_button (hbox, entry, "𝄪");
  create_button (hbox, entry, "𝄫");
  create_button (hbox, entry, "FIX -->");
  create_button (hbox, entry, "<---FIX");
   
    
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  gtk_container_add (GTK_CONTAINER (content_area), entry);

  gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_ACCEPT);
  gtk_widget_show_all (dialog);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
        {
          gchar *string = NULL;
          
          if (GTK_DIALOG (dialog))
            {
               entry_string = (gchar *) gtk_entry_get_text (GTK_ENTRY (entry));
               string = g_strdup (entry_string);
               gtk_widget_destroy (dialog);
               Annotation *ann = create_annotation (string, page, x, y, fontdesc?fontdesc:g_strdup ("Sans 16"), &colordesc);
               return ann;
           }
       }
  else
   {
     gtk_widget_destroy (dialog);
   }
   return NULL;
}


  
static void mark_annotation (Annotation *p)
{
   Annotation *ann = get_annotation_from_user (p->page, p->x, p->y);
   if (ann)
      {
         annotations = g_list_append (annotations, ann);
         gtk_widget_queue_draw (lh_view);
         gtk_widget_queue_draw (rh_view);
      }
}

static void mark_spot (Annotation *p)
{
   GdkRGBA color = { 0.98, 0.91, 0.3, 0.8 };
   Annotation *ann = create_annotation  ("●", p->page, p->x, p->y, "denemo medium 40", &color);
   if (ann)
      {
         annotations = g_list_append (annotations, ann);
         gtk_widget_queue_draw (lh_view);
         gtk_widget_queue_draw (rh_view);
      }
}

static void save_markings (void)
{
  if (markings_file)
      {
         FILE *fp = fopen (markings_file, "w");
         if (fp)
            {
               GList *g;
               fprintf (fp, "%d %d\n", width, height);
             
               for (g=annotations;g;g=g->next)
                  {
                   Annotation *ann = (Annotation *)g->data;
                   fprintf (fp, "%d %d %d %f %f %f %f\n%s\n%s\n", ann->page, (int)ann->x, (int)ann->y, ann->r, ann->g, ann->b, ann->a, ann->font, ann->annotation);
                  }  
               fclose (fp);
            }
         else
            g_warning ("Could not write %s for markings\n", markings_file);
      }
   else
      g_warning ("No file for markings");
}

static gint get_page_num_for_view (GtkWidget *view)
   {
    return view == lh_view?page1.pnum:page2.pnum; 
   }
   
static GList *nearby_annotation (gint page, gint x, gint y)
{
   GList *g;
   for (g = annotations; g; g=g->next)
      {
        Annotation *ann = g->data;
         if ((ann->page == page) && abs(ann->x-x+15)<SPOT_SIZE && abs(ann->y-y+15)<SPOT_SIZE)
            {
              return g;
            }
      }
   return NULL;
}


void
hot_cursor (void)
{
  static GdkCursor *circ = NULL;
  if (!circ)
    circ = gdk_cursor_new_for_display (gdk_display_get_default (), GDK_TARGET);
  gdk_window_set_cursor (gtk_widget_get_window (score_window), circ);
}

void
normal_cursor (void)
{
  static GdkCursor *arrowcursor = NULL;
  if (!arrowcursor)
    arrowcursor = gdk_cursor_new_for_display (gdk_display_get_default (), GDK_LEFT_PTR);
  gdk_window_set_cursor (gtk_widget_get_window (score_window), arrowcursor);
}


static GList *dragging_annlink = NULL;
static gboolean button_press (GtkWidget * view, GdkEventButton * event)
{
  if (event->button == 1)
   { 
      Annotation current_position;//just as a page, x, y , not a real annotation
      current_position.page = get_page_num_for_view (view);
      current_position.x = (int)event->x;
      current_position.y = (int)event->y;
      dragging_annlink = nearby_annotation (current_position.page, current_position.x, current_position.y);
    //  if (dragging_annlink)
     //    hot_cursor();
   }
   return TRUE;
  return FALSE;                 //propagate further 
}

static gboolean motion_notify (GtkWidget * view, GdkEventMotion * event)
{
   if (dragging_annlink)
      { 
         Annotation *ann = dragging_annlink->data;
         if (ann->page == get_page_num_for_view (view))
           {
               markings_unsaved = TRUE;
               ann->x = event->x, ann->y = event->y;
               gtk_widget_queue_draw (view);
               return TRUE;
            }
      }
      Annotation current_position;//just as a page, x, y , not a real annotation
      current_position.page = get_page_num_for_view (view);
      current_position.x = (int)event->x;
      current_position.y = (int)event->y;
      if (nearby_annotation (current_position.page, current_position.x, current_position.y))
         hot_cursor ();
      else
         normal_cursor ();
 
   return FALSE;                 //propagate further
}

static gboolean button_release (GtkWidget * view, GdkEventButton * event)
{
  if (dragging_annlink)
      {
      normal_cursor ();
      dragging_annlink = NULL;
      return TRUE;
      }
  if (event->button != 1)
   { 
      static Annotation current_position;//just as a page, x, y , not a real annotation
      current_position.page = get_page_num_for_view (view);
      current_position.x = (int)event->x;
      current_position.y = (int)event->y;
      GtkWidget *menu = gtk_menu_new ();
      GtkWidget *item;

      item = gtk_menu_item_new_with_label ("Choose score");
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
      g_signal_connect_swapped (G_OBJECT (item), "activate", G_CALLBACK (choose), NULL);

      item = gtk_menu_item_new_with_label ("Annotate here");
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
      g_signal_connect_swapped (G_OBJECT (item), "activate", G_CALLBACK (mark_annotation), &current_position);
      item = gtk_menu_item_new_with_label ("Spot here");
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
      g_signal_connect_swapped (G_OBJECT (item), "activate", G_CALLBACK (mark_spot), &current_position);
      
      
      
      GList *annlink = nearby_annotation (current_position.page, current_position.x, current_position.y);
      item = gtk_menu_item_new_with_label ("Delete this annotation");
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
      g_signal_connect_swapped (G_OBJECT (item), "activate", G_CALLBACK (delete_annotation), annlink);
      gtk_widget_set_sensitive (item,(annlink != NULL));
      
      item = gtk_menu_item_new_with_label ("Save annotations");
      gtk_widget_set_sensitive (item,markings_unsaved);
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
      g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (save_markings), NULL);
        
      item = gtk_menu_item_new_with_label ("Navigate");
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
      g_signal_connect_swapped (G_OBJECT (item), "activate", G_CALLBACK (navigate), NULL);
 
      item = gtk_menu_item_new_with_label ("Help");
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
      g_signal_connect_swapped (G_OBJECT (item), "activate", G_CALLBACK (show_help), NULL); 
      item = gtk_menu_item_new_with_label ("Quit");
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
      g_signal_connect_swapped (G_OBJECT (item), "activate", G_CALLBACK (gtk_main_quit), NULL);     

      gtk_widget_show_all (menu);
#if ((GTK_MAJOR_VERSION==3) && (GTK_MINOR_VERSION>=22))
      gtk_menu_popup_at_pointer (GTK_MENU (menu), NULL);
#else
// FIXME something for gtk2
#endif   
      return TRUE;
   }
 return FALSE;
}

static void draw_page_break (cairo_t *cr)
{
   double x0, y0, x1, y1;
   x0 = width-50;
   x1 = width;
   y0 = 0;
   y1 = height;
   
   cairo_pattern_t *pat = cairo_pattern_create_linear (x0, y0, x1, y0);
   cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 0.8);
   cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 1, 0.1);
   cairo_rectangle (cr, x0, y0, x1 - x0, y1 - y0);
   cairo_set_source (cr, pat);
   cairo_fill (cr);
   cairo_pattern_destroy (pat);
   cairo_fill (cr);
}

static void draw_text (cairo_t * cr, const char *font, const char *text, double x, double y, gboolean invert)
{
  //y -= size;
  //size *= 0.75;
  PangoLayout *layout;
  PangoFontDescription *desc;
  /* Create a PangoLayout, set the font and text */
  layout = pango_cairo_create_layout (cr);

  pango_layout_set_text (layout, text, -1);
  desc = pango_font_description_from_string (font);
 // pango_font_description_set_size (desc, size * PANGO_SCALE);
  pango_layout_set_font_description (layout, desc);
  pango_font_description_free (desc);
  pango_cairo_update_layout (cr, layout);


  cairo_move_to (cr, x, y);
  if (invert)
    cairo_scale (cr, 1, -1);
  pango_cairo_show_layout (cr, layout);
  /* free the layout object */
  g_object_unref (layout);
}



      
static gboolean overdraw (GtkWidget* view, cairo_t * cr)
{
   GList *g;
   if (lh_page==NULL) return FALSE;
   gint pnum = get_page_num_for_view (view);
   for (g = annotations; g; g=g->next)
      {
         Annotation *ann = (Annotation*)g->data;
         gdouble x = ann->x, y = ann->y; //g_print ("annotation %s page %d for page %d?\n", ann->annotation, ann->page, pnum);
         if (ann->page == pnum)
            {
               cairo_set_source_rgba (cr, ann->r, ann->g, ann->b, ann->a);
               draw_text (cr, ann->font, ann->annotation, x, y, FALSE);
            }
      }
   gint w=20,h=40;
    if (lh_page->pnum != (rh_page->pnum - 1))           
      {
         if (pnum == lh_page->pnum)
            {
               draw_page_break (cr);
            }   
         if (view == lh_view)
            {
                  cairo_pattern_t *pat = cairo_pattern_create_linear (20, 0, 20, 20);
                  cairo_pattern_add_color_stop_rgba (pat, 1, 1, 1, 0, 0.1);
                  cairo_pattern_add_color_stop_rgba (pat, 0, 0.2, 0.2, 0, 0.8);
                  cairo_rectangle (cr, 20, 0, width, 20);
                  cairo_set_source (cr, pat);
                  cairo_fill (cr);
                  cairo_pattern_destroy (pat);
                  
                  //cairo_set_line_width (cr, 10.0); 
                  cairo_move_to (cr, 15, 0);
                  cairo_rel_line_to (cr, w, 0);
                  cairo_rel_line_to (cr, -w/2, h);
                  cairo_rel_line_to (cr, -10, -40);
                  cairo_close_path (cr);
                  cairo_set_source_rgba (cr, 0.4, 0.4, 0, 0.8);
                  cairo_fill (cr);
            }
         else
            {
                  cairo_pattern_t *pat = cairo_pattern_create_linear (width, 0, width - 20, 0);
                  cairo_pattern_add_color_stop_rgba (pat, 1, 1, 1, 0, 0.1);
                  cairo_pattern_add_color_stop_rgba (pat, 0, 0.2, 0.2, 0, 0.8);
                  cairo_rectangle (cr, width - 20, 0, 20, height - 50);
                  cairo_set_source (cr, pat);
                  cairo_fill (cr);
                  cairo_pattern_destroy (pat);
                  pat = cairo_pattern_create_linear (20, 0, 20, 20);
                  cairo_pattern_add_color_stop_rgba (pat, 1, 1, 1, 0, 0.1);
                  cairo_pattern_add_color_stop_rgba (pat, 0, 0.2, 0.2, 0, 0.8);
                  cairo_rectangle (cr, 0, 0, width, 20);
                  cairo_set_source (cr, pat);
                  cairo_fill (cr);
                  cairo_pattern_destroy (pat);
                  cairo_move_to (cr, width, height - 50);
                  cairo_rel_line_to (cr, -w, 0);
                  cairo_rel_line_to (cr, w/2, -h);
                  cairo_rel_line_to (cr, w/2, h);
                  cairo_close_path (cr);
                  cairo_set_source_rgba (cr, 0.4, 0.4, 0, 0.8);
                  cairo_fill (cr);
            }
      }
      else //pages in order
      {
         if (view == lh_view)
            {
                 //a line up the center and triangle at bottom pointing up
                  
                  cairo_pattern_t *pat = cairo_pattern_create_linear (width, 0, width, height);
                  cairo_pattern_add_color_stop_rgba (pat, 1, 1, 1, 0, 0.1);
                  cairo_pattern_add_color_stop_rgba (pat, 0, 0.4, 0.4, 0, 0.8);
                  cairo_rectangle (cr, width - 20, 10, width, height - 50);
                  cairo_set_source (cr, pat);
                  cairo_fill (cr);
                  cairo_pattern_destroy (pat);
                  cairo_move_to (cr, width - 20, height - 40);
                  cairo_rel_line_to (cr, w, 0);
                  cairo_rel_line_to (cr, -w/2, -h);
                  cairo_rel_line_to (cr, -w/2, h);
                  cairo_close_path (cr);
                  cairo_set_source_rgba (cr, 0.4, 0.4, 0, 0.8);
                  cairo_fill (cr);
            }   
            else {
                  cairo_move_to (cr, -5, 9); //rightwards pointing triangle at top
                  cairo_rel_line_to (cr, 0, w);
                  cairo_rel_line_to (cr, h, -w/2);
                  cairo_rel_line_to (cr, -h, -w/2);
                  cairo_close_path (cr);
                  cairo_set_source_rgba (cr, 0.4, 0.4, 0, 0.8);
                  cairo_fill (cr);
            }      
      }  
  return FALSE;
}


int main(int argc, char **argv)
 {
   gtk_init(&argc, &argv);
   GdkRectangle r;
#if ((GTK_MAJOR_VERSION==3) && (GTK_MINOR_VERSION>=22))
   gdk_monitor_get_workarea (gdk_display_get_primary_monitor (gdk_display_get_default ()), &r);
#else
// FIXME something for gtk2
#endif   
   width = r.width;
   height = 0;
 
   if (argc>1 && (1 != sscanf (argv[1], "%d", (int*)&page_on)))
    goto error;      
   if (argc>2 && (1 != sscanf (argv[2], "%d", (int*)&page_back)))
    goto error;      
   if (argc>3 && (1 != sscanf (argv[3], "%d", (int*)&skip_page)))
    goto error;      

      
   if (argc>4 && (1 != sscanf (argv[4], "%lf", &aspect_ratio)))
    goto error;
   if (argc>5 && (1 != sscanf (argv[5], "%d", &width)))
    goto error;
   if (argc>6 && (1 != sscanf (argv[6], "%d", &height)))
    goto error;
   if (height == 0)
         {
            if (aspect_ratio<0.001)
               aspect_ratio = 1.414;
            height = width * aspect_ratio;
            if (height > r.height)
               height = r.height;
            width = height/aspect_ratio;   
            x = (r.width-width)/2;
         }
   g_print ("Using height %d width %d, aspect ratio %f at %d horizontally\n", height, width, aspect_ratio, x);
   music_home = g_get_home_dir();
   ev_init ();
   score_window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
   gtk_window_set_decorated (GTK_WINDOW (score_window), FALSE);
   g_signal_connect (score_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

   gtk_window_move (GTK_WINDOW (score_window), x, y);
   gtk_window_set_default_size (GTK_WINDOW (score_window), 2*width, height);
   GtkWidget *main_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
   gtk_container_add (GTK_CONTAINER(score_window), main_hbox);
   
   eventbox1 = gtk_event_box_new ();
   
   gtk_widget_add_events (eventbox1, (GDK_BUTTON_RELEASE_MASK | GDK_BUTTON_PRESS_MASK | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK ));
   gtk_box_pack_start (GTK_BOX(main_hbox), eventbox1, TRUE, TRUE, 0);
   
   GtkWidget *scroll1 = gtk_scrolled_window_new (NULL, NULL);
   gtk_container_add (GTK_CONTAINER(eventbox1), scroll1); 
   g_signal_connect (G_OBJECT(eventbox1), "key-press-event", G_CALLBACK (keypress), NULL);
   lh_view = (GtkWidget *) ev_view_new ();
   g_signal_connect (G_OBJECT(lh_view), "button-release-event", G_CALLBACK (button_release), NULL);
   g_signal_connect (G_OBJECT(lh_view), "button-press-event", G_CALLBACK (button_press), NULL);

   g_signal_connect (G_OBJECT (lh_view), "motion_notify_event", G_CALLBACK (motion_notify), NULL);

   g_signal_connect_after (G_OBJECT (lh_view), "draw", G_CALLBACK (overdraw), NULL);
   gtk_container_add (GTK_CONTAINER (scroll1), lh_view);


   eventbox2 = gtk_event_box_new ();
   gtk_widget_add_events (eventbox2, (GDK_BUTTON_RELEASE_MASK | GDK_BUTTON_PRESS_MASK | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK ));
   gtk_box_pack_start (GTK_BOX(main_hbox), eventbox2, TRUE, TRUE, 0);
   
   GtkWidget *scroll2 = gtk_scrolled_window_new (NULL, NULL);
   gtk_container_add (GTK_CONTAINER(eventbox2), scroll2); 
   g_signal_connect (G_OBJECT(eventbox2), "key-press-event", G_CALLBACK (keypress), NULL);
   rh_view = (GtkWidget *) ev_view_new ();
   g_signal_connect (G_OBJECT(rh_view), "button-release-event", G_CALLBACK (button_release), NULL);
   g_signal_connect (G_OBJECT(rh_view), "button-press-event", G_CALLBACK (button_press), NULL);
   g_signal_connect (G_OBJECT (rh_view), "motion_notify_event", G_CALLBACK (motion_notify), NULL);

   g_signal_connect_after (G_OBJECT (rh_view), "draw", G_CALLBACK (overdraw), NULL);
   gtk_container_add (GTK_CONTAINER (scroll2), rh_view); 

   gtk_widget_show_all (score_window);
   gtk_widget_hide (gtk_scrolled_window_get_vscrollbar (GTK_SCROLLED_WINDOW(scroll1)));
   gtk_widget_hide (gtk_scrolled_window_get_vscrollbar (GTK_SCROLLED_WINDOW(scroll2)));

  
   if (argc==1)
      show_help();
   g_idle_add ((GSourceFunc) choose, (gpointer) music_home);
   gtk_main();

   return 0;  
   error:
        g_print ("Usage: pageswitcher [page onward, default %d (ie '%c')] [lh page back, default  %d (ie '%c')]  [rh page forward, default  %d (ie '%c')]  [aspect ratio, default 1.414, ie A4] [width in pixels, default maximum][height in pixels - overrides monitor size and aspect ratio]\n",
               page_on, page_on, page_back, page_back, skip_page, skip_page);
         return -1;
 }
