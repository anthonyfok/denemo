/* lilydirectives.cpp 
 * Implements lilydirectives which are not notes 
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * A Tee  (c) 2000-2005
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <denemo/denemo.h>
#include "lilydirectives.h"
#include "chordops.h"
#include "calculatepositions.h"
#include "commandfuncs.h"
#include "contexts.h"
#include "dialogs.h"
#include "draw.h"
#include "objops.h"
#include "staffops.h"
#include "utils.h"
#include "prefops.h"

static void edit_directive_callback(GtkWidget *w, gpointer what);


static void
gtk_menu_item_set_label_text(GtkMenuItem *item, gchar *text) {
  GtkWidget *label = gtk_bin_get_child(item);
  if(label)
    gtk_label_set_text(label, text);
}


/**
 * If the curObj is a chord with a note(s)
 * return the first note at or below cursory, or the last note 
 * else return NULL
 */
static note *
findnote(DenemoObject *curObj, gint cursory) {
  note *curnote = NULL;
  if (curObj && curObj->type == CHORD && ((chord *) curObj->object)->notes ) {
    GList *notes = ((chord *) curObj->object)->notes;
    for(;notes; notes = notes->next){
      curnote =  (note*)notes->data;
      //g_print("comparing %d and %d\n", cursory, curnote->y);
      if(cursory <= curnote->mid_c_offset)
	break;
   }

  }
     return curnote;
}

static void  toggle_locked(GtkWidget *widget, gboolean *locked) {
  //g_print("Called with %d\n", *locked);
  *locked = !*locked;
}

/* lookup a directive tagged with TAG in a list DIRECTIVES and return it.
   if TAG is NULL return the first directive
   else return NULL */
static DenemoDirective *find_directive(GList *directives, gchar *tag) {
  DenemoDirective *directive = NULL;
  if(tag) {
    GList *g;
    for(g=directives;g;g=g->next){
      directive = (DenemoDirective *)g->data;
      if(directive->tag && !strcmp(tag, directive->tag->str))
	return directive;
      directive = NULL;
    }
  } else
    directive = (DenemoDirective *)directives->data;
  return directive;
}


static gboolean 
delete_directive(GList **directives, gchar *tag) {
  DenemoDirective *directive = NULL;
  if(tag) {
    GList *g;
    for(g=*directives;g;g=g->next){
      directive = (DenemoDirective *)g->data;
      if(directive->tag && !strcmp(tag, directive->tag->str)){
	*directives = g_list_remove(*directives, directive);
	free_directive(directive);
	return TRUE;
      }
    }
  }
  return FALSE;
}

/* free a list of directives and set to NULL */
void
delete_directives(GList **directives) {
  DenemoDirective *directive = NULL; //FIXME use free_directives
  if(directives)
  while(*directives){
    directive = (DenemoDirective *)(*directives)->data;
    *directives = g_list_remove(*directives, directive);
    free_directive(directive);
  }
}



static DenemoDirective*
new_directive(gchar *tag){
  DenemoDirective *directive = (DenemoDirective*)g_malloc0(sizeof(DenemoDirective));
  if(tag)
    directive->tag = g_string_new(tag);
  return directive;
}


static  DenemoObject *findobj(void) {
  DenemoGUI *gui = Denemo.gui;
  DenemoScore * si = gui->si;
  return (DenemoObject *) si->currentobject ?
    (DenemoObject *) si->currentobject->data : NULL;
  }

typedef enum attach_type {ATTACH_NOTE, ATTACH_CHORD} attach_type;
/**
 * Denemo directive attach or edit.  
if interactive: Allows user to attach a lilypond directive 
else attache the passed strings as lilypond directive
attachment is to chord ( attach is ATTACH_CHORD) or to the note at the cursor
 */
static void
attach_directive (attach_type attach, gchar *postfix, gchar *prefix, gchar *display, gchar *tag, gboolean interactive)
{
  gchar *prefixstring=NULL, *postfixstring=NULL, *displaystring=NULL;
  DenemoGUI *gui = Denemo.gui;
  DenemoScore * si = gui->si;
  note *curnote = NULL;
  DenemoObject *curObj = findobj();
  if(curObj==NULL) {  
    if(interactive)
      warningdialog("You must put the cursor on a chord to attach LilyPond");//FIXME find a note and ask
    return;
  }
  chord *thechord = NULL;
  thechord = (chord *)curObj->object;
  if(curObj->type!=CHORD) {  
    if(interactive)
      warningdialog("You must put the cursor on a chord to attach LilyPond");
    return;
  }
    
  curnote = findnote(curObj, gui->si->cursor_y);
  if(attach==ATTACH_NOTE && (curnote==NULL)) {  
    if(interactive)
      warningdialog("You must put the cursor on a note to attach LilyPond to the note");//FIXME find a note and ask
    return;
  }
 
  // setup directive to be data from thechord->directives or curnote->directives which has matching tag, or first if tag is NULL.
  DenemoDirective *directive=NULL;
    switch(attach) {
    case  ATTACH_CHORD:
      if(thechord->directives==NULL) {
	directive = new_directive(tag);
	thechord->directives = g_list_append(NULL, directive);
      } else {
	directive = find_directive(thechord->directives, tag);
	if(directive == NULL) {
	  if(tag) {
	    directive = new_directive(tag);
	    thechord->directives = g_list_append(thechord->directives, directive);
	  }
	}
      }
      break;
     case  ATTACH_NOTE:
      if(curnote->directives==NULL) {
	directive = new_directive(tag);
	curnote->directives = g_list_append(NULL, directive);
      } else {
	directive = find_directive(curnote->directives, tag);
	if(directive == NULL) {
	  if(tag) {
	    directive = new_directive(tag);
	    curnote->directives = g_list_append(curnote->directives, directive);
	  }
	}
      }
      break;
    default:
      g_warning("Error in attach type");
      return;
    }

  if(interactive) {
      if(directive->postfix)
	postfixstring = directive->postfix->str;
      if(directive->prefix)
	prefixstring = directive->prefix->str;
      if(directive->display)
	displaystring = directive->display->str;

    prefixstring = string_dialog_entry(gui, "Attach LilyPond", "Give text to place before the note", prefixstring);
    postfixstring = string_dialog_entry(gui, curnote?"Attach LilyPond to Note":"Attach LilyPond to Chord", curnote?"Give LilyPond text to postfix to note of chord":"Give LilyPond text to postfix to chord", postfixstring);
    displaystring =  string_dialog_entry(gui, "Attach LilyPond", "Give Display text if required", displaystring);
  } else {//not interactive
    if(prefix)
      prefixstring = g_strdup(prefix);
    if(postfix)
      postfixstring = g_strdup(postfix);
    if(display)
      displaystring = g_strdup(display);
  }

#define STRINGASSIGN(field, val) \
     if(val && *val) {\
     if(directive->field) g_string_assign(directive->field, val);\
     else directive->field=g_string_new(val);}
    STRINGASSIGN(postfix, postfixstring);
    STRINGASSIGN(prefix, prefixstring);
    STRINGASSIGN(display, displaystring);

#undef STRINGASSIGN

  score_status(gui, TRUE);
  displayhelper (gui);
  g_free(postfixstring);
  g_free(displaystring);
  g_free(prefixstring);
}

static void create_directives(GList **directives, gchar *tag) {
  *directives = g_list_append(NULL, new_directive(tag));
}
static void
get_lily_parameter(gchar *query, DenemoScriptParam *param) {
  DenemoObject *curObj = (DenemoObject *) Denemo.gui->si->currentobject ?
    (DenemoObject *) Denemo.gui->si->currentobject->data : NULL;
  param->status = curObj && curObj->type==LILYDIRECTIVE;
#define ASSIGN_PARAM(field)  if(!strcmp(#field, query))\
  g_string_assign(param->string, lilyobj->field->str);
  if(param->status)
    {
      lilydirective *lilyobj = (lilydirective *) curObj->object;
      ASSIGN_PARAM(postfix);
      ASSIGN_PARAM(display);
      if(!strcmp("minpixels", query))
	g_string_printf(param->string, "%d", curObj->minpixelsalloted);
    }
#undef ASSIGN_PARAM
}


static
void insert_lily_directive(gchar *postfix, gchar *display, gboolean locked, gint minpixels) {
  DenemoGUI *gui = Denemo.gui;
  DenemoScore *si = gui->si;
  DenemoObject *lily;
  lilydirective *lilyobj=NULL; /* a lily directive object */
  DenemoObject *curObj = (DenemoObject *) si->currentobject ?
    (DenemoObject *) si->currentobject->data : NULL;
  if(postfix==NULL)
    postfix="";
  gboolean is_new = FALSE;
    if (curObj && curObj->type == LILYDIRECTIVE) {
      g_string_assign((lilyobj=(lilydirective *) curObj->object)->postfix, postfix);
      curObj->minpixelsalloted = minpixels;
    }  else {  
      lily = lily_directive_new (postfix);
      is_new= TRUE;
      lilyobj = (lilydirective *) lily->object;
      lily->minpixelsalloted = minpixels;// g_print("min pixels %d\n", lily->minpixelsalloted);
    }
    if(lilyobj) {
      lilyobj->locked = locked;
      if(*postfix=='%') {//append newline if directive starts with a LilyPond comment indicator
	g_string_append(lilyobj->postfix,"\n");
      }
      if(display) {
	if(lilyobj->display)
	  g_string_assign(lilyobj->display, display);
	else
	  lilyobj->display = g_string_new(display);
      }
    }
    if(is_new)
      object_insert (gui, lily);
    score_status(gui, TRUE);
    displayhelper(gui);
}


/* Run a dialog to get a lily directive from the user
 the values returned must be freed by the caller */
static
gboolean get_lily_directive(gchar **directive, gchar **display, gboolean *locked) {
  DenemoGUI *gui = Denemo.gui;
  GtkToggleButton *button = NULL;
  button = (GtkToggleButton *)gtk_check_button_new_with_label("locked");
  g_signal_connect(button, "toggled",  G_CALLBACK (toggle_locked), locked);
  if(*locked)
    gtk_toggle_button_set_active (button, *locked), *locked=TRUE;//FIXME how is this supposed to be done?
  *directive = string_dialog_entry_with_widget(gui, "Insert LilyPond", "Give LilyPond text to insert", *directive, GTK_WIDGET(button));
  if(!*directive)
    return FALSE;
  *display =  string_dialog_entry(gui, "Insert LilyPond", "Give Display text if required", *display);
  return TRUE;
}

/* return the directive tagged tag if present at cursor postion
 if tag is NULL, return any directive at current position*/
static
DenemoDirective *get_standalone_directive(gchar *tag){
    DenemoObject *curObj = (DenemoObject *) Denemo.gui->si->currentobject ?
      (DenemoObject *)  Denemo.gui->si->currentobject->data : NULL;
    if (curObj && curObj->type == LILYDIRECTIVE) {
      DenemoDirective *ret = (DenemoDirective *)curObj->object;
      if(tag==NULL)
	return ret;
      if(ret && ret->tag && strcmp(tag, ret->tag->str))
	 ret = NULL;	
      return ret;
    }
    return NULL;
}
/**
 * Denemo directive insert/Edit.  Allows user to insert a Denemo directive 
 * before the current cursor position
 * or edit the current Denemo directive
 */
void
standalone_directive (GtkAction *action, DenemoScriptParam * param)
{
  DenemoGUI *gui = Denemo.gui;
  GET_4PARAMS(action, param, directive, postfix, display, minpixels);
  //g_print("query is %s\n", query);
  if(directive)
    postfix = directive;//support for simpler syntax directive=xxx instead of postfix=xxx
  if(query) {
    get_lily_parameter(*query?query:"postfix", param);
    return;
  }
  gboolean locked = FALSE;
  if(postfix && !display)
    display = postfix;
  if(action) {
    DenemoObject *curObj = (DenemoObject *) gui->si->currentobject ?
      (DenemoObject *) gui->si->currentobject->data : NULL;
    if (curObj && curObj->type == LILYDIRECTIVE) {
      lilydirective *lilyobj=(lilydirective *) curObj->object;
      postfix = lilyobj->postfix? lilyobj->postfix->str:NULL;
      display = lilyobj->display? lilyobj->display->str:NULL;
    }
    if(get_lily_directive(&postfix, &display, &locked))
      insert_lily_directive(postfix, display, locked, 8);
  } else {
    insert_lily_directive(postfix, display, locked, minpixels?atoi(minpixels):8);
  }
}
/**
 * callback for AttachLilyToNote (command name is historical)
 * note_directive DenemoDirective attach to chord.  The DenemoDirective is tagged with TAG
 * and attached the note below cursor in current chord
 * if one tagged with TAG already exists, it edits it with the passed values. 
 * Only postfix, prefix and display can be set with this call (for backward compatibility)
 */
void
note_directive (GtkAction *action, DenemoScriptParam *param)
{
  DenemoGUI *gui = Denemo.gui;
  GET_4PARAMS(action, param, postfix, display, prefix, tag);
  attach_directive (ATTACH_NOTE, postfix, prefix, display, tag, action!=NULL);
}
/**
 * callback for AttachLilyToChord (command name is historical)
 * chord_directive DenemoDirective attach to chord.  The DenemoDirective is tagged with TAG
 * and attached the current chord
 * if one tagged with TAG already exists, it edits it with the passed values. 
 * Only postfix, prefix and display can be set with this call (for backward compatibility)
 */
void
chord_directive (GtkAction *action, DenemoScriptParam *param)
{
  DenemoGUI *gui = Denemo.gui;
  GET_4PARAMS(action, param, postfix, display, prefix, tag);
  attach_directive (ATTACH_CHORD, postfix, prefix, display, tag, action!=NULL);
}



static DenemoObject *get_chordobject(void) {
  chord *thechord = NULL;
  DenemoObject *curObj = findobj();
  if(curObj==NULL)
    return NULL;
  thechord = (chord *)curObj->object;
  if(curObj->type!=CHORD) {  
    return NULL;
  }
  return curObj;
}
static chord *get_chord(void) {
  chord *thechord = NULL;
  DenemoObject *curObj = get_chordobject();
  if(curObj==NULL)
    return NULL;
  return (chord *)curObj->object;
}

static DenemoLilyControl *get_score(void) {
  return &Denemo.gui->lilycontrol;
}


static note *get_note(void) {
  DenemoGUI *gui = Denemo.gui;
  note *curnote = NULL;
  chord *thechord = NULL;
  DenemoObject *curObj = get_chordobject();
  if(curObj==NULL)
    return NULL;
  return findnote(curObj, gui->si->cursor_y);
}


static DenemoStaff *get_staff(void) {
  if(Denemo.gui->si->currentstaff==NULL)
    return NULL;
  return Denemo.gui->si->currentstaff->data;
}
#define get_voice get_staff


//block for new type of directive
static  clef *get_clef(void) {
  clef *ret = NULL;
  DenemoObject *curObj = findobj();  
  if(curObj && curObj->type==CLEF){
    ret = ((clef*)curObj->object);
  } else {
    DenemoStaff *curstaff = get_staff();
    if(curstaff)
      ret = &curstaff->clef;
  }
  return ret;
}
static
DenemoDirective *get_clef_directive(gchar *tag) {
  clef *curclef = get_clef();
  if(curclef==NULL || (curclef->directives==NULL))
    return NULL;
  return find_directive(curclef->directives, tag);
}
gboolean delete_clef_directive(gchar *tag) {
  clef *curclef = get_clef();
  if(curclef==NULL || (curclef->directives==NULL))
    return FALSE;
  DenemoDirective *directive = get_clef_directive(tag);
  if(directive==NULL)
    return FALSE;
  return delete_directive(&curclef->directives, tag);
}
// end of block for new type of directive


static  keysig *get_keysig(void) {
  keysig *ret = NULL;
  DenemoObject *curObj = findobj();  
  if(curObj && curObj->type==KEYSIG){
    ret = ((keysig*)curObj->object);
  } else {
    DenemoStaff *curstaff = get_staff();
    if(curstaff)
      ret = &curstaff->keysig;
  }
  return ret;
}
static
DenemoDirective *get_keysig_directive(gchar *tag) {
  keysig *curkeysig = get_keysig();
  if(curkeysig==NULL || (curkeysig->directives==NULL))
    return NULL;
  return find_directive(curkeysig->directives, tag);
}
gboolean delete_keysig_directive(gchar *tag) {
  keysig *curkeysig = get_keysig();
  if(curkeysig==NULL || (curkeysig->directives==NULL))
    return FALSE;
  DenemoDirective *directive = get_keysig_directive(tag);
  if(directive==NULL)
    return FALSE;
  return delete_directive(&curkeysig->directives, tag);
}

static  timesig *get_timesig(void) {
  timesig *ret = NULL;
  DenemoObject *curObj = findobj();  
  if(curObj && curObj->type==TIMESIG){
    ret = ((timesig*)curObj->object);
  } else {
    DenemoStaff *curstaff = get_staff();
    if(curstaff)
      ret = &curstaff->timesig;
  }
  return ret;
}
static
DenemoDirective *get_timesig_directive(gchar *tag) {
  timesig *curtimesig = get_timesig();
  if(curtimesig==NULL || (curtimesig->directives==NULL))
    return NULL;
  return find_directive(curtimesig->directives, tag);
}
gboolean delete_timesig_directive(gchar *tag) {
  timesig *curtimesig = get_timesig();
  if(curtimesig==NULL || (curtimesig->directives==NULL))
    return FALSE;
  DenemoDirective *directive = get_timesig_directive(tag);
  if(directive==NULL)
    return FALSE;
  return delete_directive(&curtimesig->directives, tag);
}


static  scoreheader *get_scoreheader(void) {
  return &Denemo.gui->scoreheader;
}
static
DenemoDirective *get_scoreheader_directive(gchar *tag) {
  scoreheader *curscoreheader = get_scoreheader();
  if(curscoreheader==NULL || (curscoreheader->directives==NULL))
    return NULL;
  return find_directive(curscoreheader->directives, tag);
}
gboolean delete_scoreheader_directive(gchar *tag) {
  scoreheader *curscoreheader = get_scoreheader();
  if(curscoreheader==NULL || (curscoreheader->directives==NULL))
    return FALSE;
  DenemoDirective *directive = get_scoreheader_directive(tag);
  if(directive==NULL)
    return FALSE;
  return delete_directive(&curscoreheader->directives, tag);
}


static  paper *get_paper(void) {
  return &Denemo.gui->paper;
}
static
DenemoDirective *get_paper_directive(gchar *tag) {
  paper *curpaper = get_paper();
  if(curpaper==NULL || (curpaper->directives==NULL))
    return NULL;
  return find_directive(curpaper->directives, tag);
}
gboolean delete_paper_directive(gchar *tag) {
  paper *curpaper = get_paper();
  if(curpaper==NULL || (curpaper->directives==NULL))
    return FALSE;
  DenemoDirective *directive = get_paper_directive(tag);
  if(directive==NULL)
    return FALSE;
  return delete_directive(&curpaper->directives, tag);
}

static  layout *get_layout(void) {
  return &Denemo.gui->si->layout;
}
static
DenemoDirective *get_layout_directive(gchar *tag) {
  layout *curlayout = get_layout();
  if(curlayout==NULL || (curlayout->directives==NULL))
    return NULL;
  return find_directive(curlayout->directives, tag);
}
gboolean delete_layout_directive(gchar *tag) {
  layout *curlayout = get_layout();
  if(curlayout==NULL || (curlayout->directives==NULL))
    return FALSE;
  DenemoDirective *directive = get_layout_directive(tag);
  if(directive==NULL)
    return FALSE;
  return delete_directive(&curlayout->directives, tag);
}


static  movementcontrol *get_movementcontrol(void) {
  return &Denemo.gui->si->movementcontrol;
}
static
DenemoDirective *get_movementcontrol_directive(gchar *tag) {
  movementcontrol *curmovementcontrol = get_movementcontrol();
  if(curmovementcontrol==NULL || (curmovementcontrol->directives==NULL))
    return NULL;
  return find_directive(curmovementcontrol->directives, tag);
}
gboolean delete_movementcontrol_directive(gchar *tag) {
  movementcontrol *curmovementcontrol = get_movementcontrol();
  if(curmovementcontrol==NULL || (curmovementcontrol->directives==NULL))
    return FALSE;
  DenemoDirective *directive = get_movementcontrol_directive(tag);
  if(directive==NULL)
    return FALSE;
  return delete_directive(&curmovementcontrol->directives, tag);
}


static  header *get_header(void) {
  return &Denemo.gui->si->header;
}
static
DenemoDirective *get_header_directive(gchar *tag) {
  header *curheader = get_header();
  if(curheader==NULL || (curheader->directives==NULL))
    return NULL;
  return find_directive(curheader->directives, tag);
}
gboolean delete_header_directive(gchar *tag) {
  header *curheader = get_header();
  if(curheader==NULL || (curheader->directives==NULL))
    return FALSE;
  DenemoDirective *directive = get_header_directive(tag);
  if(directive==NULL)
    return FALSE;
  return delete_directive(&curheader->directives, tag);
}


static
DenemoDirective *get_note_directive(gchar *tag) {
  note *curnote = get_note();
  if(curnote==NULL || (curnote->directives==NULL))
    return NULL;
  return find_directive(curnote->directives, tag);
}

static
DenemoDirective *get_chord_directive(gchar *tag) {
DenemoObject *curObj = get_chordobject();
  if(curObj==NULL)
    return NULL;
  chord *thechord = (chord *)curObj->object;
  if(thechord->directives==NULL)
    return NULL;
  return find_directive(thechord->directives, tag);
}

static
DenemoDirective *get_score_directive(gchar *tag) {

  return find_directive(Denemo.gui->lilycontrol.directives, tag);
}

static
DenemoDirective *get_staff_directive(gchar *tag) {
  if(Denemo.gui->si->currentstaff==NULL)
    return NULL;
  DenemoStaff *curstaff = Denemo.gui->si->currentstaff->data;
  //FIXME return NULL if not primary staff
  if(curstaff==NULL || curstaff->staff_directives==NULL)
    return NULL;
  return find_directive(curstaff->staff_directives, tag);
}
static
DenemoDirective *get_voice_directive(gchar *tag) {
  if(Denemo.gui->si->currentstaff==NULL)
    return NULL;
  DenemoStaff *curstaff = Denemo.gui->si->currentstaff->data;
  if(curstaff==NULL || curstaff->voice_directives==NULL)
    return NULL;
  return find_directive(curstaff->voice_directives, tag);
}

gboolean
delete_staff_directive(gchar *tag) {
  if(Denemo.gui->si->currentstaff==NULL)
    return;
  DenemoStaff *curstaff = Denemo.gui->si->currentstaff->data;
  if(curstaff==NULL || curstaff->staff_directives==NULL)
    return;
  return delete_directive(&curstaff->staff_directives, tag);
}


gboolean
delete_initialclef_directive(gchar *tag) {
  if(Denemo.gui->si->currentstaff==NULL)
    return;
  DenemoStaff *curstaff = Denemo.gui->si->currentstaff->data;
  if(curstaff==NULL || curstaff->clef.directives==NULL)
    return;
  return delete_directive(&curstaff->clef.directives, tag);
}


gboolean
delete_voice_directive(gchar *tag) {
  if(Denemo.gui->si->currentstaff==NULL)
    return FALSE;
  DenemoStaff *curstaff = Denemo.gui->si->currentstaff->data;
  if(curstaff==NULL || curstaff->voice_directives==NULL)
    return FALSE;
  return delete_directive(&curstaff->voice_directives, tag);
}

gboolean delete_note_directive(gchar *tag) {
  note *curnote = get_note();
  if(curnote==NULL || (curnote->directives==NULL))
    return FALSE;
  DenemoDirective *directive = get_note_directive(tag);
  if(directive==NULL)
    return FALSE;
  return delete_directive(&curnote->directives, tag);
}
gboolean delete_chord_directive(gchar *tag) {
DenemoObject *curObj = get_chordobject();
  if(curObj==NULL)
    return FALSE;
  chord *thechord = (chord *)curObj->object;
  if(thechord->directives==NULL)
    return FALSE;
  DenemoDirective *directive = get_chord_directive(tag);
  if(directive==NULL)
    return FALSE;
  return delete_directive(&thechord->directives, tag);
}

gboolean delete_score_directive(gchar *tagname) {
  DenemoDirective *directive = get_score_directive(tagname);
  if(directive==NULL)
    return FALSE;
  return delete_directive(&Denemo.gui->lilycontrol.directives, tagname);
}

#define GET_STR_FIELD_FUNC(what, field)\
gchar *\
what##_directive_get_##field(gchar *tag) {\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive && directive->field)\
    return directive->field->str;\
  return NULL;\
}
//typdefs to make the macros defined below pick up the right structure for staff, voice and score as chord & note do
typedef DenemoStaff staff;
typedef DenemoStaff voice;
typedef DenemoLilyControl score;
#define PUT_STR_FIELD_FUNC_NAME(what, field, name)\
gboolean \
what##_directive_put_##field(gchar *tag, gchar *value) {\
  what *current = get_##what();\
  if(current==NULL) return FALSE;\
  if(current->name==NULL)\
       create_directives (&current->name, tag);\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive==NULL){\
    directive=new_directive(tag);\
    current->name = g_list_append(current->name, directive);\
    }\
  if(directive->field)\
    g_string_assign(directive->field, value);\
  else\
    directive->field = g_string_new(value);\
  return TRUE;\
}

#define PUT_STR_FIELD_FUNC(what, field) PUT_STR_FIELD_FUNC_NAME(what, field, directives)
#define PUT_STR_FIELD_FUNCS(what, field) PUT_STR_FIELD_FUNC_NAME(what, field, staff_directives)
#define PUT_STR_FIELD_FUNCV(what, field) PUT_STR_FIELD_FUNC_NAME(what, field, voice_directives)


GET_STR_FIELD_FUNC(score, midibytes)
GET_STR_FIELD_FUNC(movementcontrol, midibytes)
GET_STR_FIELD_FUNC(note, midibytes)
GET_STR_FIELD_FUNC(chord, midibytes)
GET_STR_FIELD_FUNC(staff, midibytes)
GET_STR_FIELD_FUNC(voice, midibytes)
GET_STR_FIELD_FUNC(standalone, midibytes)

PUT_STR_FIELD_FUNC(score, midibytes)
PUT_STR_FIELD_FUNC(movementcontrol, midibytes)
PUT_STR_FIELD_FUNC(note, midibytes)
PUT_STR_FIELD_FUNC(chord, midibytes)
PUT_STR_FIELD_FUNCS(staff, midibytes)
PUT_STR_FIELD_FUNCV(voice, midibytes)


GET_STR_FIELD_FUNC(chord, prefix)
GET_STR_FIELD_FUNC(chord, postfix)
GET_STR_FIELD_FUNC(chord, display)

PUT_STR_FIELD_FUNC(chord, prefix)
PUT_STR_FIELD_FUNC(chord, postfix)
PUT_STR_FIELD_FUNC(chord, display)


GET_STR_FIELD_FUNC(note, prefix)
GET_STR_FIELD_FUNC(note, postfix)
GET_STR_FIELD_FUNC(note, display)

PUT_STR_FIELD_FUNC(note, prefix)
PUT_STR_FIELD_FUNC(note, postfix)
PUT_STR_FIELD_FUNC(note, display)

PUT_STR_FIELD_FUNC(score, prefix)
PUT_STR_FIELD_FUNC(score, postfix)
PUT_STR_FIELD_FUNC(score, display)

GET_STR_FIELD_FUNC(standalone, prefix)
GET_STR_FIELD_FUNC(standalone, postfix)
GET_STR_FIELD_FUNC(standalone, display)

GET_STR_FIELD_FUNC(score, prefix)
GET_STR_FIELD_FUNC(score, postfix)
GET_STR_FIELD_FUNC(score, display)

GET_STR_FIELD_FUNC(staff, prefix)
GET_STR_FIELD_FUNC(staff, postfix)
GET_STR_FIELD_FUNC(staff, display)

PUT_STR_FIELD_FUNCS(staff, prefix)
PUT_STR_FIELD_FUNCS(staff, postfix)
PUT_STR_FIELD_FUNCS(staff, display)

GET_STR_FIELD_FUNC(voice, prefix)
GET_STR_FIELD_FUNC(voice, postfix)
GET_STR_FIELD_FUNC(voice, display)

PUT_STR_FIELD_FUNCV(voice, prefix)
PUT_STR_FIELD_FUNCV(voice, postfix)
PUT_STR_FIELD_FUNCV(voice, display)
#undef staff

#define PUT_INT_FIELD_FUNC_NAME(what, field, name)\
gboolean \
what##_directive_put_##field(gchar *tag, gint value) {\
  what *current = get_##what();\
  if(current==NULL) return FALSE;\
  if(current->name==NULL)\
       create_directives (&current->name, tag);\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive==NULL){\
    directive=new_directive(tag);\
    current->name = g_list_append(current->name, directive);\
    }\
  directive->field = value;\
  return TRUE;\
}
#define PUT_INT_FIELD_FUNC(what, field)  PUT_INT_FIELD_FUNC_NAME(what, field, directives)
#define PUT_INT_FIELD_FUNCS(what, field)  PUT_INT_FIELD_FUNC_NAME(what, field, staff_directives)
#define PUT_INT_FIELD_FUNCV(what, field)  PUT_INT_FIELD_FUNC_NAME(what, field, voice_directives)


#define GET_INT_FIELD_FUNC(what, field)\
gint \
what##_directive_get_##field(gchar *tag) {\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive)\
    return directive->field;\
  return 0;\
}




     /* block which can be copied for new int fields */
PUT_INT_FIELD_FUNC(note, minpixels)
PUT_INT_FIELD_FUNC(chord, minpixels)
PUT_INT_FIELD_FUNCS(staff, minpixels)
PUT_INT_FIELD_FUNCV(voice, minpixels)
PUT_INT_FIELD_FUNC(score, minpixels)
PUT_INT_FIELD_FUNC(clef, minpixels)
PUT_INT_FIELD_FUNC(timesig, minpixels)
PUT_INT_FIELD_FUNC(keysig, minpixels)

PUT_INT_FIELD_FUNC(scoreheader, minpixels)
PUT_INT_FIELD_FUNC(header, minpixels)
PUT_INT_FIELD_FUNC(paper, minpixels)
PUT_INT_FIELD_FUNC(layout, minpixels)
PUT_INT_FIELD_FUNC(movementcontrol, minpixels)

     //standalone needs different code for "put" see STANDALONE_PUT* below
GET_INT_FIELD_FUNC(note, minpixels)
GET_INT_FIELD_FUNC(chord, minpixels)
GET_INT_FIELD_FUNC(staff, minpixels)
GET_INT_FIELD_FUNC(voice, minpixels)
GET_INT_FIELD_FUNC(score, minpixels)
GET_INT_FIELD_FUNC(clef, minpixels)
GET_INT_FIELD_FUNC(keysig, minpixels)
GET_INT_FIELD_FUNC(timesig, minpixels)

GET_INT_FIELD_FUNC(scoreheader, minpixels)
GET_INT_FIELD_FUNC(header, minpixels)
GET_INT_FIELD_FUNC(paper, minpixels)
GET_INT_FIELD_FUNC(layout, minpixels)
GET_INT_FIELD_FUNC(movementcontrol, minpixels)



GET_INT_FIELD_FUNC(standalone, minpixels)
  /* end block which can be copied for new int fields */






PUT_INT_FIELD_FUNC(note, override)
PUT_INT_FIELD_FUNC(chord, override)
PUT_INT_FIELD_FUNCS(staff, override)
PUT_INT_FIELD_FUNCV(voice, override)
PUT_INT_FIELD_FUNC(score, override)
GET_INT_FIELD_FUNC(note, override)
GET_INT_FIELD_FUNC(chord, override)
GET_INT_FIELD_FUNC(staff, override)
GET_INT_FIELD_FUNC(voice, override)
GET_INT_FIELD_FUNC(score, override)




PUT_INT_FIELD_FUNC(note, x)
PUT_INT_FIELD_FUNC(chord, x)
PUT_INT_FIELD_FUNCS(staff, x)
PUT_INT_FIELD_FUNCV(voice, x)

GET_INT_FIELD_FUNC(note, x)
GET_INT_FIELD_FUNC(chord, x)
GET_INT_FIELD_FUNC(staff, x)
GET_INT_FIELD_FUNC(voice, x)
 
PUT_INT_FIELD_FUNC(note, y)
PUT_INT_FIELD_FUNC(chord, y)
PUT_INT_FIELD_FUNCS(staff, y)
PUT_INT_FIELD_FUNCV(voice, y)

GET_INT_FIELD_FUNC(note, y)
GET_INT_FIELD_FUNC(chord, y)
GET_INT_FIELD_FUNC(staff, y)
GET_INT_FIELD_FUNC(voice, y)
 

PUT_INT_FIELD_FUNC(note, tx)
PUT_INT_FIELD_FUNC(chord, tx)
PUT_INT_FIELD_FUNCS(staff, tx)
PUT_INT_FIELD_FUNCV(voice, tx)
GET_INT_FIELD_FUNC(note, tx)
GET_INT_FIELD_FUNC(chord, tx)
GET_INT_FIELD_FUNC(staff, tx)
GET_INT_FIELD_FUNC(voice, tx)

PUT_INT_FIELD_FUNC(note, ty)
PUT_INT_FIELD_FUNC(chord, ty)
PUT_INT_FIELD_FUNCS(staff, ty)
PUT_INT_FIELD_FUNCV(voice, ty)
GET_INT_FIELD_FUNC(note, ty)
GET_INT_FIELD_FUNC(chord, ty)
GET_INT_FIELD_FUNC(staff, ty)
GET_INT_FIELD_FUNC(voice, ty)


PUT_INT_FIELD_FUNC(note, gx)
PUT_INT_FIELD_FUNC(chord, gx)
PUT_INT_FIELD_FUNCS(staff, gx)
PUT_INT_FIELD_FUNCV(voice, gx)
GET_INT_FIELD_FUNC(note, gx)
GET_INT_FIELD_FUNC(chord, gx)
GET_INT_FIELD_FUNC(staff, gx)
GET_INT_FIELD_FUNC(voice, gx)


PUT_INT_FIELD_FUNC(note, gy)
PUT_INT_FIELD_FUNC(chord, gy)
PUT_INT_FIELD_FUNCS(staff, gy)
PUT_INT_FIELD_FUNCV(voice, gy)
GET_INT_FIELD_FUNC(note, gy)
GET_INT_FIELD_FUNC(chord, gy)
GET_INT_FIELD_FUNC(staff, gy)
GET_INT_FIELD_FUNC(voice, gy)

GET_INT_FIELD_FUNC(standalone, override)
GET_INT_FIELD_FUNC(standalone, x)
GET_INT_FIELD_FUNC(standalone, y)

GET_INT_FIELD_FUNC(standalone, tx)
GET_INT_FIELD_FUNC(standalone, ty)

GET_INT_FIELD_FUNC(standalone, gx)
GET_INT_FIELD_FUNC(standalone, gy)


GET_INT_FIELD_FUNC(score, x)
GET_INT_FIELD_FUNC(score, y)

GET_INT_FIELD_FUNC(score, tx)
GET_INT_FIELD_FUNC(score, ty)

GET_INT_FIELD_FUNC(score, gx)
GET_INT_FIELD_FUNC(score, gy)

PUT_INT_FIELD_FUNC(score, x)
PUT_INT_FIELD_FUNC(score, y)

PUT_INT_FIELD_FUNC(score, tx)
PUT_INT_FIELD_FUNC(score, ty)

PUT_INT_FIELD_FUNC(score, gx)
PUT_INT_FIELD_FUNC(score, gy)
PUT_INT_FIELD_FUNC(score, width)
PUT_INT_FIELD_FUNC(score, height)



     /* width and height of graphic (if any), read only */
GET_INT_FIELD_FUNC(note, width)
GET_INT_FIELD_FUNC(chord, width)
GET_INT_FIELD_FUNC(staff, width)
GET_INT_FIELD_FUNC(voice, width)
GET_INT_FIELD_FUNC(standalone, width)
GET_INT_FIELD_FUNC(score, width)
GET_INT_FIELD_FUNC(note, height)
GET_INT_FIELD_FUNC(chord, height)
GET_INT_FIELD_FUNC(staff, height)
GET_INT_FIELD_FUNC(voice, height)
GET_INT_FIELD_FUNC(standalone, height)
GET_INT_FIELD_FUNC(score, height)



     //note I think you cannot change the graphic once you have set it.
#define PUT_GRAPHIC_NAME(what, directives) gboolean \
what##_directive_put_graphic(gchar *tag, gchar *value) {\
  what *current = get_##what();\
  if(current==NULL) return FALSE;\
  if(current->directives==NULL)\
       create_directives (&current->directives, tag);\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive==NULL){\
    directive=new_directive(tag);\
    current->directives = g_list_append(current->directives, directive);\
    }\
  loadGraphicItem(value, &directive->graphic, &directive->width, &directive->height);\
  if(directive->graphic_name)\
     g_string_assign(directive->graphic_name, value);\
  else\
      directive->graphic_name = g_string_new(value);\
  return TRUE;\
}


static void 
widget_for_type(DenemoDirective *directive, gchar *value, void fn()){
  GtkWidget *box;
  if(fn==score_directive_put_graphic ||fn==scoreheader_directive_put_graphic ||fn==paper_directive_put_graphic )  
    box = Denemo.gui->buttonbox;
  else
    box = Denemo.gui->si->buttonbox;
  if(fn==staff_directive_put_graphic) {
    //g_print("Doing the staff case");
    directive->graphic = gtk_menu_item_new_with_label(value);
    /* g_print("directive-type %s.....", thetype);	*/
    GtkWidget *menu;
    menu = ((DenemoStaff*)Denemo.gui->si->currentstaff->data)->staffmenu;/*gtk_ui_manager_get_widget (Denemo.ui_manager, "/StaffMenuPopup"); */ 
    g_signal_connect(G_OBJECT(directive->graphic), "activate",  G_CALLBACK(edit_directive_callback), fn);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), directive->graphic);
  } else {   
    if(fn==voice_directive_put_graphic) {
      //g_print("Doing the staff case");
      directive->graphic = gtk_menu_item_new_with_label(value);
      /* g_print("directive-type %s.....", thetype);	*/
      GtkWidget *menu;
      menu = ((DenemoStaff*)Denemo.gui->si->currentstaff->data)->voicemenu;/*gtk_ui_manager_get_widget (Denemo.ui_manager, "/StaffMenuPopup"); */ 
      g_signal_connect(G_OBJECT(directive->graphic), "activate",  G_CALLBACK(edit_directive_callback), fn);
      gtk_menu_shell_append(GTK_MENU_SHELL(menu), directive->graphic);
    }  else  {
      //g_print("Doing the non-staff case");
      directive->graphic = gtk_button_new_with_label(value);
      g_signal_connect(G_OBJECT(directive->graphic), "clicked",  G_CALLBACK(edit_directive_callback), fn);
      gtk_box_pack_start (GTK_BOX (box), directive->graphic, FALSE, TRUE,0);
      gtk_widget_show(directive->graphic);
      gtk_widget_show(box);
    }
  }

  g_object_set_data(directive->graphic, "directive", (gpointer)directive);
  gtk_widget_show_all(GTK_WIDGET(directive->graphic));
  GTK_WIDGET_UNSET_FLAGS(directive->graphic, GTK_CAN_FOCUS);

  if(directive->graphic_name)
    g_string_assign(directive->graphic_name, value);
  else
    directive->graphic_name = g_string_new(value);
  if(GTK_IS_BUTTON(directive->graphic))
    gtk_button_set_label(directive->graphic, value);
  else
    gtk_menu_item_set_label_text(directive->graphic, value);
}


// create a directive for non-DenemoObject directive #what
//if value is not NULL create a button to edit the directive as the directive->graphic
#define PUT_GRAPHIC_WIDGET(what, directives) gpointer \
what##_directive_put_graphic(gchar *tag, gchar *value) {\
  what *current = get_##what();\
  if(current==NULL) return NULL;\
  if(current->directives==NULL)\
       create_directives (&current->directives, tag);\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive==NULL){\
    directive=new_directive(tag);\
    current->directives = g_list_append(current->directives, directive);\
    }\
  if(value && directive->graphic==NULL) {\
    widget_for_type(directive, value, what##_directive_put_graphic);\
  }\
  return (gpointer)directive;\
}



#define PUT_GRAPHIC(what) PUT_GRAPHIC_NAME(what, directives)
#define PUT_GRAPHICS(what) PUT_GRAPHIC_NAME(what, staff_directives)
#define PUT_GRAPHICV(what) PUT_GRAPHIC_NAME(what, voice_directives)


PUT_GRAPHIC(chord);
PUT_GRAPHIC(note);


PUT_GRAPHIC_WIDGET(score, directives);
PUT_GRAPHIC_WIDGET(staff, staff_directives);
PUT_GRAPHIC_WIDGET(voice, voice_directives);
PUT_GRAPHIC_WIDGET(scoreheader, directives);
PUT_GRAPHIC_WIDGET(header, directives);
PUT_GRAPHIC_WIDGET(paper, directives);
PUT_GRAPHIC_WIDGET(layout, directives);
PUT_GRAPHIC_WIDGET(movementcontrol, directives);

gboolean
standalone_directive_put_graphic(gchar *tag, gchar *value) {
  DenemoDirective *directive = get_standalone_directive(tag);
  if(directive && directive->graphic) {
    
    // directive->graphic = NULL; FIXME should we do this...
    //g_string_free(directive->graphic_name, TRUE);
  }
  if(!directive) {
   	DenemoObject *obj = lily_directive_new (" ");
        directive = (DenemoDirective*)obj->object;
        directive->tag = g_string_new(tag);
	object_insert(Denemo.gui, obj);
  }
  if( loadGraphicItem(value, &directive->graphic, &directive->width, &directive->height)) {
    if(directive->graphic_name)
      g_string_assign(directive->graphic_name, value);
    else
      directive->graphic_name = g_string_new(value);
    return TRUE;
  } else 
    return FALSE;
}



#define STANDALONE_PUT_STR_FIELD_FUNC(field)\
gboolean \
standalone_directive_put_##field(gchar *tag, gchar *value) {\
  DenemoDirective *directive = get_standalone_directive(tag);\
  if(directive && directive->field)\
    g_string_assign(directive->field, value);\
  else if(directive)\
    directive->field = g_string_new(value);\
  else {\
	DenemoObject *obj = lily_directive_new (" ");\
        directive = (DenemoDirective*)obj->object;\
        directive->tag = g_string_new(tag);\
        directive->field = g_string_new(value);\
	object_insert(Denemo.gui, obj);\
   }\
  return TRUE;\
}

STANDALONE_PUT_STR_FIELD_FUNC(prefix);
STANDALONE_PUT_STR_FIELD_FUNC(postfix);
STANDALONE_PUT_STR_FIELD_FUNC(display);
STANDALONE_PUT_STR_FIELD_FUNC(midibytes);






#define STANDALONE_PUT_INT_FIELD_FUNC(field)\
gboolean \
standalone_directive_put_##field(gchar *tag, gint value) {\
  DenemoDirective *directive = get_standalone_directive(tag);\
  if(directive)\
    directive->field = value;\
  else {\
        DenemoObject *obj = lily_directive_new (" ");\
        directive = (DenemoDirective*)obj->object;\
        directive->tag = g_string_new(tag);\
        directive->field = value;\
	object_insert(Denemo.gui, obj);\
   }\
  return TRUE;\
}

//STANDALONE_PUT_INT_FIELD_FUNC(minpixels); special case
STANDALONE_PUT_INT_FIELD_FUNC(x);
STANDALONE_PUT_INT_FIELD_FUNC(y);
STANDALONE_PUT_INT_FIELD_FUNC(tx);
STANDALONE_PUT_INT_FIELD_FUNC(ty);
STANDALONE_PUT_INT_FIELD_FUNC(gx);
STANDALONE_PUT_INT_FIELD_FUNC(gy);

STANDALONE_PUT_INT_FIELD_FUNC(override);


gboolean 
standalone_directive_put_minpixels(gchar *tag, gint value) {
  DenemoDirective *directive = get_standalone_directive(tag);
  if(directive){
    directive->minpixels = value;//This field is not actually useful for standalone directives.
    DenemoObject *obj = findobj();
    obj->minpixelsalloted = value;
  }
  else {
        DenemoObject *obj = lily_directive_new (" ");
        directive = (DenemoDirective*)obj->object;
        directive->tag = g_string_new(tag);
        obj->minpixelsalloted = directive->minpixels = value;
	object_insert(Denemo.gui, obj);
   }
  return TRUE;
}

/* returns the path to a script file for editing a directive created by command of name commandname 
   a local one takes precedence over the system one
   caller must g_free the result */
static gchar *
get_script_file(gchar *commandname) {
  gchar *name = g_strconcat(commandname, ".scm", NULL);
  gchar* filename = g_build_filename (locatedotdenemo(), "actions", "editscripts", name, NULL);
  if(!g_file_test(filename, G_FILE_TEST_EXISTS)) {
    g_free(filename);
    filename = g_build_filename(get_data_dir(), "actions", "editscripts", name, NULL);
    if(g_file_test(filename, G_FILE_TEST_EXISTS))
      return filename;
    g_free(filename);
    return NULL;
  }
  return filename;
}

static gboolean
script_file_exists(gchar *commandname){
  commandname = get_script_file(commandname);
  if(commandname==NULL) return FALSE;
  g_free(commandname);
  return TRUE;
}

static gboolean
tag_choice(GtkWidget *widget, DenemoDirective **response) {
  if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    *response = g_object_get_data(G_OBJECT(widget), "choice");
  return TRUE;
}
#define UNKNOWN_TAG "<Unknown Tag>"

/* pack radio buttons for directive choice */
static gint
pack_buttons(GtkWidget *vbox, GList *directives, DenemoDirective **response) {
  GList *g;
  gint count;
  GtkWidget *widget=NULL, *widget2;
  for(count=0, g=directives;g;g=g->next) {
    DenemoDirective *directive = (DenemoDirective *) g->data;
    if (directive->tag == NULL)
      directive->tag = g_string_new(UNKNOWN_TAG);
    count++;
    if(*response==NULL)
      *response = directive;
    if(widget==NULL) {
      widget =   gtk_radio_button_new_with_label(NULL, directive->tag->str);
      g_object_set_data(G_OBJECT(widget), "choice", directive);
      g_signal_connect(G_OBJECT(widget), "toggled", G_CALLBACK(tag_choice), response);
      gtk_box_pack_start (GTK_BOX (vbox), widget, FALSE, TRUE, 0);
    } else {
      widget2  =   gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (widget), directive->tag->str);
      g_object_set_data(G_OBJECT(widget2), "choice", directive);
      g_signal_connect(G_OBJECT(widget2), "toggled", G_CALLBACK(tag_choice), response);
      gtk_box_pack_start (GTK_BOX (vbox), widget2, FALSE, TRUE, 0);
    }
  }
  return count;
}


/* let the user choose from a list of directives */
static
DenemoDirective *select_directive(gchar *instr, GList *directives) {

  GtkWidget *dialog = gtk_dialog_new_with_buttons ("Select Directive",
                                        GTK_WINDOW (Denemo.window),
                                        (GtkDialogFlags) (GTK_DIALOG_MODAL |
                                                       GTK_DIALOG_DESTROY_WITH_PARENT),
                                        GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
                                        NULL);
  GtkWidget *vbox = gtk_vbox_new(FALSE, 8);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), vbox,
		      TRUE, TRUE, 0);
  
  DenemoDirective *response = NULL;
  GList *g;
  gint count;//count tagged directives
  GtkWidget *widget, *widget2;
  widget = gtk_label_new(instr);
  gtk_box_pack_start (GTK_BOX (vbox), widget, FALSE, TRUE, 0);
  count = pack_buttons(vbox, directives, &response);

  if(count>0) {    
    gtk_widget_show_all (dialog);
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_REJECT){ 
      response = NULL;
    }
  }
  gtk_widget_destroy (dialog);
  if(response && response->tag)
    g_print("Came back with response %s\n", response->tag->str);
  return response;
}


/* let the user choose from the directives at the cursor */
static 
void user_select_directive_at_cursor(gchar **what, GList ***pdirectives, DenemoDirective **pdirective) {
  *pdirectives = NULL;
  *pdirective = get_standalone_directive(NULL);
  if(*pdirective)
    return;
  gchar *name=NULL;
  note *curnote = get_note();
  if(curnote!=NULL) {
    name = mid_c_offsettolily(curnote->mid_c_offset, curnote->enshift);
    if(curnote->mid_c_offset == Denemo.gui->si->cursor_y)
      if(curnote->directives) {
	*pdirectives = &curnote->directives;
	*what = "note";
	gchar *instr = g_strdup_printf("Select a directive attached to the note \"%s\"", name);
	*pdirective = select_directive(instr, **pdirectives);
	g_free(instr);
	if(*pdirective) {
	  g_free(name);
	  return;
	}
      }
  }
  

  {
  // not exactly on a note, offer any chord directives
    gchar *instr = "Select a directive attached to the chord";
    chord *curchord = get_chord();
    if(curchord && curchord->directives) {
      *pdirectives = &curchord->directives;
      *what = "chord";
      *pdirective = select_directive(instr, **pdirectives);
    } 
  }
  if(*pdirective==NULL && curnote)//try nearest note
    if(curnote->directives && curnote->mid_c_offset != Denemo.gui->si->cursor_y) {
      *pdirectives = &curnote->directives;
      *what = "note";
      gchar *instr = g_strdup_printf("Select a directive attached to the note \"%s\"", name);
      *pdirective = select_directive(instr, **pdirectives);
      g_free(instr);
      if(*pdirective && (g_list_length(**pdirectives)==1)) {
	/* seek confirmation of the choice of this directive since it is on a note not pointed at and
	   has been chosen automatically. */
	gchar *name = mid_c_offsettolily(curnote->mid_c_offset, curnote->enshift);
	gchar *msg = g_strdup_printf("Select the directive %s on note \"%s\"?", (*pdirective)->tag->str, name);

	if(!confirm("Select Directive", msg))
	  *pdirective = NULL;
	g_free(name);
	g_free(msg);
      }
    }
  g_free(name);
  return;
}





void edit_object(GtkAction *action,  DenemoScriptParam *param) {
  DenemoObject *obj = findobj();
  if(obj==NULL){
      warningdialog("No object here to edit");
    return;
  }
  switch(obj->type){
  case LILYDIRECTIVE:
    edit_object_directive(action, param);
    return;
  case CLEF:
     {
      popup_menu( "/ClefEditPopup");
    }  
     return;
  case KEYSIG:
     {
      popup_menu( "/KeyEditPopup");
    }  
     return;
  case TIMESIG:
     {
      popup_menu( "/TimeEditPopup");
    }  
     return;
  case CHORD:
    {
      popup_menu( "/NoteEditPopup");
    }
    return;

  case STEMDIRECTIVE:
     {
      popup_menu( "/StemControlPopup");
    }  
     return;

  default:
    warningdialog("No method for editing this type of object");
    return;
  }
}

static gboolean set_gstring (GtkWidget   *widget,   GdkEventKey *event, GString *gstr)  {
  g_string_assign(gstr, (gchar*)gtk_entry_get_text(GTK_ENTRY(widget)));
  return TRUE;
}
static gboolean set_int (GtkSpinButton *widget,  gint *val)  {
  *val = gtk_spin_button_get_value_as_int (widget);
  return TRUE;
}

static gchar* quote_scheme(gchar *s) {
  GString *dest = g_string_new("");
  gchar *c;
  for(c=s;*c;c++) {
    if(*c=='"' || *c=='\\')
      g_string_append_c(dest, '\\');
    g_string_append_c(dest, *c);  
  }
  return g_string_free(dest, FALSE);
}

static void create_script(DenemoDirective *directive, gchar *what) {
  GString *scheme = g_string_new(directive->tag->str);
  g_string_prepend(scheme, ";;;");
  g_string_append(scheme, "\n");

#define ADD_TEXT(field)\
if(directive->field)\
  {gchar *quote = quote_scheme(directive->field->str);\
   g_string_append_printf(scheme, "(d-DirectivePut-%s-%s \"%s\" \"%s\")\n",\
       what, #field, directive->tag->str, quote);\
   g_free(quote);}
ADD_TEXT(prefix);			 
ADD_TEXT(postfix);			 
ADD_TEXT(display);			 
ADD_TEXT(graphic_name);
ADD_TEXT(midibytes);		 
#undef ADD_TEXT
#define ADD_INTTEXT(field)\
if(directive->field)\
  g_string_append_printf(scheme, "(d-DirectivePut-%s-%s \"%s\" %d)\n",\
       what, #field, directive->tag->str, directive->field);
ADD_INTTEXT(minpixels);
ADD_INTTEXT(override);
ADD_INTTEXT(x);
ADD_INTTEXT(y);
ADD_INTTEXT(tx);
ADD_INTTEXT(ty);
ADD_INTTEXT(gx);
ADD_INTTEXT(gy);

#undef ADD_INTTEXT
 g_string_append(scheme,  "(d-RefreshDisplay)\n;;;End of scheme script");
 // quote_scheme(scheme);
 g_print("Scheme is %s\n", scheme->str);
  appendSchemeText(scheme->str);
  g_string_free(scheme, TRUE);
}





/* text_edit_directive
   textually edit the directive via a dialog.
   return FALSE if the user requests deletion of the directive.
*/
static gboolean text_edit_directive(DenemoDirective *directive, gchar *what) {
  gboolean ret = TRUE;
#define CREATE_SCRIPT (2)
  DenemoDirective *clone = clone_directive(directive);//for reset
  GtkWidget *dialog = gtk_dialog_new_with_buttons ("Primitive Denemo Directive Edit",
                                        GTK_WINDOW (Denemo.window),
                                        (GtkDialogFlags) (GTK_DIALOG_MODAL |
                                                       GTK_DIALOG_DESTROY_WITH_PARENT),
                                        GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        NULL);
  gtk_dialog_add_button (GTK_DIALOG(dialog), "Delete Directive", GTK_RESPONSE_REJECT);
  gtk_dialog_add_button (GTK_DIALOG(dialog), "Create Script", CREATE_SCRIPT);



  GtkWidget *vbox = gtk_vbox_new(FALSE, 8);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), vbox,
		      TRUE, TRUE, 0);
  GtkWidget *hbox;
  GString *entrycontent = g_string_new("");
  GtkWidget *entrywidget;
  GtkWidget *label;
#define TEXTENTRY(thelabel, field) \
  GtkWidget *field;\
  hbox = gtk_hbox_new (FALSE, 8);\
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);\
  label = gtk_label_new (_(thelabel));\
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);\
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);\
  entrywidget = gtk_entry_new ();\
  g_string_sprintf (entrycontent, "%s", directive->field?directive->field->str:"");\
  gtk_entry_set_text (GTK_ENTRY (entrywidget), entrycontent->str);\
  gtk_box_pack_start (GTK_BOX (hbox), entrywidget, TRUE, TRUE, 0);\
  if(directive->field==NULL) directive->field=g_string_new("");\
  g_signal_connect(G_OBJECT(entrywidget), "key-release-event", G_CALLBACK(set_gstring), directive->field);\
  g_string_assign(entrycontent, "");

#define NEWINTENTRY(thelabel, field)\
  hbox = gtk_hbox_new (FALSE, 8);\
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);\
  label = gtk_label_new (_(thelabel));\
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);\
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);\
  entrywidget = gtk_spin_button_new_with_range (0.0, (gdouble)G_MAXUINT, 1.0);\
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (entrywidget), directive->field);\
  gtk_box_pack_start (GTK_BOX (hbox), entrywidget, TRUE, TRUE, 0);\
  g_signal_connect(G_OBJECT(entrywidget), "value-changed", G_CALLBACK(set_int), &directive->field);

#define ADDINTENTRY(thelabel, fieldx, fieldy)\
  label = gtk_label_new (_(thelabel));\
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);\
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);\
  label = gtk_label_new (_(" x:"));\
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);\
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);\
  entrywidget = gtk_spin_button_new_with_range (-(gdouble)G_MAXINT, (gdouble)G_MAXINT, 1.0);\
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (entrywidget), directive->fieldx);\
  gtk_box_pack_start (GTK_BOX (hbox), entrywidget, TRUE, TRUE, 0);\
  g_signal_connect(G_OBJECT(entrywidget), "value-changed", G_CALLBACK(set_int), &directive->fieldx);\
  label = gtk_label_new (_(" y:"));\
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);\
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);\
  entrywidget = gtk_spin_button_new_with_range (-(gdouble)G_MAXINT, (gdouble)G_MAXINT, 1.0);\
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (entrywidget), directive->fieldy);\
  gtk_box_pack_start (GTK_BOX (hbox), entrywidget, TRUE, TRUE, 0);\
  g_signal_connect(G_OBJECT(entrywidget), "value-changed", G_CALLBACK(set_int), &directive->fieldy);

  TEXTENTRY("Postfix", postfix);
  TEXTENTRY("Prefix", prefix);
  TEXTENTRY("Display text", display);
  ADDINTENTRY("Text Position", tx, ty);
  TEXTENTRY("Graphic", graphic_name);
  ADDINTENTRY("Graphic Position", gx, gy);
  TEXTENTRY("Tag", tag);
  TEXTENTRY("MidiBytes", midibytes);
  NEWINTENTRY("Override Mask", override);
#undef TEXTENTRY
  gtk_widget_show_all (dialog);
  gint response = gtk_dialog_run (GTK_DIALOG (dialog));
  g_print("Got response %d\n", response);


  if (response == GTK_RESPONSE_CANCEL || response == GTK_RESPONSE_DELETE_EVENT || response ==  GTK_RESPONSE_REJECT){
    directive->graphic = NULL;//prevent any button being destroyed
    free_directive_data(directive);
    memcpy(directive, clone, sizeof(DenemoDirective));
    if (response ==  GTK_RESPONSE_REJECT) {
      ret = FALSE;//that is it may be deleted, we ensure it has not been changed first,as the tag is used for delelet
    }
  }
  else {
    clone->graphic = NULL;//prevent any button being destroyed
    free_directive(clone);
  }
#define REMOVEEMPTIES(field)\
if(directive->field && directive->field->len==0) g_string_free(directive->field, TRUE), directive->field=NULL;
  REMOVEEMPTIES(postfix);
  REMOVEEMPTIES(prefix);
  REMOVEEMPTIES(display);
  REMOVEEMPTIES(graphic_name);
  
  //REMOVEEMPTIES(tag); don't allow NULL tag
#undef REMOVEEMPTIES

  if(directive->tag && directive->tag->len==0)
    directive->tag = g_string_new(UNKNOWN_TAG);
  if(directive->graphic_name){
    if(directive->graphic && GTK_IS_WIDGET(directive->graphic))
      gtk_menu_item_set_label_text(directive->graphic, directive->graphic_name->str);
    else
      loadGraphicItem (directive->graphic_name->str, &directive->graphic,  &directive->width, &directive->height);
  }
  gtk_widget_destroy (dialog);
  if(response==CREATE_SCRIPT)
    create_script(directive, what);//g_print("(d-DirectivePut-%s \"%s\")\n", what, directive->tag->str);
  return ret;
}

/* allow edit of a directive, either via script or textually if no script exists 
   return FALSE if user confirms a request to delete the directive
*/
static gboolean
edit_directive(DenemoDirective *directive, gchar *what) {
  gboolean ret = TRUE;
  gchar *name = g_strconcat(directive->tag->str, ".scm", NULL);
  gchar* filename = g_build_filename (locatedotdenemo(), "actions", "editscripts", name, NULL);
  if(!g_file_test(filename, G_FILE_TEST_EXISTS)) {
    g_free(filename);
    filename = g_build_filename(get_data_dir(), "actions", "editscripts", name, NULL);
    if(!g_file_test(filename, G_FILE_TEST_EXISTS)){
      ret =( text_edit_directive(directive, what)  || !confirm("Directive Delete", "Are you sure you want to delete the directive?"));
      g_free(filename);
      score_status (Denemo.gui, TRUE);
      return ret;
    }
  }
  GError *error = NULL;
  gchar *script;
  if(g_file_get_contents (filename, &script, NULL, &error)) {
    call_out_to_guile(script);
    g_free(script);
  }
  g_free(filename);
  return ret;
}


static void edit_directive_callback(GtkWidget *w, gpointer what) {
  DenemoDirective *directive = g_object_get_data(w, "directive");
  if(!edit_directive(directive, what)){
    if(what == score_directive_put_graphic)
      delete_score_directive(directive->tag->str);
    else
      if(what == staff_directive_put_graphic)
	delete_staff_directive(directive->tag->str);
      else
	if(what == voice_directive_put_graphic)
	  delete_voice_directive(directive->tag->str);
	else
	  if(what == scoreheader_directive_put_graphic)
	    delete_scoreheader_directive(directive->tag->str);
	  else
	  if(what == header_directive_put_graphic)
	    delete_header_directive(directive->tag->str);
	  else
	  if(what == paper_directive_put_graphic)
	    delete_paper_directive(directive->tag->str);
	  else
	  if(what == layout_directive_put_graphic)
	    delete_layout_directive(directive->tag->str);
	  else
	  if(what == movementcontrol_directive_put_graphic)
	    delete_movementcontrol_directive(directive->tag->str);
	  else




	    g_warning("Not implemented");
    
    
  }    
 }


/**
 * callback for EditDirective on directive attached to an object.
 */
void edit_object_directive(GtkAction *action,  DenemoScriptParam *param) {
  //g_print("Edit directive called\n");
  DenemoDirective *directive;
  GList **directives;
  gchar *what=NULL;
  user_select_directive_at_cursor(&what, &directives, &directive);
  //g_print("Got directive %p in list %p\n", directive, directives);
  if(directive==NULL) {
    warningdialog("Use the ObjectMenu to modify this object - there are no directives here");
    return;
  }
  if(directive->tag == NULL)
    directive->tag = g_string_new(UNKNOWN_TAG);
  if(!edit_directive(directive, what)) {
    if(directives && *directives) {
      *directives = g_list_remove(*directives, directive);
      free_directive(directive);
    } else {//standalone directive
      dnm_deleteobject(Denemo.gui->si);
    }
  }
}

/**
 * callback for DeleteDirective 
 */
void delete_object_directive(GtkAction *action,  DenemoScriptParam *param) {
  //g_print("Edit directive called\n");
  DenemoDirective *directive;
  GList **directives;
  gchar *what=NULL;
  user_select_directive_at_cursor(&what, &directives, &directive);
  //g_print("Got directive %p in list %p\n", directive, directives);
  if(directives==NULL) {
    warningdialog("No directives here");
    return;
  }
  if(directive==NULL) {
    warningdialog("No directive selected");
    return;
  }
  if(directive->tag == NULL)
    directive->tag = g_string_new(UNKNOWN_TAG);
  if(confirm("Directive Delete", "Are you sure you want to delete the directive?"))
    delete_directive(directives, directive->tag->str);
  else
    warningdialog("Operation cancelled");
}



static
DenemoDirective *select_score_directive(void) {
  if(Denemo.gui->lilycontrol.directives==NULL)
    return NULL;
  return select_directive("Select a score directive", Denemo.gui->lilycontrol.directives);
}

static
DenemoDirective *select_scoreheader_directive(void) {
  if(Denemo.gui->scoreheader.directives==NULL)
    return NULL;
  return select_directive("Select a score header block directive", Denemo.gui->scoreheader.directives);
}

static
DenemoDirective *select_paper_directive(void) {
  if(Denemo.gui->paper.directives==NULL)
    return NULL;
  return select_directive("Select a score paper block directive", Denemo.gui->paper.directives);
}


static
DenemoDirective *select_header_directive(void) {
  if(Denemo.gui->si->header.directives==NULL)
    return NULL;
  return select_directive("Select a movement header block directive", Denemo.gui->si->header.directives);
}
static
DenemoDirective *select_layout_directive(void) {
  if(Denemo.gui->si->layout.directives==NULL)
    return NULL;
  return select_directive("Select a movement layout block directive", Denemo.gui->si->layout.directives);
}

static
DenemoDirective *select_movementcontrol_directive(void) {
  if(Denemo.gui->si->movementcontrol.directives==NULL)
    return NULL;
  return select_directive("Select a movement control directive", Denemo.gui->si->movementcontrol.directives);
}

static
DenemoDirective *select_clef_directive(void) {
  clef *curclef = get_clef();
  if(curclef==NULL || curclef->directives==NULL)
    return NULL;
  return select_directive("Select a clef directive", curclef->directives);
}

static
DenemoDirective *select_keysig_directive(void) {
  keysig *curkeysig = get_keysig();
  if(curkeysig==NULL || curkeysig->directives==NULL)
    return NULL;
  return select_directive("Select a key signature directive", curkeysig->directives);
}

static
DenemoDirective *select_timesig_directive(void) {
  timesig *curtimesig = get_timesig();
  if(curtimesig==NULL || curtimesig->directives==NULL)
    return NULL;
  return select_directive("Select a time signature directive", curtimesig->directives);
}

static
DenemoDirective *select_staff_directive(void) {
  if(Denemo.gui->si->currentstaff==NULL)
    return NULL;
  DenemoStaff *curstaff = Denemo.gui->si->currentstaff->data;
  //FIXME return NULL if not primary staff
  if(curstaff==NULL || curstaff->staff_directives==NULL)
    return NULL;
  return select_directive("Select a staff directive", curstaff->staff_directives);
}
static
DenemoDirective *select_voice_directive(void) {
  if(Denemo.gui->si->currentstaff==NULL)
    return NULL;
  DenemoStaff *curstaff = Denemo.gui->si->currentstaff->data;
  if(curstaff==NULL || curstaff->voice_directives==NULL)
    return NULL;
  return select_directive("Select a voice directive", curstaff->voice_directives);
}


/**
 * callback for EditVoiceDirective 
 */
void edit_voice_directive(GtkAction *action,  DenemoScriptParam *param) {
  //g_print("Edit directive called\n");
  DenemoDirective *directive = select_voice_directive();
  g_print("Got directive %p\n", directive);
  if(directive==NULL)
    return;
  if(directive->tag == NULL)
    directive->tag = g_string_new(UNKNOWN_TAG);
  if(!edit_directive(directive, "voice"))
    delete_voice_directive(directive->tag->str);
  score_status (Denemo.gui, TRUE);
}

/**
 * callback for EditStaffDirective 
 */
void edit_staff_directive(GtkAction *action,  DenemoScriptParam *param) {
  //g_print("Edit directive called\n");
  DenemoDirective *directive = select_staff_directive();
  g_print("Got directive %p\n", directive);
  if(directive==NULL)
    return;
  if(directive->tag == NULL)
    directive->tag = g_string_new(UNKNOWN_TAG);
  if(!edit_directive(directive, "staff"))
    delete_staff_directive(directive->tag->str);
  score_status (Denemo.gui, TRUE);
}

/**
 * callback for EditClefDirective 
 */
void edit_clef_directive(GtkAction *action,  DenemoScriptParam *param) {
  //g_print("Edit directive called\n");
  DenemoDirective *directive = select_clef_directive();
  //g_print("Got directive %p\n", directive);
  if(directive==NULL)
    return;
  if(directive->tag == NULL)
    directive->tag = g_string_new(UNKNOWN_TAG);
  if(!edit_directive(directive, "clef"))
    delete_clef_directive(directive->tag->str);
  score_status (Denemo.gui, TRUE);
}
/**
 * callback for EditKeysigDirective 
 */
void edit_keysig_directive(GtkAction *action,  DenemoScriptParam *param) {
  //g_print("Edit directive called\n");
  DenemoDirective *directive = select_keysig_directive();
  //g_print("Got directive %p\n", directive);
  if(directive==NULL)
    return;
  if(directive->tag == NULL)
    directive->tag = g_string_new(UNKNOWN_TAG);
  if(!edit_directive(directive, "keysig"))
    delete_keysig_directive(directive->tag->str);
  score_status (Denemo.gui, TRUE);
}


/**
 * callback for EditTimesigDirective 
 */
void edit_timesig_directive(GtkAction *action,  DenemoScriptParam *param) {
  //g_print("Edit directive called\n");
  DenemoDirective *directive = select_timesig_directive();
  //g_print("Got directive %p\n", directive);
  if(directive==NULL)
    return;
  if(directive->tag == NULL)
    directive->tag = g_string_new(UNKNOWN_TAG);
  if(!edit_directive(directive, "timesig"))
    delete_timesig_directive(directive->tag->str);
  score_status (Denemo.gui, TRUE);
}

/**
 * callback for EditScoreDirective 
 */
void edit_score_directive(GtkAction *action,  DenemoScriptParam *param) {
#define ScoreDirectives  "ScoreDirectives"
#define ScoreHeaderBlockDirectives  "Score Header Block Directives"
#define ScorePaperBlockDirectives  "Score Paper Block Directives"
#define HeaderBlockDirectives  "Movement Header Block Directives"

#define STRINGAPPEND(field)  g_string_append_len(options, field"\0", 1+strlen(field))
  GString *options = g_string_new("");
  gchar *option;
  if(Denemo.gui->lilycontrol.directives)
    STRINGAPPEND(ScoreDirectives);
  if(Denemo.gui->scoreheader.directives)
     STRINGAPPEND(ScoreHeaderBlockDirectives);
  if(Denemo.gui->paper.directives)
    STRINGAPPEND(ScorePaperBlockDirectives);
  if(Denemo.gui->si->header.directives)
    STRINGAPPEND(HeaderBlockDirectives);

  if(strlen(options->str) != options->len) {
    option = get_option(options->str, options->len);
    if(option==NULL) {
      g_string_free(options, TRUE);
      return;
    }
  } else
    option = options->str;
#define EDITTYPE(type, what)\
  if(!strcmp(option, type)) {\
    DenemoDirective *directive = select_##what##_directive();\
    if(directive==NULL)\
      return;\
    if(directive->tag == NULL)\
      directive->tag = g_string_new(UNKNOWN_TAG);\
    if(!edit_directive(directive, #what))\
      delete_##what##_directive(directive->tag->str);\
  score_status (Denemo.gui, TRUE);\
  }


  EDITTYPE(ScoreDirectives, score);
  EDITTYPE(ScoreHeaderBlockDirectives, scoreheader);
  EDITTYPE(ScorePaperBlockDirectives, paper);
  EDITTYPE(HeaderBlockDirectives, header);

  //  g_print("option was %s\n",option);
  g_string_free(options, TRUE);
#undef EDITTYPE
#undef STRINGAPPEND
}



/**
 * callback for EditMovementDirective 
 */
void edit_movement_directive(GtkAction *action,  DenemoScriptParam *param) {
#define LayoutDirectives  "LayoutDirectives"
#define MovementDirectives  "MovementDirectives"
#define HeaderBlockDirectives  "Movement Header Block Directives"

#define STRINGAPPEND(field)  g_string_append_len(options, field"\0", 1+strlen(field))
  GString *options = g_string_new("");
  gchar *option;
  if(Denemo.gui->si->layout.directives)
    STRINGAPPEND(LayoutDirectives);
  if(Denemo.gui->si->movementcontrol.directives)
    STRINGAPPEND(MovementDirectives);
 
  if(Denemo.gui->si->header.directives)
    STRINGAPPEND(HeaderBlockDirectives);

  if(strlen(options->str) != options->len) {
    option = get_option(options->str, options->len);
    if(option==NULL) {
      g_string_free(options, TRUE);
      return;
    }
  } else
    option = options->str;
#define EDITTYPE(type, what)\
  if(!strcmp(option, type)) {\
    DenemoDirective *directive = select_##what##_directive();\
    if(directive==NULL)\
      return;\
    if(directive->tag == NULL)\
      directive->tag = g_string_new(UNKNOWN_TAG);\
    if(!edit_directive(directive, #what))\
      delete_##what##_directive(directive->tag->str);\
  score_status (Denemo.gui, TRUE);\
  }



  EDITTYPE(HeaderBlockDirectives, header);
  EDITTYPE(LayoutDirectives, layout);
  EDITTYPE(MovementDirectives, movementcontrol);

  g_string_free(options, TRUE);
#undef EDITTYPE
#undef STRINGAPPEND
}




/* block which can be copied for type of directive (minpixels is done as sample for new int fields */
PUT_INT_FIELD_FUNC(clef, x)
PUT_INT_FIELD_FUNC(clef, y)
PUT_INT_FIELD_FUNC(clef, tx)
PUT_INT_FIELD_FUNC(clef, ty)
PUT_INT_FIELD_FUNC(clef, gx)
PUT_INT_FIELD_FUNC(clef, gy)
PUT_INT_FIELD_FUNC(clef, override)
GET_INT_FIELD_FUNC(clef, x)
GET_INT_FIELD_FUNC(clef, y)
GET_INT_FIELD_FUNC(clef, tx)
GET_INT_FIELD_FUNC(clef, ty)
GET_INT_FIELD_FUNC(clef, gx)
GET_INT_FIELD_FUNC(clef, gy)
GET_INT_FIELD_FUNC(clef, override)
GET_INT_FIELD_FUNC(clef, width)
GET_INT_FIELD_FUNC(clef, height)

PUT_GRAPHIC(clef)

PUT_STR_FIELD_FUNC(clef, prefix)
PUT_STR_FIELD_FUNC(clef, postfix)
PUT_STR_FIELD_FUNC(clef, display)

GET_STR_FIELD_FUNC(clef, prefix)
GET_STR_FIELD_FUNC(clef, postfix)
GET_STR_FIELD_FUNC(clef, display)

/* end block which can be copied for type of directive */



PUT_INT_FIELD_FUNC(keysig, x)
PUT_INT_FIELD_FUNC(keysig, y)
PUT_INT_FIELD_FUNC(keysig, tx)
PUT_INT_FIELD_FUNC(keysig, ty)
PUT_INT_FIELD_FUNC(keysig, gx)
PUT_INT_FIELD_FUNC(keysig, gy)
PUT_INT_FIELD_FUNC(keysig, override)
GET_INT_FIELD_FUNC(keysig, x)
GET_INT_FIELD_FUNC(keysig, y)
GET_INT_FIELD_FUNC(keysig, tx)
GET_INT_FIELD_FUNC(keysig, ty)
GET_INT_FIELD_FUNC(keysig, gx)
GET_INT_FIELD_FUNC(keysig, gy)
GET_INT_FIELD_FUNC(keysig, override)
GET_INT_FIELD_FUNC(keysig, width)
GET_INT_FIELD_FUNC(keysig, height)

PUT_GRAPHIC(keysig)

PUT_STR_FIELD_FUNC(keysig, prefix)
PUT_STR_FIELD_FUNC(keysig, postfix)
PUT_STR_FIELD_FUNC(keysig, display)

GET_STR_FIELD_FUNC(keysig, prefix)
GET_STR_FIELD_FUNC(keysig, postfix)
GET_STR_FIELD_FUNC(keysig, display)

PUT_INT_FIELD_FUNC(timesig, x)
PUT_INT_FIELD_FUNC(timesig, y)
PUT_INT_FIELD_FUNC(timesig, tx)
PUT_INT_FIELD_FUNC(timesig, ty)
PUT_INT_FIELD_FUNC(timesig, gx)
PUT_INT_FIELD_FUNC(timesig, gy)
PUT_INT_FIELD_FUNC(timesig, override)
GET_INT_FIELD_FUNC(timesig, x)
GET_INT_FIELD_FUNC(timesig, y)
GET_INT_FIELD_FUNC(timesig, tx)
GET_INT_FIELD_FUNC(timesig, ty)
GET_INT_FIELD_FUNC(timesig, gx)
GET_INT_FIELD_FUNC(timesig, gy)
GET_INT_FIELD_FUNC(timesig, override)
GET_INT_FIELD_FUNC(timesig, width)
GET_INT_FIELD_FUNC(timesig, height)

PUT_GRAPHIC(timesig)

PUT_STR_FIELD_FUNC(timesig, prefix)
PUT_STR_FIELD_FUNC(timesig, postfix)
PUT_STR_FIELD_FUNC(timesig, display)

GET_STR_FIELD_FUNC(timesig, prefix)
GET_STR_FIELD_FUNC(timesig, postfix)
GET_STR_FIELD_FUNC(timesig, display)



PUT_INT_FIELD_FUNC(scoreheader, x)
PUT_INT_FIELD_FUNC(scoreheader, y)
PUT_INT_FIELD_FUNC(scoreheader, tx)
PUT_INT_FIELD_FUNC(scoreheader, ty)
PUT_INT_FIELD_FUNC(scoreheader, gx)
PUT_INT_FIELD_FUNC(scoreheader, gy)
PUT_INT_FIELD_FUNC(scoreheader, override)
GET_INT_FIELD_FUNC(scoreheader, x)
GET_INT_FIELD_FUNC(scoreheader, y)
GET_INT_FIELD_FUNC(scoreheader, tx)
GET_INT_FIELD_FUNC(scoreheader, ty)
GET_INT_FIELD_FUNC(scoreheader, gx)
GET_INT_FIELD_FUNC(scoreheader, gy)
GET_INT_FIELD_FUNC(scoreheader, override)
GET_INT_FIELD_FUNC(scoreheader, width)
GET_INT_FIELD_FUNC(scoreheader, height)

     //PUT_GRAPHIC(scoreheader)

PUT_STR_FIELD_FUNC(scoreheader, prefix)
PUT_STR_FIELD_FUNC(scoreheader, postfix)
PUT_STR_FIELD_FUNC(scoreheader, display)

GET_STR_FIELD_FUNC(scoreheader, prefix)
GET_STR_FIELD_FUNC(scoreheader, postfix)
GET_STR_FIELD_FUNC(scoreheader, display)


PUT_INT_FIELD_FUNC(header, x)
PUT_INT_FIELD_FUNC(header, y)
PUT_INT_FIELD_FUNC(header, tx)
PUT_INT_FIELD_FUNC(header, ty)
PUT_INT_FIELD_FUNC(header, gx)
PUT_INT_FIELD_FUNC(header, gy)
PUT_INT_FIELD_FUNC(header, override)
GET_INT_FIELD_FUNC(header, x)
GET_INT_FIELD_FUNC(header, y)
GET_INT_FIELD_FUNC(header, tx)
GET_INT_FIELD_FUNC(header, ty)
GET_INT_FIELD_FUNC(header, gx)
GET_INT_FIELD_FUNC(header, gy)
GET_INT_FIELD_FUNC(header, override)
GET_INT_FIELD_FUNC(header, width)
GET_INT_FIELD_FUNC(header, height)

     //PUT_GRAPHIC(header)

PUT_STR_FIELD_FUNC(header, prefix)
PUT_STR_FIELD_FUNC(header, postfix)
PUT_STR_FIELD_FUNC(header, display)

GET_STR_FIELD_FUNC(header, prefix)
GET_STR_FIELD_FUNC(header, postfix)
GET_STR_FIELD_FUNC(header, display)


PUT_INT_FIELD_FUNC(paper, x)
PUT_INT_FIELD_FUNC(paper, y)
PUT_INT_FIELD_FUNC(paper, tx)
PUT_INT_FIELD_FUNC(paper, ty)
PUT_INT_FIELD_FUNC(paper, gx)
PUT_INT_FIELD_FUNC(paper, gy)
PUT_INT_FIELD_FUNC(paper, override)
GET_INT_FIELD_FUNC(paper, x)
GET_INT_FIELD_FUNC(paper, y)
GET_INT_FIELD_FUNC(paper, tx)
GET_INT_FIELD_FUNC(paper, ty)
GET_INT_FIELD_FUNC(paper, gx)
GET_INT_FIELD_FUNC(paper, gy)
GET_INT_FIELD_FUNC(paper, override)
GET_INT_FIELD_FUNC(paper, width)
GET_INT_FIELD_FUNC(paper, height)

     //PUT_GRAPHIC(paper)

PUT_STR_FIELD_FUNC(paper, prefix)
PUT_STR_FIELD_FUNC(paper, postfix)
PUT_STR_FIELD_FUNC(paper, display)

GET_STR_FIELD_FUNC(paper, prefix)
GET_STR_FIELD_FUNC(paper, postfix)
GET_STR_FIELD_FUNC(paper, display)
PUT_INT_FIELD_FUNC(layout, x)
PUT_INT_FIELD_FUNC(layout, y)
PUT_INT_FIELD_FUNC(layout, tx)
PUT_INT_FIELD_FUNC(layout, ty)
PUT_INT_FIELD_FUNC(layout, gx)
PUT_INT_FIELD_FUNC(layout, gy)
PUT_INT_FIELD_FUNC(layout, override)
GET_INT_FIELD_FUNC(layout, x)
GET_INT_FIELD_FUNC(layout, y)
GET_INT_FIELD_FUNC(layout, tx)
GET_INT_FIELD_FUNC(layout, ty)
GET_INT_FIELD_FUNC(layout, gx)
GET_INT_FIELD_FUNC(layout, gy)
GET_INT_FIELD_FUNC(layout, override)
GET_INT_FIELD_FUNC(layout, width)
GET_INT_FIELD_FUNC(layout, height)

     //PUT_GRAPHIC(layout)

PUT_STR_FIELD_FUNC(layout, prefix)
PUT_STR_FIELD_FUNC(layout, postfix)
PUT_STR_FIELD_FUNC(layout, display)

GET_STR_FIELD_FUNC(layout, prefix)
GET_STR_FIELD_FUNC(layout, postfix)
GET_STR_FIELD_FUNC(layout, display)



PUT_INT_FIELD_FUNC(movementcontrol, x)
PUT_INT_FIELD_FUNC(movementcontrol, y)
PUT_INT_FIELD_FUNC(movementcontrol, tx)
PUT_INT_FIELD_FUNC(movementcontrol, ty)
PUT_INT_FIELD_FUNC(movementcontrol, gx)
PUT_INT_FIELD_FUNC(movementcontrol, gy)
PUT_INT_FIELD_FUNC(movementcontrol, override)
GET_INT_FIELD_FUNC(movementcontrol, x)
GET_INT_FIELD_FUNC(movementcontrol, y)
GET_INT_FIELD_FUNC(movementcontrol, tx)
GET_INT_FIELD_FUNC(movementcontrol, ty)
GET_INT_FIELD_FUNC(movementcontrol, gx)
GET_INT_FIELD_FUNC(movementcontrol, gy)
GET_INT_FIELD_FUNC(movementcontrol, override)
GET_INT_FIELD_FUNC(movementcontrol, width)
GET_INT_FIELD_FUNC(movementcontrol, height)

     //PUT_GRAPHIC(movementcontrol)

PUT_STR_FIELD_FUNC(movementcontrol, prefix)
PUT_STR_FIELD_FUNC(movementcontrol, postfix)
PUT_STR_FIELD_FUNC(movementcontrol, display)

GET_STR_FIELD_FUNC(movementcontrol, prefix)
GET_STR_FIELD_FUNC(movementcontrol, postfix)
GET_STR_FIELD_FUNC(movementcontrol, display)



#undef STANDALONE_PUT_INT_FIELD_FUNC
#undef PUT_GRAPHIC
#undef PUT_INT_FIELD_FUNC
#undef GET_INT_FIELD_FUNC
#undef PUT_STR_FIELD_FUNC
#undef GET_STR_FIELD_FUNC



gchar * get_scoretitle(void){
  gchar *scoretitle = NULL;
  GList *first = Denemo.gui->movements;
  if(first) {
    DenemoScore *si = (DenemoScore *)first->data;
    if(si){
       DenemoDirective *directive = find_directive(si->header.directives, "Movement-title");
       if(directive && directive->display)
	 scoretitle = directive->display->str;
    }
  }
  return scoretitle;
}


#define TEXT_EDIT_DIRECTIVE(what)\
gboolean text_edit_##what##_directive(gchar *tag) {\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive)\
    return text_edit_directive(directive, #what);\
  return FALSE;\
}
TEXT_EDIT_DIRECTIVE(note);
TEXT_EDIT_DIRECTIVE(chord);
TEXT_EDIT_DIRECTIVE(staff);
TEXT_EDIT_DIRECTIVE(voice);
TEXT_EDIT_DIRECTIVE(score);
TEXT_EDIT_DIRECTIVE(clef);
TEXT_EDIT_DIRECTIVE(timesig);
TEXT_EDIT_DIRECTIVE(keysig);
TEXT_EDIT_DIRECTIVE(scoreheader);
TEXT_EDIT_DIRECTIVE(header);
TEXT_EDIT_DIRECTIVE(paper);
TEXT_EDIT_DIRECTIVE(layout);
TEXT_EDIT_DIRECTIVE(movementcontrol);
#undef TEXT_EDIT_DIRECTIVE
