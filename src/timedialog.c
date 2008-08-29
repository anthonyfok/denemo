/* timesigdialog.cpp
 * a callback that creates a dialog boxes prompting the
 * user for information on changing the time signature
 
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 2000-2005 Matthew Hiller */

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "calculatepositions.h"
#include "commandfuncs.h"
#include "contexts.h"
#include <denemo/denemo.h>
#include "dialogs.h"
#include "draw.h"
#include "measureops.h"
#include "objops.h"
#include "staffops.h"
#include "utils.h"

/**
 * Is the integer a power of 2
 *
 */
gint
ispow2 (gint x)
{
  for (; !(x & 1); x >>= 1)
    ;	/* Go through all the low order bits that are equal to 0 */
  return (x == 1);
}


/**
 * Set the timesig values for the given staff
 * 
 */
static void
settimesig (DenemoStaff * curstaffstruct, gint time1, gint time2)
{
  curstaffstruct->stime1 = time1;
  curstaffstruct->stime2 = time2;
  beamsandstemdirswholestaff (curstaffstruct);
}

/**
 * Set the initial timesig on current staff or across entire score
 * @param si pointer to the DenemoScore structure.
 * @param curstaffstruct the staff to set the timesig for.
 * @param time1 the time signature nominator
 * @param time2 the time signature denominator
 * @param all_staves apply the new time signature to all staves
 *
 * @return None
 */
void
dnm_setinitialtimesig (DenemoScore * si, DenemoStaff * curstaffstruct, 
		       gint time1, gint time2, gboolean all_staves)
{
  staffnode *curstaff;

  if (time1 && time2 && ispow2 (time2))
    {
      if (all_staves)
	{
	  for (curstaff = si->thescore; curstaff; curstaff = curstaff->next)
	    {
	      curstaffstruct = (DenemoStaff *) curstaff->data;
	      settimesig (curstaffstruct, time1, time2);
	    }
	  find_leftmost_allcontexts (si);
	}
      else
	{
	  settimesig (curstaffstruct, time1, time2);
	  find_leftmost_staffcontext (curstaffstruct, si);
	}

    }
}


/**
 * Insert time sig change on either a single staff or across the 
 * entire score.
 */
static void
insert_timesig (DenemoScore * si, DenemoStaff * curstaffstruct, gint time1,
		gint time2)
{
  staffnode *curstaff = NULL;
  measurenode *curmeasure = NULL;
  objnode *firstobj = NULL;
  DenemoObject *firstmudobj = NULL;
  gboolean replacing = FALSE;	/* if we don't use this trick, anomalous
				 * stuff can happen when replacing a time
				 * signature */

  for (curstaff = si->thescore; curstaff; curstaff = curstaff->next)
    {
      if (((DenemoStaff *) curstaff->data)->is_parasite)
	continue;

      curmeasure = g_list_nth (firstmeasurenode (curstaff),
			       si->currentmeasurenum - 1);
      /* First, look to see if there already is a time signature change at
         the beginning of this measure. If so, delete it first. */
      if(!curmeasure)
	continue;
      firstobj = firstobjnode (curmeasure);
      if (firstobj)
	firstmudobj = (DenemoObject *) firstobj->data;
      else
	firstmudobj = NULL;
      if (firstmudobj && firstmudobj->type == TIMESIG)
	{
	  replacing = TRUE;
	  curmeasure->data =
	    g_list_remove_link ((objnode *) curmeasure->data, firstobj);
	  freeobject (firstmudobj);
	  g_list_free_1 (firstobj);
	}
      curmeasure->data = g_list_prepend ((objnode *) curmeasure->data,
					 dnm_newtimesigobj (time1, time2));
      if (curmeasure == si->currentmeasure)
	{
	  if (!replacing)
	    si->cursor_x++;
	  if (si->cursor_appending)
	    si->currentobject = g_list_last ((objnode *) curmeasure->data);
	  else
	    si->currentobject =
	      g_list_nth ((objnode *) curmeasure->data, si->cursor_x);
	}
      beamsandstemdirswholestaff ((DenemoStaff *) curstaff->data);
    }

}

/**
 * Callback to insert a time sig change
 * Calls timesig_change with the INSERT argument
 */
void
timesig_change_insert (GtkAction *action, gpointer param)
{
  DenemoGUI *gui = Denemo.gui;
  timesig_change (gui, INSERT);
}

/**
 * Callback to change the initial  time sig change
 * Calls timesig_change with the CHANGEINITIAL argument
 */
void
timesig_change_initial (GtkAction *action, gpointer param)
{
  DenemoGUI *gui = Denemo.gui;
  timesig_change (gui, CHANGEINITIAL);
}

/**
 * Time sig change dialog.  allows the user to set
 * the time signature to insert or change
 */
void
timesig_change (DenemoGUI * gui, actiontype action)
{
  GtkWidget *dialog;
  GtkWidget *label;
  GtkWidget *textentry1;
  GtkWidget *textentry2;
  GtkWidget *checkbutton;

  DenemoStaff *curstaffstruct = (DenemoStaff *) gui->si->currentstaff->data;

  if (gui->si->lily_file && action == CHANGEINITIAL)
    return;	  /* no code for this yet - just edit textually */

  dialog = 
    gtk_dialog_new_with_buttons (((action == CHANGEINITIAL) ? 
				  _("Change initial time signature") : 
				  _("Insert time signature change")), 
				 NULL,	/* parent window */
				 (GtkDialogFlags) 
				 (GTK_DIALOG_MODAL |
				  GTK_DIALOG_DESTROY_WITH_PARENT),
				 GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
				 GTK_STOCK_CANCEL, GTK_STOCK_CANCEL,
				 NULL);

  gtk_container_set_border_width (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox),
				  12);
  
  label = gtk_label_new (_("Enter desired time signature:"));
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
		      label, FALSE, FALSE, 8);
  
  textentry1 = gtk_spin_button_new_with_range (1, 128, 1.0);
  gtk_spin_button_set_digits (GTK_SPIN_BUTTON (textentry1), 0);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (textentry1),
			     (gdouble) curstaffstruct->stime1);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), textentry1, FALSE,
		      FALSE, 8);
  gtk_entry_set_activates_default (GTK_ENTRY (textentry1), TRUE);

  textentry2 = gtk_spin_button_new_with_range (1, 16, 1.0);
  gtk_spin_button_set_digits (GTK_SPIN_BUTTON (textentry2), 0);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (textentry2),
			     (gdouble) curstaffstruct->stime2);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), textentry2, FALSE,
		      FALSE, 8);
  gtk_entry_set_activates_default (GTK_ENTRY (textentry2), TRUE);

  checkbutton = gtk_check_button_new_with_label (_("Apply to all staves"));
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
		      checkbutton, FALSE, FALSE, 8);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), TRUE);

  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_ACCEPT);
  gtk_widget_show_all (dialog);
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      gint time1 =
	gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (textentry1));
      gint time2 =
	gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (textentry2));
      gboolean all_staves =
	gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton));
      if (action == CHANGEINITIAL)
	{
	  dnm_setinitialtimesig (gui->si, curstaffstruct, time1, 
				 time2, all_staves);
	}
      else
	{
	  insert_timesig (gui->si, curstaffstruct, time1, time2);
	}
      displayhelper (gui);
    }
  score_status(gui, TRUE);
  gtk_widget_destroy (dialog);
}
