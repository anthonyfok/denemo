/* measureops.cpp
 * functions dealing with measures
 *
 * for Denemo, a gtk+ frontent to GNU Lilypond
 * (c) 2000-2005 Matthew Hiller, Adam Tee
 */

#include "accwidths.h"
#include "chordops.h"
#include <denemo/denemo.h>
#include "drawingprims.h"
#include "measureops.h"
#include "notewidths.h"
#include "objops.h"
#include "staffops.h"
#include "string.h"
#include "utils.h"
#include "barline.h"

#define STEMDIFFERENCE 6
#define HALFSTEMDIFFERENCE 3


/**
 * Adds measures to the score at given position, and returns the
 * measurenode * to the first one in currentstaff 
 * @param si the scoreinfo structure
 * @param pos position in staff to insert measures
 * @param nummeasures number of measures to insert
 * @param all append across all staffs
 * @return measurenode pointer to the first added in the current staff
 */
measurenode *
addmeasures (DenemoScore * si, gint pos, guint nummeasures, gint all)
{
  staffnode *curstaff;
  guint i;
  objnode *barlinenode = NULL;

  for (i = 0; i < nummeasures; i++)
    {
      if (all)
	{
	  for (curstaff = si->thescore; curstaff; curstaff = curstaff->next)
	    {
	      if (((DenemoStaff *) curstaff->data)->is_parasite)
		{
		  ((DenemoStaff *) curstaff->data)->measures =
		    *(((DenemoStaff *) curstaff->data)->is_parasite);
		  continue;
		  /*assumes parasite staff always come before their hosts */
		}
	      ((DenemoStaff *) curstaff->data)->measures =
		g_list_insert (firstmeasurenode (curstaff), barlinenode, pos);
	      ((DenemoStaff *) curstaff->data)->nummeasures++;
	    }
	}
      else
	{
	  ((DenemoStaff *) si->currentstaff->data)->measures =
	    g_list_insert (firstmeasurenode (si->currentstaff),
			   barlinenode, pos);
	  ((DenemoStaff *) si->currentstaff->data)->nummeasures++;

	}
      if (g_list_length (si->measurewidths) <
	  ((DenemoStaff *) si->currentstaff->data)->nummeasures)
	{
/* ???????????????? what if different staffs have different nummeasures??????? */
	  si->measurewidths = g_list_insert (si->measurewidths,
					     GINT_TO_POINTER (si->
							      measurewidth),
					     pos);
	}


    }


  return g_list_nth (firstmeasurenode (si->currentstaff), pos);
}

measurenode *
dnm_addmeasures (DenemoScore * si, gint pos, guint nummeasures, gint all)
{
	addmeasures (si,pos,nummeasures,all);
}
/**
 * g_list_foreach function called by freeobjlist
 */
static void
freeit_object (gpointer data, gpointer user_data)
{
  freeobject ((DenemoObject *) data);
}

/**
 * Free a measures objects 
 *
 */
void
freeobjlist (gpointer data, gpointer user_data)
{
  objnode *delobjs = (objnode *) data;
  if (delobjs)
    {
      /* Free all the mudela objects */
      g_list_foreach (delobjs, freeit_object, NULL);
      /* Free the object list itself */
      g_list_free (delobjs);
    }
}


/**
 * staffremovemeasures
 * Contains common code to remove a measure from a staff
 *
 * @param curstaff the staff to remove the measure from
 * @param pos the position in the staff to remove the measure from
 *
 */
void
staffremovemeasures (staffnode * curstaff, guint pos)
{
  //g_print ("In Staffremovemeasures\n");
  measurenode *firstmeasure;
  measurenode *delmeasure;
  if (((DenemoStaff *) curstaff->data)->is_parasite)
    {
      ((DenemoStaff *) curstaff->data)->measures =
	*((DenemoStaff *) curstaff->data)->is_parasite;
    }
  firstmeasure = firstmeasurenode (curstaff);
  delmeasure = g_list_nth (firstmeasure, pos);
  if(delmeasure) {
#ifdef DEBUG
    g_print ("Firstmeasure %x\t DelMeasure %x \t Position\n",
	     firstmeasure, delmeasure, pos);
#endif
    
    freeobjlist (delmeasure->data, NULL);
    ((DenemoStaff *) curstaff->data)->measures =
      g_list_remove_link (firstmeasure, delmeasure);
    g_list_free_1 (delmeasure);
    
    ((DenemoStaff *) curstaff->data)->nummeasures--;
  }
}

/**
 * Remove measures at given position, and return an appropriate
 * currentmeasure
 *
 * @param si pointer to the scoreinfo structure
 * @param pos position to remove the measures from
 * @param nummeasures number of measures to remove
 * @param all remove from all staffs
 */
measurenode *
removemeasures (DenemoScore * si, guint pos, guint nummeasures, gboolean all)
{
  staffnode *curstaff;
  measurenode *firstmeasure;
  measurenode *delmeasure;
  GList *temp;
	guint totalmeasures = 0;
  guint i;

  if (nummeasures <=
      g_list_length (firstmeasurenode ((staffnode *) si->currentstaff)) - pos)
  {
		for (i = 0; i < nummeasures; i++)
		  {
		    
		    
		    for (curstaff = si->thescore; curstaff;
			 curstaff = curstaff->next)
		      {
			if (curstaff==si->currentstaff || all) {
			  staffremovemeasures (curstaff, pos);
			  if(!firstmeasurenode(curstaff)) {
			    ((DenemoStaff *) curstaff->data)->measures =
			      g_list_append (NULL, NULL);
			    ((DenemoStaff *) curstaff->data)->nummeasures=1;
			  } 	    
			}
		      }
		 
		  
			for(curstaff = si->thescore; curstaff; curstaff = curstaff->next)
			{
				totalmeasures = MAX(totalmeasures ,((DenemoStaff *)curstaff->data)->nummeasures);
			}
			
		  if(totalmeasures <= (g_list_length(si->measurewidths) - 1))
		  {
			  /* And get rid of the no-longer-needed width data too */
			  temp = g_list_nth (si->measurewidths, pos);
			  si->measurewidths = g_list_remove_link (si->measurewidths, temp);
			  g_list_free_1 (temp);
		  }
	  }
    }
  else
    {
      g_warning (_("removemeasures: received request to delete more measures\
                   than exist.  Junking request."));
      return si->currentmeasure;
    }
  firstmeasure = firstmeasurenode (si->currentstaff);
  if (pos ==
      g_list_length (firstmeasurenode ((staffnode *) si->currentstaff)))
    {
      /* That is, we deleted the last measure */
      si->currentmeasurenum--;
      return g_list_nth (firstmeasure, pos - 1);
    }
  else
    return g_list_nth (firstmeasure, pos);
}

/**
 * This function calculates the number of ticks per beat in a given
 * time signature 
 */
gint
calcticksperbeat (gint time1, gint time2)
{
  gint ret = WHOLE_NUMTICKS / time2;

  /* If time1 is divisible by three and time2 is greater than 4;
   * e.g.,the time signature is 3/8, 15/16, etc., it's assumed that
   * the beat group is actually 3 times what's above */

  if (time2 > 4 && time1 % 3 == 0)
    ret *= 3;

  return ret;
}

/**
 * This function goes through a measure and properly sets
 * durinticks and starttickofnextnote values for everything in
 * that measure, tuplets or no.
 *
 * It works out that this function can be called wherever
 * calculatebeamsandstemdirs is invoked, and would share code besides, so
 * that's precisely where it is invoked 
 */
void
settickvalsinmeasure (objnode * theobjs, gint ticksperbeat)
{
  gint numerator = 1, denominator = 1;
  objnode *curobjnode;
  DenemoObject *theobj;
  gint ticks_so_far = 0;
  gint basic_ticks_in_tuplet_group = 0;
  gint basic_ticks_in_grace_group = 0;
  gboolean in_tuplet = FALSE;
  gboolean in_grace = FALSE;

  for (curobjnode = theobjs; curobjnode; curobjnode = curobjnode->next)
    {
		theobj = (DenemoObject *) curobjnode->data;
		theobj->starttick =
		ticks_so_far + (basic_ticks_in_tuplet_group * numerator
						/ denominator) + basic_ticks_in_grace_group;
		
		if (theobj->type == CHORD)
		{
			if (in_tuplet)
			{
				set_tuplefied_numticks (theobj, numerator, denominator);
				basic_ticks_in_tuplet_group += theobj->basic_durinticks;
			}
			else if (in_grace)
			{
				set_grace_numticks (theobj, 8);
				basic_ticks_in_grace_group += theobj->basic_durinticks;
			}
			else
			{
				set_tuplefied_numticks (theobj, 1, 1);
				set_grace_numticks (theobj, 1);
				ticks_so_far += theobj->durinticks;
			}
		}
		else if (theobj->type == TUPOPEN)
		{
			in_tuplet = TRUE;
			numerator = ((tupopen *) theobj->object)->numerator;
			denominator = ((tupopen *) theobj->object)->denominator;
			basic_ticks_in_tuplet_group = 0;	/* Probably gratuitous */
		}
		else if (theobj->type == TUPCLOSE)
		{
			in_tuplet = FALSE;
			ticks_so_far += ((basic_ticks_in_tuplet_group * numerator)
			/ denominator);
			numerator = 1;
			denominator = 1;
			basic_ticks_in_tuplet_group = 0;
		}
		else if (theobj->type == GRACE_START)
		{
			in_grace = TRUE;
			basic_ticks_in_grace_group = 0;
		}
		else if (theobj->type == GRACE_END)
		{
			in_grace = FALSE;
			/*      ticks_so_far += basic_ticks_in_grace_group;  RTS - this is just a mistake I think */
			basic_ticks_in_grace_group = 0;
		}
		
		theobj->starttickofnextnote =
		ticks_so_far + (basic_ticks_in_tuplet_group * numerator
						/ denominator) + basic_ticks_in_grace_group;
    }
}

/**
 * This function simply sets stem directions. It probably deals with
 * staves that have a fixed stem direction inefficiently, but this was
 * the easiest way to add things to the existing code. 
 */
static void
setsdir (objnode * starter, objnode * ender, gint beamgroup_sum,
	 gint beamgroup_number, gint beamgroup_highest,
	 gint beamgroup_lowest, gint clef, gint stem_directive)
{
  objnode *curobjnode;
  DenemoObject *theobj;
  gint avgoffset = beamgroup_number ? beamgroup_sum / beamgroup_number : 0;
  gint avgheight = calculateheight (avgoffset, clef);
  gboolean is_stemup = TRUE;
  gint stemoffset;
  gint stemy;

  switch (stem_directive)
    {
    case DENEMO_STEMUP:
      is_stemup = TRUE;
      break;
    case DENEMO_STEMBOTH:
      is_stemup = avgheight > MID_STAFF_HEIGHT;
      break;
    case DENEMO_STEMDOWN:
      is_stemup = FALSE;
      break;
    }
  theobj = (DenemoObject *) starter->data;
  if (is_stemup)
    stemoffset = MAX (beamgroup_lowest + 7, beamgroup_highest + 5);
  else
    stemoffset = MIN (beamgroup_highest - 7, beamgroup_lowest - 5);
  stemy = calculateheight (stemoffset, clef);

  /* Okay; now that we've got everything calculated, just roll through
   * the measure and set stem heights. */

  for (curobjnode = starter; curobjnode != ender->next;
       curobjnode = curobjnode->next)
    {
      theobj = (DenemoObject *) curobjnode->data;
      if (theobj->type == CHORD)
	{
	  if (((chord *) theobj->object)->baseduration == 0)
	    /* Whole notes are always laid out stemup */
	    ((chord *) theobj->object)->is_stemup = TRUE;
	  else
	    {
	      ((chord *) theobj->object)->is_stemup = is_stemup;
	      ((chord *) theobj->object)->stemy = stemy;
	    }
	  findreversealigns (theobj);
	}
    }
}

/**
 * This function takes all these int *s in so that appropriate values
 * can be fed into the function when it's called again for the
 * next measure -- see beamsandstemdirswholestaff for details
  */
void
calculatebeamsandstemdirs (objnode * theobjs, gint * pclef, gint * time1,
			   gint * time2, gint * stem_directive)
{
  DenemoObject *prevobj = NULL, *theobj;
  objnode *curobjnode, *starter = NULL;
  chord chordval;
  gint beatendsat, ticksperbeat;
  gint beamgroup_sum = 0;	/* Sum of mid_c_offsets in the beamgroup */
  gint beamgroup_number = 0;
  gint beamgroup_highest = 0;
  gint beamgroup_lowest = 0;
  gint next_clef = *pclef;	/* Useful for when a clef intrudes
				   mid-beamgroup */
  gint next_stem_directive = *stem_directive;
  gboolean isrest;

  /* Check to see if first item is a time signature indiactor */
  curobjnode = theobjs;
  if (curobjnode)
    {
      theobj = (DenemoObject *) curobjnode->data;

      if (theobj->type == TIMESIG)
	{
	  *time1 = ((timesig *) theobj->object)->time1;
	  *time2 = ((timesig *) theobj->object)->time2;
	}
    }

  ticksperbeat = calcticksperbeat (*time1, *time2);
  settickvalsinmeasure (theobjs, ticksperbeat);
  beatendsat = ticksperbeat;

  for (; curobjnode; prevobj = theobj, curobjnode = curobjnode->next)
    {
      theobj = (DenemoObject *) curobjnode->data;
      isrest = (theobj->type == CHORD) && !((chord *) theobj->object)->notes;
      if (theobj->type != CHORD || isrest)
	{
	  /* A non-chord or rest always breaks up a beam group */
	  /* I may want to adjust rest behavior to something else at some point */
	  theobj->isstart_beamgroup = TRUE;
	  theobj->isend_beamgroup = TRUE;

	  switch (theobj->type)
	    {
	    case CLEF:
	      next_clef = ((clef *) theobj->object)->type;
	      break;
	    case STEMDIRECTIVE:
	      next_stem_directive = ((stemdirective *) theobj->object)->type;
	      break;
	    default:
	      break;
	    }
	}
      else
	{
	  /* Determine whether this is the start or end of another
	   * beam group.  Quarter notes or longer automatically are. */
	  if (((chord *) theobj->object)->baseduration <= 2)
	    theobj->isstart_beamgroup = theobj->isend_beamgroup = TRUE;
	  else			/* otherwise... */
	    {
	      if (prevobj)
		theobj->isstart_beamgroup = prevobj->isend_beamgroup;
	      else
		theobj->isstart_beamgroup = TRUE;

	      /* Does this note occupy a beat boundary. i.e., it's dotted,
	       * syncopated, whatever? If so, then it's its own beamgroup. */
	      if (theobj->starttickofnextnote > beatendsat)
		theobj->isstart_beamgroup = theobj->isend_beamgroup = TRUE;
	      /* Does it end exactly on the beat? Then it's the end of
	       * the beamgroup */
	      else if (theobj->starttickofnextnote == beatendsat)
		theobj->isend_beamgroup = TRUE;
	      /* Is it the last note in the measure */
	      else if (!curobjnode->next)
		theobj->isend_beamgroup = TRUE;
	      /* Okay. So it's not the end of the beamgroup */
	      else
		theobj->isend_beamgroup = FALSE;
	    }			/* End inner else */
	}			/* End outer else */

      /* Update beatendsat to reflect the bit of music that's just
       * been tacked on */

      while (theobj->starttickofnextnote >= beatendsat)
	beatendsat += ticksperbeat;

      /* Backtrack a little bit -- we may not have known that prevobj
       * was the end of the preceding beamgroup until just now. If it
       * is, set the stem direction and such for the preceding
       * beamgroup. */
      if (prevobj && !prevobj->isend_beamgroup && theobj->isstart_beamgroup)
	{
	  prevobj->isend_beamgroup = TRUE;
	  setsdir (starter, curobjnode->prev, beamgroup_sum, beamgroup_number,
		   beamgroup_highest, beamgroup_lowest, *pclef,
		   *stem_directive);
	}

      /* Now that we've determined this note's status, what to actually
       * do about it: */

      *pclef = next_clef;
      *stem_directive = next_stem_directive;

      if (theobj->isstart_beamgroup)
	{
	  starter = curobjnode;
	  beamgroup_sum = beamgroup_number = 0;
	  beamgroup_highest = G_MININT;
	  beamgroup_lowest = G_MAXINT;
	}
      if (theobj->type == CHORD)
	{
	  chordval = *(chord *) theobj->object;
	  beamgroup_sum += chordval.sum_mid_c_offset;
	  beamgroup_number += g_list_length (chordval.notes);
	  beamgroup_highest = MAX (beamgroup_highest, chordval.highestpitch);
	  beamgroup_lowest = MIN (beamgroup_lowest, chordval.lowestpitch);
	}
      if (theobj->isend_beamgroup)
	{
	  setsdir (starter, curobjnode, beamgroup_sum, beamgroup_number,
		   beamgroup_highest, beamgroup_lowest, *pclef,
		   *stem_directive);
	}
    }				/* End object loop */
}				/* End function */





#define ACCS_TOO_CLOSE 5
/**
 * This function offsets accidentals that are near to each
 * other on the same chord.  
 */
void
set_accidental_positions (DenemoObject * the_chord)
{
  GList *current;
  note *current_note;
  gint columns[ACCS_TOO_CLOSE];
  gint column_widths[ACCS_TOO_CLOSE];
  gint column_positions[ACCS_TOO_CLOSE];
  gint i;
  chord chordval = *(chord *) the_chord->object;
  gint additional_space
    = ((!chordval.is_stemup && chordval.is_reversealigned)
       ? headwidths[MIN (chordval.baseduration, 2)] : 0);


  for (i = 0; i < ACCS_TOO_CLOSE; i++)
    {
      columns[i] = G_MAXINT;
      column_widths[i] = 0;
    }

  /* First pass through notes: assign accidentals to numerical
     columns: 0 -> closest to noteheads, ACCS_TOO_CLOSE - 1 ->
     furthest away.  Store this value in position_of_accidental, though
     it will be replaced fairly quickly.  */

  for (current = g_list_last (chordval.notes);
       current; current = current->prev)
    {
      current_note = (note *) current->data;
      if (current_note->showaccidental)
	{
	  for (i = 0;
	       columns[i] < current_note->mid_c_offset + ACCS_TOO_CLOSE; i++)
	    ;
	  current_note->position_of_accidental = i;
	  columns[i] = current_note->mid_c_offset;
	  column_widths[i] = MAX (column_widths[i],
				  accwidths[current_note->enshift + 2]);
	}
    }

  /* Second pass: go through the notes again and replace
     position_of_accidental with a more useful value.  */

  column_positions[0] = (column_widths[0] + additional_space + EXTRABACKOFF);
  for (i = 1; i < ACCS_TOO_CLOSE; i++)
    column_positions[i] = (column_positions[i - 1] + column_widths[i]
			   + EXTRABACKOFF);
  for (current = chordval.notes; current; current = current->next)
    {
      current_note = (note *) current->data;
      if (current_note->showaccidental)
	current_note->position_of_accidental
	  = column_positions[current_note->position_of_accidental];
    }
}


#define UNSET -3
#define CONTRADICTED 3

/**
 * Calculate which accidentials should be shown
 *
 */
gint
showwhichaccidentals (objnode * theobjs, gint initialnum, gint * initialaccs)
{
  gint whatpersisted[7];
  static gint initialaccsthischord[7]
    = { UNSET, UNSET, UNSET, UNSET, UNSET, UNSET, UNSET };
  gint accsthischord[7];
  gboolean freshthischord[7];
  gboolean contradicted[7];
  gint otn;			/* offsettonumber */
  objnode *curobjnode;
  DenemoObject *theobj;
  GList *curtone;
  note *thetone;
  gint ret[7];
  gint retnum = initialnum;
  gint i;

  memcpy (ret, initialaccs, SEVENGINTS);
  memcpy (whatpersisted, initialaccs, SEVENGINTS);
  for (curobjnode = theobjs; curobjnode; curobjnode = curobjnode->next)
    {
      theobj = (DenemoObject *) curobjnode->data;
      if (theobj->type == CHORD)
	{
	  ((chord *) theobj->object)->hasanacc = FALSE;
	  memcpy (accsthischord, initialaccsthischord, SEVENGINTS);
	  memset (freshthischord, 0, SEVENGINTS);
	  memset (contradicted, 0, SEVENGINTS);
	  /* First loop through chord - looks for conflicting values
	   * for the same note */
	  for (curtone = ((chord *) theobj->object)->notes; curtone;
	       curtone = curtone->next)
	    {
	      thetone = (note *) curtone->data;
	      otn = offsettonumber (thetone->mid_c_offset);
	      if (thetone->enshift != whatpersisted[otn])
		{
		  freshthischord[otn] = TRUE;
		  whatpersisted[otn] = thetone->enshift;
		}
	      if (accsthischord[otn] == UNSET)
		accsthischord[otn] = thetone->enshift;
	      else if (accsthischord[otn] != thetone->enshift)
		{
		  contradicted[otn] = TRUE;
		  whatpersisted[otn] = CONTRADICTED;
		}
	    }			/* End first loop through chord */
	  /* Now loop through the chord again, setting note->showaccidental
	   * appropriately */
	  for (curtone = ((chord *) theobj->object)->notes; curtone;
	       curtone = curtone->next)
	    {
	      thetone = (note *) curtone->data;
	      otn = offsettonumber (thetone->mid_c_offset);
	      if (contradicted[otn])
		/* We've got conflicting accidentals for the same pitch */
		thetone->showaccidental =
		  ((chord *) theobj->object)->hasanacc = TRUE;
	      else if (freshthischord[otn])
		/* A new accidental not present in the original chord */
		thetone->showaccidental =
		  ((chord *) theobj->object)->hasanacc = TRUE;
	      else if (thetone->directive && (*thetone->directive->str=='!'|| *thetone->directive->str=='?'))
		thetone->showaccidental =
		  ((chord *) theobj->object)->hasanacc = TRUE;
	      else
		thetone->showaccidental = FALSE;
	    }			/* End second loop through chord */
	  set_accidental_positions (theobj);
	  setpixelmin (theobj);
	}			/* End if chord */
      else if (theobj->type == KEYSIG)
	{
	  for (i = 0; i < 7; i++)
	    initialaccsthischord[i] = UNSET;
	  memcpy (ret, ((keysig *) theobj->object)->accs, SEVENGINTS);
	  memcpy (whatpersisted, ret, SEVENGINTS);
	  theobj->minpixelsalloted =
	    draw_key (NULL, NULL, 0, 0, ((keysig *) theobj->object)->number,
		      retnum, 0, FALSE);
	  retnum = ((keysig *) theobj->object)->number;
	}


    }				/* End object loop */
  memcpy (initialaccs, ret, SEVENGINTS);
  return retnum;
}

/**
 *  Force and accidental to be shown on the score.
 * 
 * @param theobj the DenemoObject to force the accidential on
 */
void
forceaccidentals (DenemoObject * theobj)
{
  GList *curtone;
  note *thetone;

  for (curtone = ((chord *) theobj->object)->notes; curtone;
       curtone = curtone->next)
    {
      thetone = (note *) curtone->data;
      thetone->showaccidental = TRUE;
    }
  ((chord *) theobj->object)->hasanacc = TRUE;
  set_accidental_positions (theobj);
  setpixelmin (theobj);
  if (theobj->user_string)
    {
      g_free (theobj->user_string);
      theobj->user_string = NULL;
    }
}
