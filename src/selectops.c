/**
 * selectops.c
 * operations for selecting, cutting, copying, and pasting music
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 1999-2005 Matthew Hiller, Adam Tee 
 *
 * TODO: Fix Undo/Redo
 */

#include <string.h>
#include "calculatepositions.h"
#include "commandfuncs.h"
#include <denemo/denemo.h>
#include "draw.h"
#include "objops.h"
#include "measureops.h"
#include "selectops.h"
#include "staffops.h"
#include "prefops.h"
#include "lyric.h"
#include "lilydirectives.h"
#include "scoreops.h"
/*For save selection function*/
#include "utils.h"
/**
 * The copy buffer is a GList of objnode *s -- at first, I was going
 * to use staffnode *s, measurenode *s, and then objnode *s, but I
 * realized that'd be overkill and just complicate the implementation
 * unnecessarily.
 *
 * Each item in the copybuffer list corresponds to the stuff in
 * the buffer on each staff.  
 */

static void
undo (DenemoGUI * gui);
static void
redo (DenemoGUI * gui);
static GList *copybuffer = NULL; // this is a list one for each staff of lists of objects

static gint staffsinbuffer = 0;
static gint measurebreaksinbuffer = 0;

static GList *clipboards = NULL;

static GList *clone_obj_list(GList *g) {
  GList *ret=NULL;
  do {
    ret = g_list_append(ret, dnm_clone_object(g->data));
  } while((g=g->next));
  return ret;
}
void push_clipboard (void) {
  GList *thecopy = NULL;
  GList *g;
  for(g=copybuffer;g;g=g->next) {
    thecopy = g_list_append(thecopy, clone_obj_list(g->data));
  }
  clipboards = g_list_prepend(clipboards, thecopy);
}

gboolean pop_clipboard(void) {
  GList *thecopy = NULL;
  if(clipboards==NULL)
    return FALSE;
  thecopy = clipboards->data;
  clipboards = g_list_remove(clipboards, thecopy);
  clearbuffer();
  copybuffer = thecopy;
  return TRUE;
}

/* returns the top clipboard popped off the stack.
   The caller must free the clipboard with 
   when done */
GList *pop_off_clipboard(void) {
  GList *thecopy = NULL;
  thecopy = clipboards->data;
  clipboards = g_list_remove(clipboards, thecopy);
  return thecopy;
}

/**
 *  sets current object to the given cursor position
 * 
 */
void
setcurrentobject (DenemoScore * si, gint cursorpos)
{

  g_debug ("Set Current Object Cursor pos %d\n", cursorpos);

  si->currentobject = g_list_nth ((objnode *) si->currentmeasure->data,
				  cursorpos);
  //g_assert (si->currentobject != NULL);
}

/**
 *  clearbuffer
 *  Clears the copybuffer of data
 *  Arguments - None
 *  return - none
 */
void
clearbuffer (void)
{
  g_list_foreach (copybuffer, freeobjlist, NULL);
  g_list_free (copybuffer);
  copybuffer = NULL;
  staffsinbuffer = 0;
  measurebreaksinbuffer = 0;
}

void
free_clipboard(GList *clipboard) {
  if(clipboard) {
    push_clipboard();
    copybuffer = clipboard;
    clearbuffer();
    pop_clipboard();
  }
}

gint get_staffs_in_clipboard(void) {
  return staffsinbuffer;
}
void
insert_clipboard(GList *clipboard) {
  if(clipboard) {
    push_clipboard();
    copybuffer = clipboard;
    call_out_to_guile("(d-Paste)");
    copybuffer = NULL;
    pop_clipboard();
  }
}
/**
 *  saveselection 
 *  Saves the current selection to a given file
 * 
 *  @param si pointer to the score information structure
 *  @return none
 */
void
saveselection (DenemoScore * si)
{
  if (si->markstaffnum == 0)	/* Indicator that there's no selection.  */
    return;

  clearbuffer ();

  staffsinbuffer = si->laststaffmarked - si->firststaffmarked + 1;

  copytobuffer (si);
  si->savebuffer = copybuffer;
  /* Test code for save selection
     FILE *fp;
     GString *file = NULL; 
     file = g_string_new(locatedotdenemo());
     g_string_append(file, "/denemoanalysispattern");

     filesaveselection(file->str, si);
     clearbuffer ();
     g_free(file);
   */
}

/**
 *   copytobuffer
 *   Copies selection to the copybuffer
 *   
 *   @param si pointer to the score information structure
 */
void
copytobuffer (DenemoScore * si)
{
  staffnode *curstaff;
  measurenode *curmeasure;
  objnode *curobj;
  objnode *theobjs;
  DenemoObject *clonedobject;
  gint i, j, k;

  if (si->markstaffnum == 0)	/* Indicator that there's no selection.  */
    return;

  clearbuffer ();

  staffsinbuffer = si->laststaffmarked - si->firststaffmarked + 1;
  g_debug ("No staffs in copybuffer %d\n", staffsinbuffer);
  /* Staff loop.  */
  for (i = si->firststaffmarked, curstaff = g_list_nth (si->thescore, i - 1);
       curstaff && i <= si->laststaffmarked; curstaff = curstaff->next, i++)
    {
      if (((DenemoStaff *) curstaff->data)->is_parasite)
	continue;
      /* Initialize first ->data for copybuffer to NULL.  */
      theobjs = NULL;
      /* Measure loop.  */
      for (j = si->firstmeasuremarked, k = si->firstobjmarked,
	   curmeasure = g_list_nth (firstmeasurenode (curstaff), j - 1);
	   curmeasure && j <= si->lastmeasuremarked;
	   curmeasure = curmeasure->next, j++)
	{
	  for (curobj = g_list_nth ((objnode *) curmeasure->data, k);
	       /* cursor_x is 0-indexed */
	       curobj && (j < si->lastmeasuremarked
			  || k <= si->lastobjmarked);
	       curobj = curobj->next, k++)
	    {
	      clonedobject = dnm_clone_object ((DenemoObject *) curobj->data);
	      theobjs = g_list_append (theobjs, clonedobject);
	    }			/* End object loop */
	  g_debug ("cloned objects on staff \n");
	  
	  if (j < si->lastmeasuremarked || k < si->lastobjmarked)
	    {
	      if(!((j==si->lastmeasuremarked))) {
		g_debug ("Insert measurebreak obj in copybuffer");
		/* ???outdated comment??? That is, there's another measure, the cursor is in appending
		   position, or the selection spans multiple staffs, in which 
		   case another measure boundary should be added.  */
		theobjs = g_list_append (theobjs, newmeasurebreakobject ());
		if (i == si->firststaffmarked)
		  measurebreaksinbuffer++;
	      }
	    }
	  k = 0;		/* Set it for next run through object loop */
	  
	}			/* End measure loop */
      if ((staffsinbuffer > 1) && (i < si->laststaffmarked))
	{
	  theobjs = g_list_append (theobjs, newstaffbreakobject ());
	  g_debug ("Inserting Staffbreak object in copybuffer");
	}
      if(theobjs)
	copybuffer = g_list_append (copybuffer, theobjs);
    }				/* End staff loop */
}


/**
 *  cuttobuffer
 *  Cuts selection to the copybuffer, removing it from the score
 *
 *  @param si pointer to score information structure
 */
static void
cuttobuffer (DenemoScore * si, gboolean copyfirst)
{
  staffnode *curstaff;
  measurenode *curmeasure;
  objnode *tempobj;
  gint i, jcounter, //jcounter is marking the position of the measure currently being cleared I think
    max;
  if (!si->markstaffnum)
    return;
  take_snapshot();
  if(copyfirst)
    copytobuffer (si);
  gint staffs_removed_measures = 0;// a count of removed measures in the case where multiple staffs are involved
  gint lmeasurebreaksinbuffer = si->lastmeasuremarked - si->firstmeasuremarked;
  gint lstaffsinbuffer = si->laststaffmarked - si->firststaffmarked + 1;
  if(copyfirst) {
    if(!( lmeasurebreaksinbuffer == measurebreaksinbuffer))
      g_warning("logic of copy to buffer seems wrong about measure breaks");
    if(!(lstaffsinbuffer == staffsinbuffer))
      g_warning("logic of copy to buffer seems wrong about staff breaks");
  }
  if (lstaffsinbuffer == 1)
    {
      /* Just a single staff is a special case, again.  */
      jcounter = si->firstmeasuremarked; //currently clearing stuff from the firstmeasuremarked
      curmeasure = g_list_nth (firstmeasurenode (si->currentstaff), jcounter - 1);

      /* Clear the relevant part of the first measure selected */
      if (lmeasurebreaksinbuffer)
	max = G_MAXINT;
      else
	max = si->lastobjmarked;
      for (i = si->firstobjmarked;
	   ((tempobj = g_list_nth ((objnode *) curmeasure->data,
				   si->firstobjmarked)) && i <= max); i++)
	{
	  curmeasure->data =
	    g_list_remove_link ((objnode *) curmeasure->data, tempobj);
	  freeobject ((DenemoObject *) tempobj->data);
	  g_list_free_1 (tempobj);
	}
      jcounter++; //move on to the second measure being cleared
      curmeasure = curmeasure->next;

      if (!si->thescore->next)
	{
	  /* That is, the score has only this one staff
	   remove the (whole) measures between the first and last - which may be partial.*/
	  if (lmeasurebreaksinbuffer - 1 > 0)
	    {
	      curmeasure =
		removemeasures (si, jcounter - 1, lmeasurebreaksinbuffer - 1, TRUE);
	      jcounter += lmeasurebreaksinbuffer - 1;// increased by the number of measures *between* first and last marked
	    }
	}
      else
	for (; curmeasure && jcounter < si->lastmeasuremarked;
	     curmeasure = curmeasure->next, jcounter++)
	  {
	    freeobjlist (curmeasure->data, NULL);
	    curmeasure->data = NULL;
	  }
      /* Now clear the relevant part of the last measure selected */
      if (curmeasure && (jcounter <= si->lastmeasuremarked))
	{
	  for (i = 0; curmeasure->data && i <= si->lastobjmarked; i++)
	    {
	      tempobj = (objnode *) curmeasure->data;
	      curmeasure->data =
		g_list_remove_link ((objnode *) curmeasure->data, tempobj);
	      freeobject ((DenemoObject *) tempobj->data);
	      g_list_free_1 (tempobj);
	    }
	  /* And delete it, if the measure's been cleared and there's only
	     one staff.  */
#if 0
	  if (!curmeasure->data && !si->thescore->next)
	    removemeasures (si, jcounter - 1, 1, TRUE);//WRONG the other measures have been removed, so jcounter no longer indexes anything in the staff!
#else
	  if (!curmeasure->data && !si->thescore->next)
	    removemeasures (si, g_list_position(firstmeasurenode(si->currentstaff), curmeasure), 1, TRUE);
#endif



	}
      showwhichaccidentalswholestaff ((DenemoStaff *) si->currentstaff->data);
      beamsandstemdirswholestaff ((DenemoStaff *) si->currentstaff->data);
    } // end of single staff
  else
    {				/* Multiple staff selection */
      if (lstaffsinbuffer == (gint) (g_list_length (si->thescore)))
	{
	  /* Every staff was part of the selection */
	  if (lmeasurebreaksinbuffer > 0)
	    {
	      
	      removemeasures (si, si->firstmeasuremarked - 1,
			    lmeasurebreaksinbuffer+1, TRUE);
	      staffs_removed_measures = lmeasurebreaksinbuffer;
	    }
	  else
	    for (curstaff = si->thescore; curstaff; curstaff = curstaff->next)
	      {
		curmeasure = g_list_nth (firstmeasurenode (curstaff),  si->firstmeasuremarked-1);
		freeobjlist (curmeasure->data, NULL);
		curmeasure->data = NULL;
		
		showwhichaccidentalswholestaff ((DenemoStaff *) curstaff->data);
		beamsandstemdirswholestaff ((DenemoStaff *) curstaff->data);
	      }
	}
      else
	{
	  /* Staff loop */
	  for (i = si->firststaffmarked,
	       curstaff = g_list_nth (si->thescore, i - 1);
	       curstaff && i <= si->laststaffmarked;
	       curstaff = curstaff->next, i++)
	    {
	      if (((DenemoStaff *) curstaff->data)->is_parasite)
		continue;
	      /* Measure loop */
	      for (jcounter = si->firstmeasuremarked,
		   curmeasure = g_list_nth (firstmeasurenode (curstaff),
					    jcounter - 1);
		   curmeasure && jcounter <= si->lastmeasuremarked;
		   curmeasure = curmeasure->next, jcounter++)
		{
		  freeobjlist (curmeasure->data, NULL);
		  curmeasure->data = NULL;
		}
	      showwhichaccidentalswholestaff ((DenemoStaff *) curstaff->data);
	      beamsandstemdirswholestaff ((DenemoStaff *) curstaff->data);
	    }
	}
    }
  si->firststaffmarked = si->markstaffnum = 0;//only the latter is needed, but there was some confusion at one time...
  /* And set some currents. This would probably be better to split off
   * into a more-generalized version of setcurrents or something;
   * what's here is more-or-less copied from dnm_deleteobject in
   * commandfuncs */

  si->currentmeasurenum = si->firstmeasuremarked - (staffs_removed_measures?1:0);

  if(si->currentmeasurenum<1) {
    si->currentmeasurenum = 1;
  }


  si->currentmeasure = g_list_nth (firstmeasurenode (si->currentstaff),
				   si->currentmeasurenum - 1);
#if 0
  if(si->currentmeasure==NULL)
    si->currentmeasure = g_list_last (firstmeasurenode (si->currentstaff));
#endif



  si->cursor_x = si->firstobjmarked;
  if (si->cursor_x <
      (gint) (g_list_length ((objnode *) si->currentmeasure->data)))
    {
      si->currentobject = g_list_nth ((objnode *) si->currentmeasure->data,
				      si->cursor_x);
      si->cursor_appending = FALSE;
    }
  else
    {
      si->currentobject = g_list_last ((objnode *) si->currentmeasure->data);
      si->cursor_appending = TRUE;
    }
  //if clef has been deleted we need to re-validate leftmost clef - would only apply if the clef being deleted was off the left side of screen - some sort of scripting scenario...
  find_leftmost_allcontexts(si);

  
  isoffleftside(Denemo.gui);
  isoffrightside(Denemo.gui);

  score_status(Denemo.gui, TRUE);

  /*   isoffleftside;  */
  /*find_xes_in_all_measures (si);
    nudgerightward (Denemo.gui);
    gtk_widget_draw (si->scorearea, NULL);  */
}



DenemoObjType get_clip_obj_type(gint m, gint n) {
  if(copybuffer==NULL)
    return -1;
  GList *stafflist = g_list_nth(copybuffer, m);
  if(stafflist==NULL)
    return -1;
  GList *curbufferobj = g_list_nth(stafflist->data, n);
  if(curbufferobj==NULL || curbufferobj->data==NULL )
    return -1;
  return ((DenemoObject*)(curbufferobj->data))->type;
}

gint get_clip_objs(gint m) {
  if(copybuffer==NULL)
    return -1;
  GList *stafflist = g_list_nth(copybuffer, m);
  if(stafflist==NULL)
    return -1;
  return g_list_length(stafflist->data);
}

//FIXME yet another insert object, compare object_insert() in commandfuncs.c

void insert_object(DenemoObject *clonedobj) {
  DenemoScore *si = Denemo.gui->si;
  staffnode *curstaff = si->currentstaff;
  clonedobj->starttick = (si->currentobject?
			  ((DenemoObject *)si->currentobject->data)->starttickofnextnote: 0);
  /* update undo information */
  DenemoUndoData *undo;
  if (!si->undo_guard)
    {
      undo = (DenemoUndoData *) g_malloc (sizeof (DenemoUndoData));
      //undo->object = clonedobj;
      //do position after inserting, so we can go back to it to delete
    }

  Denemo.gui->si->currentmeasure->data =
    g_list_insert ((objnode *)si->currentmeasure->data,
		   clonedobj, si->cursor_x);


 if (!si->undo_guard)
    {
      get_position(si, &undo->position);
      undo->position.appending = 0;
      undo->action = ACTION_INSERT;
      update_undo_info (si, undo);
    }



  si->cursor_x++;
  if (si->cursor_appending)
    si->currentobject = g_list_last ((objnode *) si->currentmeasure->data);
  else
    si->currentobject = g_list_nth ((objnode *) si->currentmeasure->data,
				    si->cursor_x);

  if(si->currentobject==NULL) {
    g_warning("problematic parameters on insert %d out of %d objects", si->cursor_x+1, g_list_length((objnode *) si->currentmeasure->data));
    si->cursor_x--;
    si->currentobject = g_list_nth ((objnode *) si->currentmeasure->data,
				    si->cursor_x);
  }


  beamsandstemdirswholestaff ((DenemoStaff *) curstaff->data);
  find_xes_in_all_measures (si);
}

// insert the nth object from the copybuffer into music at the cursor position
// return TRUE if inserted
gboolean insert_clip_obj(gint m, gint n) {
  DenemoScore *si = Denemo.gui->si;
  staffnode *curstaff = si->currentstaff;
  if(copybuffer==NULL)
    return FALSE;
  GList *stafflist = g_list_nth(copybuffer, m);
  if(stafflist==NULL)
    return FALSE;
  objnode *curbufferobj = g_list_nth(stafflist->data, n);
  if(curbufferobj==NULL)
    return FALSE;
  DenemoObject *clonedobj; 
  DenemoObject *curobj = (DenemoObject*)curbufferobj->data; 
  clonedobj = dnm_clone_object (curobj);
  insert_object(clonedobj);
#if 0
  octave_up_key(Denemo.gui);//FIXME up and down to fix clef change bug !!!!!!!!
  octave_down_key(Denemo.gui);//FIXME up and down to fix clef change bug !!!!!!!!
#endif
  //reset_cursor_stats (si);
  fixnoteheights ((DenemoStaff *) si->currentstaff->data);
  beamsandstemdirswholestaff ((DenemoStaff *) si->currentstaff->data);
  find_xes_in_all_measures (si);
  showwhichaccidentals ((objnode *) si->currentmeasure->data,
			    si->curmeasurekey, si->curmeasureaccs);

  return TRUE;
}
/**
 * pastefrombuffer
 * Pastes the current buffer to the score
 *

 *
 * The updates that are done towards the bottom of this function -
 * beamsandstemdirswholestaff, find_xes_in_all_measures, etc. - are
 * too much gruntwork and are inefficient in terms of everything
 * but additional lines-of-code required for implementation. 
 * return FALSE if no pasting was done.
 */

static gboolean
pastefrombuffer (void)
{
  DenemoGUI *gui = Denemo.gui;
  DenemoScore *si = gui->si;
  staffnode *curstaff;
  measurenode *curmeasure;
  GList *curbuffernode = copybuffer;
  objnode *curbufferobj;
  gint initialinsertat;
  DenemoObject *clonedobject;
  gint staffs_used = 0;//number of staffs we have pasted into after the first
  gint i, j;
  gint measuretoaddat = si->currentmeasurenum;
  if((staffsinbuffer>1) || (measurebreaksinbuffer>0)) {
    /* g_print("si->appending=%d si->cursoroffend=%d curobjnot1st=%d\n", si->cursor_appending, si->cursoroffend, 
       (si->currentobject!=si->currentmeasure->data));*/

    if((!si->cursor_appending) && (si->currentobject!=si->currentmeasure->data))
      return FALSE;
    if(si->currentobject) {
      //g_print("Adding %d measures at %d\n", measurebreaksinbuffer+(staffsinbuffer==1), si->currentmeasurenum);
      if(si->cursor_appending) {
	addmeasures (si, si->currentmeasurenum, measurebreaksinbuffer+(staffsinbuffer==1), (staffsinbuffer>1));
	measureright(NULL);measuretoaddat++;//Better check measureright worked
      } else {
	addmeasures (si, si->currentmeasurenum-1, measurebreaksinbuffer+(staffsinbuffer==1), (staffsinbuffer>1));
      }
      setcurrents (gui->si);
    }
    //currentobject is NULL, currentmeasure is first of added measures
  }

  /* All right. Any necessary measures have been inserted - now paste away */

  if (staffsinbuffer == 1)
    initialinsertat = si->cursor_x;
  else
    initialinsertat = 0;

  g_debug ("Insert At position %d\n", initialinsertat);

  for (curstaff = si->currentstaff; curstaff && curbuffernode;
       curstaff? curstaff = si->currentstaff:0, curbuffernode = curbuffernode->next)
    {

      /*g_debug ("Current staff %x, Current Staff Next %x,"
	       " CurBuf %x, CurBuf Next %x\n",
	       curstaff, curstaff->next, curbuffernode, curbuffernode->next);
      */
      //gint prevailing_clef = find_prevailing_clef(si);

      curmeasure = g_list_nth (firstmeasurenode (curstaff),
			       si->currentmeasurenum - 1);

      
      for (curbufferobj = (objnode *) curbuffernode->data;
	   curbufferobj && curmeasure; curbufferobj = curbufferobj->next)
	{
	  DenemoObject *curobj = (DenemoObject *) curbufferobj->data;
	  if (curobj->type == STAFFBREAK)
	    {

	     break;
	    }
	  else if (curobj->type ==
		   MEASUREBREAK)
	    {
	      /*Do nothing as we will not insert a new barline at 
		this point. It is done automatically */
	      g_debug("Have measurebreak object\n");
	    
	    }
	  else
	    {
	      g_debug("Paste: Cursor Position %d\n", si->cursor_x);
	      clonedobject =
		dnm_clone_object (curobj);
	      
	      clonedobject->starttick = (si->currentobject?
					 ((DenemoObject *)si->currentobject->data)->starttickofnextnote: 0);//guess
	      
	      g_debug ("offend %d start of next note %d\n", 
		       si->cursoroffend, clonedobject->starttick);
	      insertion_point (si);
	      object_insert(gui, clonedobject);

	      si->cursoroffend = (si->currentobject?
		((DenemoObject *)si->currentobject->data)->starttickofnextnote
				  >= (WHOLE_NUMTICKS * si->cursortime1 / si->cursortime2): 0);   // guess  


	    } //not a staff break
	}			/* End bufferobj loop */
      fixnoteheights(curstaff->data);
      showwhichaccidentalswholestaff ((DenemoStaff *) curstaff->data);
      beamsandstemdirswholestaff ((DenemoStaff *) curstaff->data);
      si->currentmeasurenum =  measuretoaddat;
      curstaff = si->currentstaff->next;
      if(staffsinbuffer>1) {
	DenemoScriptParam param; 
	if(!staffdown(&param))
	  break;//FIXME wrap this function up to be called from C
	staffs_used++;
      }
      setcurrents (gui->si);
    } /* End staff loop */
  // g_print("check %d against %d\n", staffsinbuffer, staffs_used);
  while(staffs_used--)
    {
      DenemoScriptParam param; 
      staffup(&param);
    }
  si->currentmeasure = g_list_nth (firstmeasurenode (si->currentstaff),
				   si->currentmeasurenum - 1);
  si->currentobject = g_list_nth ((objnode *) si->currentmeasure->data,
				  initialinsertat);
  if (!si->currentobject)
    /* Yah. There wasn't anything in the buffer after all. */
    si->cursor_appending = TRUE;
  else
    si->cursor_appending = FALSE;
  find_xes_in_all_measures (si);

  g_debug ("End of Paste Cursor X: %d\n", si->cursor_x);
  return TRUE;
}


DenemoObject *get_mark_object(void){
  DenemoGUI * gui = Denemo.gui;
  DenemoScore *si = gui->si;
  if(!si->markstaffnum)
    return NULL;
  staffnode *curstaff = g_list_nth (si->thescore, si->firststaffmarked - 1);
  DenemoStaff *firststaff =  (DenemoStaff *) curstaff->data;
  measurenode *firstmeasure = g_list_nth (firststaff->measures,  si->firstmeasuremarked - 1);
  objnode *firstobj = g_list_nth (firstmeasure->data, si->firstobjmarked);
  //g_print("First %d\n",  si->firstobjmarked);
  return firstobj? ((DenemoObject *)firstobj->data):NULL;
}
DenemoObject *get_point_object(void){
  DenemoGUI * gui = Denemo.gui;
  DenemoScore *si = gui->si;
  if(!si->markstaffnum)
    return NULL;
  staffnode *curstaff = g_list_nth (si->thescore, si->laststaffmarked - 1);
  DenemoStaff *laststaff =  (DenemoStaff *) curstaff->data;
  measurenode *lastmeasure = g_list_nth (laststaff->measures,  si->lastmeasuremarked - 1);
  objnode *lastobj = g_list_nth (lastmeasure->data, si->lastobjmarked);
  return lastobj? ((DenemoObject *)lastobj->data):NULL;
}

/**
 * mark_boundaries_helper
 * Helper function which marks the boundaries of the 
 * mark
 *
 * Inputs 
 * @param si pointer to the DenemoScore structure
 * @param mark_staff  
 * @param mark_measure -
 * @param mark_object -
 * @param point_staff -
 * @param point_measure -
 * @param point_object -
 * @param type -
 */
static void
mark_boundaries_helper (DenemoScore * si, gint mark_staff,
			gint mark_measure, gint mark_object, gint point_staff,
			gint point_measure, gint point_object,
			enum drag_selection_type type)
{
  if (mark_staff)
    {
      si->firststaffmarked = MIN (mark_staff, point_staff);
      si->laststaffmarked = MAX (mark_staff, point_staff);

      switch (type)
	{
	case NO_DRAG:
	  /* error, really.  */
	  break;
	case NORMAL_SELECT:
	case WHOLE_MEASURES:
	  /* I was thinking of handling these with a fallthrough, but
	     the commonality in setting si->firstmeasuremarked and
	     si->lastmeasuremarked caused it not to work out cleanly.  */
	  si->firstmeasuremarked = MIN (mark_measure, point_measure);
	  si->lastmeasuremarked = MAX (mark_measure, point_measure);
	  if (type == NORMAL_SELECT
	        && si->firststaffmarked == si->laststaffmarked )
	    {
	      if (mark_measure < point_measure)
		{
		  si->firstobjmarked = mark_object;
		  si->lastobjmarked = point_object;
		}
	      else if (mark_measure > point_measure)
		{
		  si->firstobjmarked = point_object;
		  si->lastobjmarked = mark_object;
		}
	      else
		{		/* Same measure */
		  si->firstobjmarked = MIN (mark_object, point_object);
		  si->lastobjmarked = MAX (mark_object, point_object);
		}
	    }
	  else
	    {
	      si->firstobjmarked = 0;
	      si->lastobjmarked = G_MAXINT;
	    }
	  break;
	case WHOLE_STAFFS:
	  si->firstmeasuremarked = 1;
	  si->lastmeasuremarked = g_list_length (si->measurewidths);
	  si->firstobjmarked = 0;
	  si->lastobjmarked = G_MAXINT;
	}
    }
}


/**
 *  setmark
 *  Sets the current mark for the start of the buffer
 *
 *  @param gui pointer to the DenemoGUI structure
 */
void
set_mark (DenemoGUI * gui)
{
  DenemoScore *si = gui->si;
  si->markstaffnum = si->currentstaffnum;
  si->markmeasurenum = si->currentmeasurenum;
  si->markcursor_x = si->cursor_x;
  calcmarkboundaries (si);
}

/**
 *  set_point
 *  Sets the current cursor position as the end of the selection
 *
 *  @param gui pointer to the DenemoGUI structure
 */
void
set_point (DenemoGUI * gui)
{
  DenemoScore *si = gui->si;
  if(si->markstaffnum) {
    mark_boundaries_helper (si, si->markstaffnum, si->markmeasurenum,
			  si->markcursor_x, si->currentstaffnum,
			  si->currentmeasurenum, si->cursor_x, NORMAL_SELECT);
 
  }
}

gboolean
mark_status (void) {
  return Denemo.gui->si->markstaffnum!=0;
}

/**
 * unset_mark
 * Remove the current mark
 *
 * @param gui pointer to the DenemoGUI structure
 */
void
unset_mark (DenemoGUI * gui)
{
  DenemoScore *si = gui->si;
  si->markstaffnum = 0;
  calcmarkboundaries (si);
}


gboolean in_selection(DenemoScore *si) {
  if(si->markstaffnum) {
    if(si->currentstaffnum >= si->firststaffmarked &&
       si->currentstaffnum <= si->laststaffmarked) {
      if(si->currentmeasurenum == si->firstmeasuremarked) {
	if (si->currentmeasurenum == si->lastmeasuremarked) {
	  if((si->cursor_x >= si->firstobjmarked) &&
	     (si->cursor_x <= si->lastobjmarked))
	    return TRUE;
	  else return FALSE;
	}
	if (si->currentmeasurenum < si->lastmeasuremarked){
	  if(si->cursor_x >= si->firstobjmarked)
	    return TRUE;
	  return FALSE;
	}
      }
      if(si->currentmeasurenum > si->firstmeasuremarked) {
	if (si->currentmeasurenum == si->lastmeasuremarked) {
	  if((si->cursor_x <= si->lastobjmarked))
	    return TRUE;
	  else return FALSE;
	}
	if (si->currentmeasurenum < si->lastmeasuremarked)
	  return TRUE;
      }
    }
  }
  return FALSE;  
}

/* save/restore selection */
static      gint firststaff;
static      gint laststaff;
static      gint firstobj;
static      gint lastobj;
static      gint firstmeasure;
static      gint lastmeasure;

void save_selection(DenemoScore *si) {
  firststaff = si->firststaffmarked;
  laststaff = si->laststaffmarked;
  firstobj = si->firstobjmarked;
  lastobj = si->lastobjmarked;
  firstmeasure =si->firstmeasuremarked;
  lastmeasure =si->lastmeasuremarked;
}
void restore_selection(DenemoScore *si) {
  si->firststaffmarked = firststaff;
  si->laststaffmarked = laststaff;
  si->firstobjmarked  = firstobj;
  si->lastobjmarked = lastobj;
  si->firstmeasuremarked = firstmeasure;
  si->lastmeasuremarked = lastmeasure;
}



/**
 * goto_mark
 * goto the current mark without changing the selection
 *
 * 
 */
void
goto_mark (GtkAction *action, DenemoScriptParam *param)
{
  DenemoScriptParam local_param;
  local_param.status = TRUE;
  DenemoScore *si = Denemo.gui->si;
  if(!action)
    ((DenemoScriptParam *)param)->status = si->markstaffnum;
  else
    param = &local_param;
  if(si->markstaffnum){
    save_selection(si);
    set_currentmeasurenum (Denemo.gui, si->markmeasurenum);
    set_currentstaffnum (Denemo.gui,si->markstaffnum);
    while(si->cursor_x < si->markcursor_x && param->status)
      cursorright(param);
    restore_selection(si);
    if(!action)
      displayhelper(Denemo.gui);
  } 
}

/**
 * goto_selection_start
 * move cursor the first object in the selection without changing the selection
 *
 * 
 */
void
goto_selection_start (GtkAction *action, DenemoScriptParam *param)
{
  DenemoScore *si = Denemo.gui->si;
  if(!action)
    ((DenemoScriptParam *)param)->status = si->markstaffnum;
  if(si->markstaffnum){
    gint first = si->firstobjmarked;
    save_selection(si);
    set_currentmeasurenum (Denemo.gui, si->firstmeasuremarked);
    set_currentstaffnum (Denemo.gui,si->firststaffmarked);
    while(si->cursor_x < first)
      cursorright(param);
    restore_selection(si);
    if(!action)
      displayhelper(Denemo.gui);
  } 
}




static GSList *positions=NULL;
DenemoPosition *pop_position(void) {
  DenemoPosition *pos;
  if(positions) {
    pos = positions->data;
    positions = g_slist_delete_link(positions, positions);
    return pos;
  }
  return NULL;
}

void get_position(DenemoScore *si, DenemoPosition *pos) {
  pos->movement =  g_list_index(Denemo.gui->movements, si)+1;
  pos->staff =  si->currentstaffnum;
  pos->measure = si->currentmeasurenum;
  pos->object =  si->currentobject?si->cursor_x+1:0;
  pos->appending = si->cursor_appending;
  pos->offend = si->cursoroffend;
}
void push_position(void) {
  DenemoScore *si = Denemo.gui->si;
  DenemoPosition *pos = ( DenemoPosition *)g_malloc(sizeof(DenemoPosition));
  get_position(si, pos);
  if(pos->movement)
     positions = g_slist_prepend(positions, pos);
  else
    g_free(pos);
  //g_print("%d %d %d %d \n", pos->movement, pos->staff, pos->measure, pos->object);
}

static void push_given_position(DenemoPosition *pos) {
  DenemoPosition *position = (DenemoPosition*)g_malloc(sizeof(DenemoPosition));
  memcpy(position, pos, sizeof(DenemoPosition));
  positions = g_slist_prepend(positions, position);
}
/**
 *  copywrapper
 *  Wrapper function for the copy command
 *  
 * @param action pointer to the GTKAction event
 * @param gui pointer to the DenemoGUI structure
 */
void
copywrapper (GtkAction *action, DenemoScriptParam *param)
{
  DenemoGUI *gui = Denemo.gui;
  copytobuffer (gui->si);
}

/**
 * cutwrapper
 * Wrapper function for the cut command
 *
 * @param action pointer to the GTKAction event
 * @param gui pointer to the DenemoGUI structure 
 */
void
cutwrapper (GtkAction *action, DenemoScriptParam *param)
{
  DenemoGUI *gui = Denemo.gui;
  cuttobuffer (gui->si, TRUE);
  //check that measurewidths is long enough after cutting empty measures
  displayhelper (gui);
}

void
delete_selection(void) {
  DenemoGUI *gui = Denemo.gui;
  cuttobuffer (gui->si, FALSE);
  displayhelper (gui);
}
/**
 * pastewrapper
 * Wrapper function for the paste command
 *
 * @param gui pointer to the DenemoGUI structure
 * @param action pointer to the GtkAction event
 */
void
pastewrapper (GtkAction *action, DenemoScriptParam *param)
{  
  gboolean success;
  stage_undo(Denemo.gui->si, ACTION_STAGE_END);//undo is a queue (ie stack) so we push the end first
  success = !call_out_to_guile("(DenemoPaste)");
  //FIXME if not success a ACTION_SCRIPT_ERROR will have been put in the undo queue...
  stage_undo(Denemo.gui->si, ACTION_STAGE_START);

  score_status(Denemo.gui, TRUE);
}


/**
 * saveselwrapper
 * Wrapper function for the Save selection command
 *
 * @param action pointer to the GtkAction event 
 * @param gui pointer to the DenemoGUI structure
 */
void
saveselwrapper (GtkAction *action, DenemoScriptParam *param)
{
  DenemoGUI *gui = Denemo.gui;
  saveselection (gui->si);
}


/**
 * calcmarkboundaries
 * Wrapper function for the mark_boundaries_helper function
 * drag selection type is set to NORMAL_SELECT
 *
 * Inputs 
 * scoreinfo - score information
 */
void
calcmarkboundaries (DenemoScore * si)
{
  mark_boundaries_helper (si, si->markstaffnum, si->markmeasurenum,
			  si->markcursor_x, si->currentstaffnum,
			  si->currentmeasurenum, si->cursor_x, NORMAL_SELECT);
}

void
swap_point_and_mark(GtkAction *action, gpointer param) {
  DenemoScore * si = Denemo.gui->si;
  gint temp =  si->currentstaffnum;
  si->currentstaffnum = si->markstaffnum;
  si->markstaffnum = temp;

  temp =  si->currentmeasurenum;
  si->currentmeasurenum = si->markmeasurenum;
  si->markmeasurenum = temp;

  temp =  si->cursor_x;
  si->cursor_x = si->markcursor_x;
  si->markcursor_x = temp;
  setcurrentobject (si, si->cursor_x);
  calcmarkboundaries (si);
  displayhelper(Denemo.gui);
}
/**
 * undowrapper
 * Wrapper function for the undo command
 *
 * Inputs 
 * data - pointer to the score 
 * callback_action - unused
 * widget - unused
 */
void
undowrapper (GtkAction *action, gpointer param)
{
  DenemoGUI *gui = Denemo.gui;
  undo (gui);
  displayhelper (gui);
}

/**
 * redowrapper
 * Wrapper function for the redo command
 *
 * Inputs 
 * data - pointer to the score 
 * callback_action - unused
 * widget - unused
 */
void
redowrapper (GtkAction *action, gpointer param)
{
  DenemoGUI *gui = Denemo.gui;
  redo (gui);
  displayhelper (gui);
}

/* store the passed object as ACTION_CHANGE undo information */
/* potentially we could optimize the storage of undo information by telescoping changes to the same object when the undo is staged, it would mean keeping a global note of whether the undo is currently staged. We would peek at the head of the queue and if it was an ACTION_CHANGE at the same position we could free the stored object and replace it with the clone created here */
void store_for_undo_change (DenemoScore *si, DenemoObject *curobj) {
  if (!si->undo_guard)
    {
      DenemoUndoData *data = (DenemoUndoData *) g_malloc (sizeof (DenemoUndoData));
      data->object = dnm_clone_object (curobj);
      get_position(si, &data->position);
      data->action = ACTION_CHANGE;
      update_undo_info (si, data);
    }
}
void store_for_undo_measure_insert(DenemoScore *si, gint staffnum, gint measurenum) {
  if (!si->undo_guard)
    {
      DenemoUndoData *data = (DenemoUndoData *) g_malloc (sizeof (DenemoUndoData));     
      data->position.staff = staffnum;
      data->position.measure = measurenum+1;
      data->action = ACTION_MEASURE_CREATE;
      update_undo_info (si, data);
    }
}

static void 
free_chunk(DenemoUndoData *chunk) {
g_print("free %d\n", chunk->action);
  switch (chunk->action) {
  case ACTION_STAGE_START:
  case ACTION_STAGE_END:
  case ACTION_SCRIPT_ERROR:
    return;//statically allocated
   
  case ACTION_INSERT:
  case ACTION_DELETE:
  case ACTION_CHANGE:
    freeobject(chunk->object);
    g_free(chunk);
    break;
    
  case ACTION_MEASURE_CREATE:
  case ACTION_MEASURE_REMOVE:
    g_free(chunk);
    break;
  case ACTION_SNAPSHOT:
    g_warning("Snapshot free is not implemented");
    g_free(chunk);
    break;
  default:
    g_warning("Uknown type of undo data %d", chunk->action);
  }
}



static DenemoUndoData ActionStageStart={ACTION_STAGE_START};
static DenemoUndoData  ActionStageEnd = {ACTION_STAGE_END};
static DenemoUndoData  ActionScriptError = {ACTION_SCRIPT_ERROR};
void stage_undo(DenemoScore *si, action_type type) {
  switch(type) {
  case ACTION_STAGE_START: {
    if(g_queue_is_empty(si->undodata))
      return;
    DenemoUndoData *chunk = g_queue_peek_head(si->undodata);
    if(chunk->action==ACTION_STAGE_END) {
      chunk = g_queue_pop_head(si->undodata);
      // free_chunk(chunk); not needed, is static anyway
      // g_print("Script did not need undoing");
    } else
      update_undo_info (si, &ActionStageStart);
  }
    break;
  case ACTION_STAGE_END:
    update_undo_info (si, &ActionStageEnd);
    break;
  case ACTION_SCRIPT_ERROR:
    update_undo_info (si, &ActionScriptError);
    break;
  default:
    g_warning("Unknown undo action %d will not be stored\n", type);
  }
}

// snapshot the current movement for undo
gboolean take_snapshot(void) {
  if (!Denemo.gui->si->undo_guard)
    {
      DenemoUndoData *chunk;
      chunk = (DenemoUndoData *) g_malloc (sizeof (DenemoUndoData));
      chunk->object = (DenemoObject*)clone_movement(Denemo.gui->si);
      //fix up somethings...
      get_position(Denemo.gui->si,&chunk->position);
      chunk->position.appending = 0;
      chunk->action = ACTION_SNAPSHOT;
      update_undo_info (Denemo.gui->si, chunk);
      
      return TRUE;
    } else
    return FALSE;
}

static void 
print_queue(gchar *msg, GQueue *q) {
  GList*g;
  g_debug("%s",msg);
  for(g=q->head;g;g=g->next) {
    DenemoUndoData *chunk = g->data;
    switch(chunk->action) {
    case ACTION_STAGE_START:
      g_print("[");
      break;
    case ACTION_STAGE_END:
      g_print("]\n");
      break;
    case ACTION_SNAPSHOT:
      g_print("Snapshot; ");
      break;
    case ACTION_INSERT:
      g_print("Ins; ");
      break;
    case ACTION_DELETE:
      g_print("Del %s; ", DenemoObjTypeNames[((DenemoObject*) chunk->object)->type]);
      break;
    case ACTION_CHANGE:
      g_print("Chn %s; ", DenemoObjTypeNames[((DenemoObject*) chunk->object)->type]);
      break;
    case ACTION_MEASURE_CREATE:
      g_print("Create; ");
      break;
    case ACTION_MEASURE_REMOVE:
      g_print("Remove; ");
      break;
    case ACTION_NOOP:
      g_print("No-op; ");
      break;
    default:
      g_print("Unknown action %d\n", chunk->action);

    }
  }
  g_print("\nEnd queue\n");
}


static gboolean position_for_chunk(DenemoGUI * gui, DenemoUndoData *chunk) {
  DenemoScriptParam param;
  param.status=TRUE;
  //g_print("undo guard before %d level is %d\n undo action is %d\n",  gui->si->undo_guard, gui->undo_level, chunk->action);

  switch(chunk->action) {
  case ACTION_CHANGE:
    if(chunk->position.object==0)
      return FALSE;//Cannot undo a change in an empty measure=>undo queue is corrupt
    //FALL THRU
  case ACTION_INSERT:
  case ACTION_DELETE:
  case ACTION_MEASURE_CREATE://this creates an (blank)measure
  case ACTION_MEASURE_REMOVE://this is the action that removes a blank measure at pos
    { 	
      push_given_position(&chunk->position);	
      PopPosition(NULL, &param);
    }
    break;
  case ACTION_NOOP:
    break;
  default:
    break;
  }
  return param.status;
}

//Takes the action needed for one chunk of undo/redo data

static void	action_chunk(DenemoGUI * gui, DenemoUndoData **pchunk) {
  DenemoUndoData *chunk = *pchunk;
	switch(chunk->action) {
	case ACTION_MEASURE_CREATE: {
	  //delete the empty measure in the chunk->position.staff at measure number chunk->position->object
	  dnm_deletemeasure(gui->si);
	  chunk->action = ACTION_MEASURE_REMOVE;
	  if(chunk->position.measure>1)
	    chunk->position.measure--;
	  else
	    chunk->action = ACTION_NOOP;

	  if(!gui->si->currentmeasure) {
	    g_warning("position after undo insert Bug in selectops.c");
	    position_for_chunk(gui, chunk);
	      //movetoend(NULL, NULL);
	  }
	}
	  break;

	case ACTION_MEASURE_REMOVE: {
	  //create empty measure in the chunk->position.staff at measure number chunk->position->object
	  insertmeasureafter(gui);
	  chunk->action = ACTION_MEASURE_CREATE;
	  chunk->position.measure++;
	  if(!gui->si->currentmeasure) {
	    g_warning("position after undo insert Bug in selectops.c");
	    position_for_chunk(gui, chunk);//????
	      //movetoend(NULL, NULL);
	  }
	}
	  break;



	case ACTION_STAGE_START:
	  gui->undo_level++;

	  *pchunk = &ActionStageEnd;
	  break;
	case ACTION_STAGE_END:
	  gui->undo_level--;
	  *pchunk = &ActionStageStart;

	  break;
	case ACTION_SCRIPT_ERROR:
	  //chunk = &ActionScriptError;
	  gui->undo_level = 0;
	  break;

	case ACTION_INSERT:
	  {
	    chunk->object = dnm_clone_object(gui->si->currentobject->data);
	    dnm_deleteobject (gui->si);	    
	    chunk->action = ACTION_DELETE;
	  }
	  break;
	case  ACTION_DELETE:
	  {	    
	    object_insert (gui, chunk->object);
	    chunk->action = ACTION_INSERT;
	    chunk->object = NULL;	    
	  }
	  break;
	case ACTION_CHANGE:
	  {
	    //FIXME guard against a corrupt undo queue here by checking  if(gui->si->currentobject) {
	    DenemoObject *temp = gui->si->currentobject->data;
	    gui->si->currentobject->data =  chunk->object;
	    chunk->object = temp;
	  }
	  break;
	case ACTION_SNAPSHOT:
	  {

	    DenemoScore *si = (DenemoScore*)chunk->object;
	    gint initial_guard = gui->si->undo_guard;
	    gint initial_changecount = gui->si->changecount;
	    gboolean initial_redo_invalid = gui->si->redo_invalid;
	    gpointer initial_smf = gui->si->smf;
	    // replace gui->si in gui->movements with si
	    GList *find = g_list_find(gui->movements, gui->si);
	    if(find) {
	      find->data = si;
	      GList *g, *gorig, *curstaff;
	      for(curstaff = gui->si->thescore;curstaff;curstaff=curstaff->next) {
		DenemoStaff *thestaff = curstaff->data;
		gorig =	g = thestaff->verses;
		thestaff->verses = NULL;
		for(;g;g=g->next) {
		  gchar *text = get_text_from_view (g->data);
		  gtk_widget_destroy(g->data);//what about its parent??? FIXME
		  thestaff->verses = g_list_append(thestaff->verses, text);
		  if(thestaff->currentverse==g)
		    thestaff->currentverse = g_list_last(thestaff->verses);
		}

		{GList *direc;
		  for(direc=thestaff->staff_directives;direc;direc=direc->next) {
		    DenemoDirective *directive = direc->data;
		    if(directive->widget) {
		    gtk_widget_destroy(directive->widget);
		    directive->widget = NULL;
		    }
		    //widget_for_staff_directive(directive);
		  }
		}
		{GList *direc;
		  for(direc=thestaff->voice_directives;direc;direc=direc->next) {
		    DenemoDirective *directive = direc->data;
		    if(directive->widget) {		    
		    gtk_widget_destroy(directive->widget);
		    directive->widget = NULL;
		    }
		    //widget_for_voice_directive(directive);
		  }
		}
	      }


	      {GList *direc;
	      for(direc=gui->si->movementcontrol.directives;direc;direc=direc->next) {
		DenemoDirective *directive = direc->data;
		gtk_widget_destroy(directive->widget);
		directive->widget = NULL;

	      }
	    } 
	    {GList *direc;
	      for(direc=gui->si->header.directives;direc;direc=direc->next) {
		DenemoDirective *directive = direc->data;
		gtk_widget_destroy(directive->widget);
		directive->widget = NULL;
	      }
	    }
	    {GList *direc;
	      for(direc=gui->si->layout.directives;direc;direc=direc->next) {
		DenemoDirective *directive = direc->data;
		gtk_widget_destroy(directive->widget);
		directive->widget = NULL;
	      }
	    }

	    g_list_free(gorig);
	    chunk->object = (DenemoObject*)gui->si;
	    //FIXME fix up other values in stored object si?????? voice/staff directive widgets
	    gui->si = si;
	    for(curstaff = si->thescore;curstaff;curstaff=curstaff->next) {
	      DenemoStaff *thestaff = curstaff->data;
	      gorig = g = thestaff->verses;
	      gint curversenum = g_list_position(g, thestaff->currentverse);
	      thestaff->verses = NULL;
		
	      for(;g;g=g->next) {
		add_verse_to_staff(si, thestaff);
		gtk_text_buffer_set_text (gtk_text_view_get_buffer ((GtkTextView *) thestaff->currentverse->data), g->data, -1);
		gtk_widget_show(thestaff->currentverse->data);
		g_signal_connect (G_OBJECT (gtk_text_view_get_buffer (thestaff->currentverse->data)), "changed", G_CALLBACK (lyric_change), NULL);
	      }
	      thestaff->currentverse = g_list_nth(thestaff->verses, curversenum);


	      {GList *direc;
		for(direc=thestaff->staff_directives;direc;direc=direc->next) {
		  DenemoDirective *directive = direc->data;	    
		  directive->widget = NULL;   
		  widget_for_staff_directive(directive, thestaff->staffmenu);
		}
	      }
	      {GList *direc;
		for(direc=thestaff->voice_directives;direc;direc=direc->next) {
		  DenemoDirective *directive = direc->data;
		  directive->widget = NULL;
		  widget_for_voice_directive(directive, thestaff->voicemenu);
		}
	      }
	      g_list_free(gorig);
	    }
	    

	    {GList *direc;
	      for(direc=gui->si->movementcontrol.directives;direc;direc=direc->next) {
		DenemoDirective *directive = direc->data;
		directive->widget = NULL;
		widget_for_movementcontrol_directive(directive);
	      }
	    } 
	    {GList *direc;
	      for(direc=gui->si->header.directives;direc;direc=direc->next) {
		DenemoDirective *directive = direc->data;
		directive->widget = NULL;
		widget_for_header_directive(directive);
	      }
	    }
	    {GList *direc;
	      for(direc=gui->si->layout.directives;direc;direc=direc->next) {
		DenemoDirective *directive = direc->data;
		directive->widget = NULL;
		widget_for_layout_directive(directive);
	      }
	    }
	    gui->si->smf = initial_smf;
	    gui->si->smfsync = -1;//force recalculation of midi
	    gui->si->redo_invalid = initial_redo_invalid;
	    gui->si->undo_guard = initial_guard;//we keep all the guards we had on entry which will be removed when
	    gui->si->changecount = initial_changecount;
	    position_for_chunk(gui, chunk);//FIXME check return val
	    if(!gui->si->currentmeasure) {
	      g_warning("positioning after snapshot Bug in selectops.c");
	      movetoend(NULL, NULL);
	    }	    
	    gui->si->currentstaffnum = 1+g_list_position(gui->si->thescore, gui->si->currentstaff);	    
	    }
	    else {
	      g_critical("Movement does not exist in list of movements");
	    }
	  }
	  break;
	case ACTION_NOOP:
	  break;

	default:
	  g_warning("Unexpected undo case ");
	}
}




static void position_warning(DenemoUndoData *chunk) {
  g_warning("Could not find position for undotype %d  movement %d staff %d measure %d object %d appending %d offend %d\n",
	    chunk->action,
	    chunk->position.movement,
	    chunk->position.staff,
	    chunk->position.measure,
	    chunk->position.object,
	    chunk->position.appending,
	    chunk->position.offend);
  print_queue("The undo queue was:", Denemo.gui->si->undodata);
  print_queue("The redo queue was:", Denemo.gui->si->redodata);
}
static void
warn_no_more_undo(DenemoGUI *gui) 
{
  g_warning("No more undo information at level %d guard %d ... resetting", gui->undo_level, gui->si->undo_guard );
  gui->undo_level = 0;
  gui->si->undo_guard = Denemo.prefs.disable_undo;
}



static void 
free_queue( GQueue *queue) {
  DenemoUndoData *chunk;
  //g_print("before redo queue %p is %d empty\n", queue, g_queue_is_empty(queue));
  while((chunk = (DenemoUndoData *) g_queue_pop_head (queue)))
    free_chunk(chunk);
  //g_print("after redo queue %p is %d empty\n", queue, g_queue_is_empty(queue));
}

/**
 * undo
 * Undoes an insert, delete change of a DenemoObject, transferring the undo object to the redo queue and switching it between delete/insert
 * Undoes other changes to movement by returning to a snapshot.
 *
 * PARAM gui  the score (why??? this is per movement undo FIXME)
 */
static void
undo (DenemoGUI * gui)
{

  DenemoUndoData *chunk = (DenemoUndoData *) g_queue_pop_head (gui->si->undodata);
  if (chunk)
    {      
      gui->si->undo_guard++;
      //g_print("undo %d\n", chunk->action);
      if(position_for_chunk(gui, chunk)) {
	action_chunk(gui, &chunk);
      } else {
	position_warning(chunk);
	free_queue(gui->si->redodata);
	free_queue(gui->si->undodata);
	warn_no_more_undo(gui);//returns guard to user preference and sets level 0
	return;
      }
      //g_print("actioned undo now pushing %d\n", chunk->action);
      update_redo_info (gui->si, chunk);	  
      gui->si->undo_guard--;
      //g_print("***undo guard after undo %d\n",  gui->si->undo_guard);
      if(gui->undo_level>0)
	undo(gui);
      score_status(gui, TRUE);
      //print_queue("Undo, queue: ", gui->si->undodata);
    } else
    warn_no_more_undo(gui);
}



/**
 * redo
 * Takes objects from the redo queue and actions them, staged by ACTION_STAGE_START/END
 * Once actioned they are transferred back to the undo queue, with inverse transformation
 *
 * Input
 * scoreinfo - score data
 */
void
redo (DenemoGUI * gui)
{  
  DenemoUndoData *chunk = (DenemoUndoData *) g_queue_pop_head (gui->si->redodata);
  if (chunk)
    {
      //g_print("Before %s and %d\n", gui->si->currentobject?"Obj":"noObj", gui->si->cursor_x);
     gui->si->undo_guard++;
      if(position_for_chunk(gui, chunk)) {
	action_chunk(gui, &chunk);
      } else {
	position_warning(chunk);

      }
      update_undo_info (gui->si, chunk);	  
      gui->si->undo_guard--;
      //g_print("After %s and %d\n", gui->si->currentobject?"Obj":"noObj!!", gui->si->cursor_x);
      if(gui->undo_level>0)
	redo(gui);
      score_status(gui, TRUE);
    } else
    warn_no_more_undo(gui);
}




/**
 *  update_undo_info
 *  
 *  Updates the undo list with current operation.
 *  Is passed score structure and undo_data structure
 *
 */
void
update_undo_info (DenemoScore * si, DenemoUndoData * undo)
{
  DenemoUndoData *tmp = NULL;


  //g_print ("Adding: Action %d at pos %d appending %d\n",  undo->action, undo->position.object, undo->position.appending); 

  //  if (g_queue_get_length (si->undodata) == MAX_UNDOS)
  //    {
  //      tmp = g_queue_pop_tail (si->undodata);//FIXME freeing undo info, especially the object
  //      g_warning("Lost undo of %p %p\n", tmp, tmp->object);
  //   }

  g_queue_push_head (si->undodata, undo);
  si->redo_invalid = TRUE;
  // print_queue("\nUpdate Undo, queue:", si->undodata);
}


/**
 * update_redo_info
 *  
 *  Updates the redo list with last undo operation.
 *  Is passed score structure and redo_data structure
 *  @param si pointer to the DenemoScore structure
 *  @param redo redo data structure to prepend to the queue
g */

void
update_redo_info (DenemoScore * si, DenemoUndoData * redo)
{
  DenemoUndoData *tmp = NULL;
  //print_queue("Update redo ******************\nUndo queue:\n", si->undodata);
  //print_queue("Update redo ******************\nredo queue:\n", si->redodata);


  if(si->redo_invalid)
    {
      free_queue(si->redodata);
      si->redo_invalid = FALSE;
      //g_print("queue = %p\n", si->redodata);
    }
  g_queue_push_head (si->redodata, redo);
}
