/* figure.c
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
static void
insertfigure (gboolean filter, gpointer data)
{
  struct callbackdata *cbdata = (struct callbackdata *) data;
  DenemoGUI *gui = cbdata->gui;
  DenemoScore *si = gui->si;
  static staff_info null_info;
  GString *current_figure;
  gchar filter_sep = filter?'/':'|';
  gchar filter_spc = filter?'*':' ';
  if(si->measurewidth==DENEMO_INITIAL_MEASURE_WIDTH)
    si->measurewidth = DENEMO_INITIAL_MEASURE_WIDTH*2;

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
	if(c==figure || *(c-1)==' ' || *(c-1)==filter_spc || *(c-1)=='|' || *(c-1)==filter_sep)
	  g_string_append(f, "_+");
	else
	  g_string_append(f,"+");
      }else
	if(*c=='-') {
	  if(c==figure || *(c-1)==' ' ||*(c-1)==filter_spc || *(c-1)=='|' || *(c-1)==filter_sep)
	    g_string_append(f, "_-");
	  else
	    g_string_append(f,"-");
	}else
	  if(*c==filter_sep)
	    g_string_append(f, "|");
          else {
	    if(*c==filter_spc)
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
	  movecursorright (NULL);
	else if (gui->si->currentmeasure->next)
	  movetomeasureright (NULL);
	else 
	  break;
	curObj =
	  si->currentobject ? (DenemoObject *) si->currentobject->data : NULL;
      }
    while ((curObj != NULL) && (curObj->type != CHORD));
    
    
    si->has_figures = (gpointer)TRUE; //&null_info;
    score_status(gui, TRUE);
  } // if currentobject not null
  else {
    warningdialog("No current object to attach a figure to");	 
  } 
}




void delete_figured_bass  (GtkAction *action, gpointer param) {
  DenemoGUI *gui = Denemo.gui;
  DenemoStaff* thestaff = (DenemoStaff*)gui->si->currentstaff->data;
  if(confirm("Figured Bass Deletion", "Delete all figured bass markings from this staff?")) {
    thestaff->hasfigures=FALSE;
    measurenode *curmeasure;
    for(curmeasure = thestaff->measures;curmeasure;curmeasure=curmeasure->next) {
      objnode *curobj;
      for(curobj = curmeasure->data;curobj;curobj=curobj->next) {    
	DenemoObject *curObj=(DenemoObject*)curobj->data;
	if (curObj && curObj->type == CHORD) {
	  GString *s= ((chord *) curObj->object)->figure;
	  if(s) g_string_free(s, TRUE);
	  ((chord *) curObj->object)->figure = NULL;
	}
      }
    }
  }
}
void hide_figured_bass  (GtkAction *action, gpointer param) {
  DenemoGUI *gui = Denemo.gui;
  DenemoStaff* thestaff = (DenemoStaff*)gui->si->currentstaff->data;
   thestaff->hasfigures=FALSE;
}
/* turn on figured bass if any figures are present */
void show_figured_bass  (GtkAction *action, gpointer param) {
  DenemoGUI *gui = Denemo.gui;
  DenemoStaff* thestaff = (DenemoStaff*)gui->si->currentstaff->data;
  measurenode *curmeasure;
  for(curmeasure = thestaff->measures;curmeasure;curmeasure=curmeasure->next) {
      objnode *curobj;
      for(curobj = curmeasure->data;curobj;curobj=curobj->next) {    
	DenemoObject *curObj=(DenemoObject*)curobj->data;
	if (curObj && curObj->type == CHORD) {
	  GString *s= ((chord *) curObj->object)->figure;
	  if(s) thestaff->hasfigures=TRUE;
	}
      }
    }
}
/**
 * Creates figured bass entry dialog
 *
 */
void
figure_insert (GtkAction *action, DenemoScriptParam * param)
{
  DenemoGUI *gui = Denemo.gui;
  gchar *string = NULL;
  gchar *PreValue = NULL;
  DenemoScore *si = gui->si;
  static struct callbackdata cbdata;
  DenemoObject *curObj = (DenemoObject *) si->currentobject ?
	(DenemoObject *) si->currentobject->data : NULL;
 
  if (curObj && curObj->type == CHORD && ((chord *) curObj->object)->figure)
	  PreValue = ((GString *) ((chord *) curObj->object)->figure)->str;

  if(!action && param && param->string) {//Called from scheme, could be "query" or setting the values
    GString *values = param->string;
    gchar *str;
    gint i;
    if(!strcmp(values->str, "query")) {
      if(PreValue && *PreValue) {
	param->status = TRUE;
	g_string_assign(param->string, PreValue);
      } else
      g_string_assign(param->string, "");
    }  else //detect the string "figures" followed by a separator character and then the figures
      for(i=0;i<values->len;i+=strlen(values->str+i)+1) {
	  if((str = g_strstr_len(values->str+i,strlen(values->str+i), "figures"))) {
		string = g_strdup(str+strlen("figures")+1);
		break;
	  }
      }
    } else {//interactive
      string = string_dialog_entry(gui, "Insert/Edit Figure", "Give figures followed by Enter key", PreValue);
    }
  cbdata.gui = gui;
  cbdata.string = string;
   
  if (string)
    {
      insertfigure (action!=NULL, &cbdata);
      //also \set Staff.useBassFigureExtenders = ##t
      ((DenemoStaff*)si->currentstaff->data)->hasfigures=TRUE;
      displayhelper (gui);
    }
  g_free(string);
}
