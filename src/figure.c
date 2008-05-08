/* figure.cpp
 *
 * Functions for the manipulations of figured basses
 *
 * for Denemo, a gtk+ frontend for GNU Lilypond
 * (c) 2003-2006 Richard Shann
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chordops.h"
#include "calculatepositions.h"
#include "commandfuncs.h"
#include "contexts.h"
#include "figure.h"
#include "dialogs.h"
#include "draw.h"
#include "objops.h"
#include "staffops.h"
#include "utils.h"

struct callbackdata
{
  DenemoGUI *gui;
  gchar *string;
};




/**
 * Function to actually insert a figure to an object
 *
 */
void
insertfigure (GtkWidget * widget, gpointer data)
{
  struct callbackdata *cbdata = (struct callbackdata *) data;
  DenemoGUI *gui = cbdata->gui;
  DenemoScore *si = gui->si;
  static staff_info null_info;
  GString *current_figure;
  if (si->currentobject != NULL) {
    DenemoObject *curObj = (DenemoObject *) si->currentobject ?
      (DenemoObject *) si->currentobject->data : NULL;
    gchar *figure = (cbdata->string);
    if(strlen(figure)<1)
      figure = "_";/* in case user deleted the figure to yield <> */
    /* translate the input somewhat */
    GString *f = g_string_new("");
    gchar *c = figure;
    for(c=figure;*c;c++) {
      if(*c=='+') {
	if(c==figure || *(c-1)==' ' || *(c-1)=='*' || *(c-1)=='|' || *(c-1)=='/')
	  g_string_append(f, "_+");
	else
	  g_string_append(f,"+");
      }else
	if(*c=='-') {
	  if(c==figure || *(c-1)==' ' ||*(c-1)=='*' || *(c-1)=='|' || *(c-1)=='/')
	    g_string_append(f, "_-");
	  else
	    g_string_append(f,"-");
	}else
	  if(*c=='/')
	    g_string_append(f, "|");
          else {
	    if(*c=='*')
	      g_string_append(f, " ");
		else
		  g_string_append_c(f, *c);
	  }
    }
    
    if (curObj && curObj->type == CHORD)
      ((chord *) curObj->object)->is_figure = TRUE;
    ((chord *) curObj->object)->figure = g_string_new(f->str);//FIXME memory leak of old figure
    g_string_free(f, TRUE);
    do
      {
	if (si->currentobject->next)
	  cursorright (gui);
	else if (gui->si->currentmeasure->next)
	  measureright (gui);
	else 
	  break;
	curObj =
	  si->currentobject ? (DenemoObject *) si->currentobject->data : NULL;
      }
    while ((curObj != NULL) && (curObj->type != CHORD));
    
    
    si->has_figures = (gpointer)TRUE; //&null_info;
    gui->changecount++;
  } // if currentobject not null
  else {
    warningdialog("No current object to attach a figure to");	 
  } 
}

/**
 * Creates figured bass entry dialog
 *
 */
void
figure_insert (GtkAction * action, DenemoGUI * gui)
{
  gchar *string;
  gchar *PreValue = NULL;
  DenemoScore *si = gui->si;
  static struct callbackdata cbdata;
  DenemoObject *curObj = (DenemoObject *) si->currentobject ?
    (DenemoObject *) si->currentobject->data : NULL;
 
  if (curObj && curObj->type == CHORD && ((chord *) curObj->object)->figure)
    {
      PreValue = ((GString *) ((chord *) curObj->object)->figure)->str;
    }


  string = string_dialog_entry(gui, "Insert/Edit Figure", "Give figures followed by Enter key", PreValue);

  cbdata.gui = gui;
  cbdata.string = string;
   
  if (string)
    {
      insertfigure (NULL, &cbdata);
      ((DenemoStaff*)si->currentstaff->data)->hasfigures=TRUE;
      displayhelper (gui);
    }
  g_free(string);
}
