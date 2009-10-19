/**
 * staffops.cpp
 * functions dealing with whole staffs
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 1999-2005 Matthew Hiller 
 */

#include "chordops.h"
#include "contexts.h"
#include <denemo/denemo.h>
#include "dialogs.h"
#include "measureops.h"
#include "moveviewport.h"
#include "objops.h"
#include "processstaffname.h"
#include "staffops.h"
#include <stdlib.h>
#include <string.h>
#include "calculatepositions.h"
#include "commandfuncs.h"
#include "lilydirectives.h"

#ifdef _HAVE_JACK_
#include "jackmidi.h"
#endif

/**
 * Return the first object node of the given measure
 * @param mnode a measurenode
 * @return the first object node of the measure
 */
objnode *
firstobjnode (measurenode * mnode)
{
  return (objnode *) mnode->data;
}

/**
 * Return the last object node of the given measure
 * @param mnode a measurenode
 * @return the last object node of the measure
 */
objnode *
lastobjnode (measurenode * mnode)
{
  return g_list_last ((objnode *) mnode->data);
}

/**
 * Return the first measure node of the given staffops
 * @param thestaff a staffnode
 * @return the first measure node of the staff
 */
measurenode *
firstmeasurenode (staffnode * thestaff)
{
  return ((DenemoStaff *) thestaff->data)->measures;
}

/**
 * Return the nth measure of the given staff
 * @param thestaff a staffnode
 * @param n the number of the measure to return
 * @return the nth measure of the staff
 */
measurenode *
nth_measure_node_in_staff (staffnode * thestaff, gint n)
{
  return g_list_nth (((DenemoStaff *) thestaff->data)->measures, n);
}

/**
 * Return the first object node in the given staff
 * @param thestaff a staffnode
 * @return the first object node of the staff
 */
objnode *
firstobjnodeinstaff (staffnode * thestaff)
{
  return firstobjnode (firstmeasurenode (thestaff));
}

/**
 * Reset si->currentprimarystaff based on current value of
 * si->currentstaff 
 * @param si a scoreinfo structure
 * @return none
 */
void
setcurrentprimarystaff (DenemoScore * si)
{
  for (si->currentprimarystaff = si->currentstaff;
       ((DenemoStaff *) si->currentprimarystaff->data)->voicenumber != 1;
       si->currentprimarystaff = si->currentprimarystaff->prev)
    ;
}

/**
 * Copies the staff data from a source staff to destination staff
 * @param src the source staff
 * @param dest the destination staff
 * @return none
 */
static void
copy_staff_bits (DenemoStaff * src, DenemoStaff * dest)
{
  dest->clef.type = src->clef.type;//other fields - take care if dynamic
  dest->keysig.number = src->keysig.number;
  dest->keysig.isminor = src->keysig.isminor;
  memcpy (dest->keysig.accs, src->keysig.accs, SEVENGINTS);
  dest->timesig.time1 = src->timesig.time1;
  dest->timesig.time2 = src->timesig.time2;
  dest->volume = 65;
  dest->no_of_lines = 5;
  dest->transposition = 0;
  dest->pos_in_half_lines = 0;
  dest->space_above = 0;
  dest->space_below = 0;
  dest->context = DENEMO_NONE;
}

/**
 * Copies a staffs parameters from source to destination
 * @param src the source staff
 * @param dest the destination staff
 * @return none
 */
static void
copy_staff_properties (DenemoStaff * src, DenemoStaff * dest)
{
  set_lily_name (dest->denemo_name, dest->lily_name);

  /* !!!! Insert advisory function for detecting colliding staff names
   * here */

  dest->midi_instrument = g_string_new (src->midi_instrument->str);
  dest->space_above = src->space_above;
  dest->space_below = src->space_below;
  dest->no_of_lines = src->no_of_lines;
  dest->transposition = src->transposition;
  dest->pos_in_half_lines = src->pos_in_half_lines;
  dest->volume = src->volume;
  dest->voicenumber = 2;
  beamsandstemdirswholestaff (dest);

}

/**
 * Insert a new staff into the score
 * @param si the scoreinfo structure
 * @param thestaffstruct the staff to insert
 * @param action where to insert the new staff
 * @param addat the position to insert at
 * @return none
 */
static void
insert_staff (DenemoScore * si, DenemoStaff * thestaffstruct,
	      enum newstaffcallbackaction action, gint addat)
{
  si->thescore = g_list_insert (si->thescore, thestaffstruct, addat - 1);
  if (action != BEFORE)
    if (action != AFTER) {
      si->currentstaff = g_list_nth (si->thescore, addat - 1);
      si->currentstaffnum = addat;
      setcurrentprimarystaff (si);
      find_leftmost_staffcontext (thestaffstruct, si);
    }

}


/**
 * Create and insert a new staff into the score
 * @param si the scoreinfo structure
 * @param action the staffs type / where to insert it
 * @param context the staffs contexts
 * @return none
 */
void
newstaff (DenemoGUI * gui, enum newstaffcallbackaction action,
	  DenemoContext context)
{
  DenemoScore *si = gui->si;
  g_assert (si != NULL);
  /* What gets added */
  gint ret = -1, err;
  DenemoStaff *thestaffstruct =
    (DenemoStaff *) g_malloc (sizeof (DenemoStaff));

  //CREATE a GtkMenus to popup when clicking to left of the staff
  {
    //hmm, cannot populate it, why not ? FIXME 
    GtkWidget *item;
    thestaffstruct->staffmenu = (GtkMenu *)gtk_menu_new();
    item = gtk_menu_item_new_with_label("Edit Attributes");
    gtk_menu_shell_append(GTK_MENU_SHELL( thestaffstruct->staffmenu), item);
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(edit_staff_directive), NULL);
    gtk_widget_show(item);
    thestaffstruct->voicemenu = (GtkMenu *)gtk_menu_new();
    item = gtk_menu_item_new_with_label("Edit Attributes");
    gtk_menu_shell_append(GTK_MENU_SHELL( thestaffstruct->voicemenu), item);
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(edit_voice_directive), NULL);
    gtk_widget_show(item);
  }

  struct newstaffinfotopass itp;
  measurenode *themeasures = NULL;	/* Initial set of measures in staff */
  gint numstaffs = g_list_length (si->thescore);
  gint i, n, addat = 1;
  if (si->lily_file)
    return;			/* no code for this yet - just edit textually */
#ifdef DEBUG
  g_print ("newstaff: Num staffs %d\n", numstaffs);
#endif
  if (numstaffs == 0)
    {
      action = INITIAL;
      n = 1;

      thestaffstruct->clef.type = DENEMO_TREBLE_CLEF;
      thestaffstruct->keysig.number = 0;
      thestaffstruct->keysig.isminor = FALSE;
      memset (thestaffstruct->keysig.accs, 0, SEVENGINTS);
      thestaffstruct->timesig.time1 = 4;
      thestaffstruct->timesig.time2 = 4;
      thestaffstruct->volume = 65;
      thestaffstruct->no_of_lines = 5;
      thestaffstruct->transposition = 0;
      thestaffstruct->pos_in_half_lines = 0;
      thestaffstruct->space_above = 0;
      thestaffstruct->space_below = 0;
      thestaffstruct->nummeasures = 1;

#if 0
      si->measurewidths = g_list_append (si->measurewidths,
					 GINT_TO_POINTER (si->measurewidth));
#else
      si->measurewidths = g_list_append (NULL,  GINT_TO_POINTER (si->measurewidth));//FIXME free old measurewidths
#endif
    }
  else {
      /* how did this work before? a new staff must have the same number of measures as the present one(s) */
      thestaffstruct->nummeasures = g_list_length (firstmeasurenode (si->thescore));
      copy_staff_bits ((DenemoStaff *) si->currentstaff->data,
		       thestaffstruct);
    }

  if (action == NEWVOICE)
    {
      thestaffstruct->voicenumber = 2;
      thestaffstruct->nummeasures =
	g_list_length (firstmeasurenode (si->currentstaff));//FIXME redundant
    }
  else if (action == LYRICSTAFF)
    {
      thestaffstruct->voicenumber = 3;
    }
  else if (action == FIGURESTAFF)
    {
      thestaffstruct->voicenumber = 3;	/* what does this mean? */
    }
  else if (action == CHORDSTAFF)
    {
      thestaffstruct->voicenumber = 3;
    }
  else
    {
      thestaffstruct->voicenumber = 1;
    };

  for (i = 0; i < thestaffstruct->nummeasures; i++)
    {
      themeasures = g_list_append (themeasures, NULL);
    };

  if (action == INITIAL || action == ADDFROMLOAD)
    {
      si->currentmeasure = themeasures;
    }

#if 0
  // always go home for new staffs.
 else {
      si->currentmeasure = g_list_nth(themeasures, si->currentmeasurenum - 1);

    }
#endif

  /* Now fix the stuff that shouldn't be directly copied from
   * the current staff, if this staff was non-initial and that
   * was done to begin with */

  thestaffstruct->measures = themeasures;
  thestaffstruct->denemo_name = g_string_new (NULL);
  thestaffstruct->lily_name = g_string_new (NULL);

  thestaffstruct->staff_prolog = NULL;
  thestaffstruct->lyrics_prolog = NULL;
  thestaffstruct->figures_prolog = NULL;
  thestaffstruct->fakechords_prolog = NULL;
  thestaffstruct->context = context;
  if (action == NEWVOICE)
    g_string_sprintf (thestaffstruct->denemo_name, _("poly voice %d"),
		      numstaffs + 1);
  else if (action == LYRICSTAFF)
    g_string_sprintf (thestaffstruct->denemo_name, _("lyrics"));
  else if (action == FIGURESTAFF)
    g_string_sprintf (thestaffstruct->denemo_name, _("figures"));
  else
    g_string_sprintf (thestaffstruct->denemo_name, _("voice %d"),
		      numstaffs + 1);
  set_lily_name (thestaffstruct->denemo_name, thestaffstruct->lily_name);
  thestaffstruct->midi_instrument = g_string_new ("acoustic grand");
  thestaffstruct->leftmost_timesig = &thestaffstruct->timesig;

  /* In what position should the scrollbar be added?  */
  switch (action)
    {
    case INITIAL:
      addat = 1;
      break;
    case LAST:
    case ADDFROMLOAD:
      addat = numstaffs + 1;
      break;
    case BEFORE:
      addat = si->currentstaffnum;
      break;
    case AFTER:
    case NEWVOICE:
    case LYRICSTAFF:
    case FIGURESTAFF:
      addat = si->currentstaffnum + 1;
      break;
    case CHORDSTAFF:
      addat = si->currentstaffnum + 1;
      break;
    case FIRST:
      addat = 1;
      break;
    }
  itp.gui = gui;
  itp.staff = thestaffstruct;
  itp.addat = addat;
  
  if (action != INITIAL && action != ADDFROMLOAD)
    {
      if (action == NEWVOICE)
	{
	  copy_staff_properties ((DenemoStaff *) si->currentstaff->data,
				 thestaffstruct);
	  insert_staff (si, thestaffstruct, action, addat);
	}
      else
	{
	  //	  thestaffstruct->jack_midi_out_port = -1;
	  //	  ret = staff_properties_change (&itp);
	  //	  if (ret)
	    {
	      /* 
	         If staff_properties_change returns false,
	         then the staff should probably be removed
	         Fixed 09042005 Adam Tee 
	       */
	      insert_staff (si, thestaffstruct, action, addat);
	      si->currentmeasurenum = 1;
	      /*
	         Reset leftmeasure num to 1 to be at the start of 
	         the next staff. 
	       */
	      si->leftmeasurenum = 1;
	    }
	    //	  else
	    //	    {
	      	      /*
	       *  Free the staff struct as it has not been inserted 
	       *  into the score
	       */
	    //     g_free (thestaffstruct);
	    //  }
	}
    }
  else // is INITIAL or ADDFROMLOAD
    {
      insert_staff (si, thestaffstruct, action, addat);
      si->leftmeasurenum = 1;
    }
#ifdef _HAVE_JACK_
      thestaffstruct->jack_midi_out_port = create_jack_midi_port(thestaffstruct->lily_name->str);
#endif

  //si->haschanged = TRUE;
}


gboolean confirm_insertstaff_custom_scoreblock(DenemoGUI *gui) {
  if(gui->custom_scoreblocks)
    return confirm("Custom LilyPond Score Block", "You will need to edit the LilyPond text to make this change show in the print-out. Proceed?");
  return TRUE;
}

gboolean confirm_deletestaff_custom_scoreblock(DenemoGUI *gui) {
  if(gui->custom_scoreblocks)
    return confirm("Custom LilyPond Score Block", "You will need to edit the LilyPond text to delete any reference to the deleted staff(s) in the scoreblock. Proceed?");
  return TRUE;
}


/**
 * Remove the gui->si->currentstaff from the piece gui and reset si->currentstaff
 * if only one staff, inserts a new empty one
 * if interactive checks for custom_scoreblock
 * if a staff is deleted, updates the changecount
 * @param gui the DenemoGUI structure
 * @return nothing
 */
void
deletestaff (DenemoGUI * gui, gboolean interactive)
{
  DenemoScore *si = gui->si;
  DenemoStaff *curstaffstruct = si->currentstaff->data;
 
#ifdef _HAVE_JACK_
  int err = remove_jack_midi_port(curstaffstruct->jack_midi_out_port);
#endif

  if(interactive && !confirm_deletestaff_custom_scoreblock(gui))
    return;
  if(si->currentstaff==NULL)
    return;
  
  gboolean give_info=FALSE;//give info about removing matching context
  if(interactive && (curstaffstruct->context!=DENEMO_NONE) &&
     (!confirm("A context is set on this staff", "You will need to alter/delete the matching staff; Proceed?")))
    return;
  if(interactive &&  (curstaffstruct->context!=DENEMO_NONE))
    give_info = TRUE;
    
  gboolean isprimary = ((int) curstaffstruct->voicenumber == 1);
  //FIXME free_staff()

  free_directives(curstaffstruct->staff_directives);
  free_directives(curstaffstruct->timesig.directives);
  free_directives(curstaffstruct->keysig.directives);

  gtk_widget_destroy((GtkWidget *)(curstaffstruct->staffmenu));
  gtk_widget_destroy((GtkWidget *)(curstaffstruct->voicemenu));


  g_list_foreach (curstaffstruct->measures, freeobjlist, NULL);
  g_list_free (curstaffstruct->measures);
  g_string_free (curstaffstruct->denemo_name, FALSE);//FIXME these should all be TRUE??
  g_string_free (curstaffstruct->lily_name, FALSE);
  g_string_free (curstaffstruct->midi_instrument, FALSE);
  g_list_foreach (curstaffstruct->verses, (GFunc)gtk_widget_destroy, NULL);
  g_free (curstaffstruct);




  if(si->currentstaff==g_list_last(si->thescore))
    si->currentstaffnum--;//deleting the last, so the currentstaffnum must decrease
  si->thescore = g_list_delete_link (si->thescore, si->currentstaff);
  if(si->thescore==NULL) {
    newstaff (gui, INITIAL, DENEMO_NONE);
  }
  si->currentstaff = g_list_nth (si->thescore, si->currentstaffnum - 1);


  if (isprimary) // we deleted the primary, so the next one must become the primary
    {
      ((DenemoStaff *) si->currentstaff->data)->voicenumber = 1;
      si->currentprimarystaff = si->currentstaff;
    } else {
      setcurrentprimarystaff (si);
    }

  //FIXME none of this works to get the current measure stem direction correct
  setcurrents (si);
      //     find_xes_in_all_measures (si);
      //   beamsandstemdirswholestaff ((DenemoStaff *) si->currentstaff->data);
      //   si->markstaffnum = 0;
      //  displayhelper (gui);
  //  if(gui->si->currentstaffnum > gui->si->top_staff)
  //    gui->si->top_staff++;
  //  else
  if(gui->si->currentstaffnum < gui->si->top_staff)
    gui->si->top_staff = gui->si->currentstaffnum; 
  update_vscrollbar (gui);

  displayhelper (gui);
  score_status(gui,TRUE);
  if(give_info)
    infodialog ("The staff deleted had a start/end context; if you still have the staff with the matching end/start context\n then you should remove it (or its context) now.\nSee Staff->properties->context\nYou will not be able to print with miss-matched contexts.");
  return;
}	

/**
 * Sets the beams and stem directions across the given staff
 * @param thestaff a staff structure
 * @return none
 */
void
beamsandstemdirswholestaff (DenemoStaff * thestaff)
{
  measurenode *curmeasure;
  gint nclef, time1, time2, stem_directive;

  nclef = thestaff->clef.type;
  time1 = thestaff->timesig.time1;
  time2 = thestaff->timesig.time2;
  stem_directive = DENEMO_STEMBOTH;

  for (curmeasure = thestaff->measures; curmeasure;
       curmeasure = curmeasure->next)
    {
      calculatebeamsandstemdirs ((objnode *) curmeasure->data, &nclef, &time1,
				 &time2, &stem_directive);
    }
}

/**
 * Sets which accidentals to show across a staff on a key sig change
 * @param thestaff a staff stucture
 * @return none
 */
void
showwhichaccidentalswholestaff (DenemoStaff * thestaff)
{
  gint feed[7];
  gint feednum;
  measurenode *curmeasure;

  memcpy (feed, thestaff->keysig.accs, SEVENGINTS);
  feednum = thestaff->keysig.number;
  for (curmeasure = thestaff->measures; curmeasure;
       curmeasure = curmeasure->next)
    feednum =
      showwhichaccidentals ((objnode *) curmeasure->data, feednum, feed);
}

/**
 * Function to set the note positions on the given staff when there is a clef change
 * @param thestaff a staff structure
 * @return none 
 */
void
fixnoteheights (DenemoStaff * thestaff)
{
  gint nclef = thestaff->clef.type;
  //gint time1 = thestaff->stime1;//USELESS
  //gint time2 = thestaff->stime2;//USELESS
  //gint initialclef;//USELESS
  measurenode *curmeasure;
  objnode *curobj;
  DenemoObject *theobj;

  for (curmeasure = thestaff->measures; curmeasure;
       curmeasure = curmeasure->next)
    {
      //initialclef = nclef;
      for (curobj = (objnode *) curmeasure->data; curobj;
	   curobj = curobj->next)
	{
	  theobj = (DenemoObject *) curobj->data;
	  switch (theobj->type)
	    {
	    case CHORD:
	      newclefify (theobj, nclef);
	      break;
	    case TIMESIG://USELESS
	      //time1 = ((timesig *) theobj->object)->time1;
	      // time2 = ((timesig *) theobj->object)->time2;
	      break;
	    case CLEF:
	      nclef = ((clef *) theobj->object)->type;
	      break;
	    default:
	      break;
	    }
	}			/* End for */
    }				/* End for */
  beamsandstemdirswholestaff (thestaff);
}



/**
 * Callback function to insert a staff in the initial position
 * @param action a Gtk Action
 * @param gui the DenemoGUI structure
 * @return none
 */
void
newstaffinitial (GtkAction *action, gpointer param)
{
  DenemoGUI *gui = Denemo.gui;
  while(gui->si->currentstaff && gui->si->currentstaff->prev)
    staffup(param);
  newstaffbefore (action, NULL);
}

/**
 Callback function to insert a staff before the current staff
 * @param action a Gtk Action
 * @param gui the DenemoGUI structure
 * @return none
 */
void
newstaffbefore (GtkAction *action, gpointer param)
{
  DenemoGUI *gui = Denemo.gui;
  if(!confirm_insertstaff_custom_scoreblock(gui))
    return;
  tohome(NULL, NULL);
  newstaff (gui, BEFORE, DENEMO_NONE);
  if(gui->si->currentstaffnum>= gui->si->top_staff)
    gui->si->top_staff++;
  gui->si->currentstaffnum++;
  gui->si->bottom_staff++;
  set_bottom_staff (gui);
  move_viewport_down (gui);

  staffup (param);
  displayhelper (gui);

}

/**
 * Callback function to insert a staff after the current staff
 * @param action a Gtk Action
 * @param gui the DenemoGUI structure
 * @return none
 */
void
dnm_newstaffafter (GtkAction *action, gpointer param)
{
  DenemoGUI *gui = Denemo.gui;
  if(!confirm_insertstaff_custom_scoreblock(gui))
    return;
  tohome(NULL, NULL);
  newstaff (gui, AFTER, DENEMO_NONE);
  set_bottom_staff (gui);
  update_vscrollbar (gui);
  staffdown (param);
  displayhelper (gui);
}

/**
 * Callback function to insert a staff at the bottom of the score
 * @param action a Gtk Action
 * @param gui the DenemoGUI structure
 * @return none
 */
void
newstafflast (GtkAction *action, gpointer param)
{
  DenemoGUI *gui = Denemo.gui;
  while(gui->si->currentstaff && gui->si->currentstaff->next)
    staffdown(param);
  dnm_newstaffafter(action, param);
}

/**
 * Callback function to add a new voice to the current staff
 * @param action a Gtk Action
 * @param gui the DenemoGUI structure
 * @return none
 */
void
dnm_newstaffvoice (GtkAction *action, gpointer param)
{
  DenemoGUI *gui = Denemo.gui;
  newstaff (gui, NEWVOICE, DENEMO_NONE);
  set_bottom_staff (gui);
  update_vscrollbar (gui);
  setcurrents(gui->si);
  displayhelper (gui);
}

