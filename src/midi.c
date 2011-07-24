/*
 * midi.c
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * Copyright (C) 2000-2005 Brian Delaney
 * Copyright (C) 2011  Dominic Sacré
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <denemo/denemo.h>
#include "midi.h"
#include "audiobackend.h"
#include "smf.h"
#include "exportmidi.h"
#include "draw.h"
#include "view.h"

#include <glib.h>
#include <math.h>
#include <string.h>
#include <assert.h>


static void initialize_clock();


static volatile gboolean playing = FALSE;

static double last_draw_time;

// huh?
static gboolean midi_capture_on = FALSE;//any midi events not caught by midi_divert will be dropped if this is true

static  gdouble playalong_time = 0.0;


void update_position(smf_event_t *event) {
  DenemoScore *si = Denemo.gui->si;

  if (event) {
    if ((event->midi_buffer[0] & 0xf0) == MIDI_NOTE_ON &&
        event->time_seconds - last_draw_time > Denemo.prefs.display_refresh) {
      last_draw_time = event->time_seconds;
      queue_redraw_playhead(event);
    }
  } else {
    si->playingnow = NULL;
    si->playhead = 0;
    queue_redraw_all();
  }
}



void start_playing() {
  smf_t *smf = Denemo.gui->si->smf;

  smf_rewind(smf);

  int r = smf_seek_to_seconds(smf, Denemo.gui->si->start_time);

  initialize_clock();

  initialize_playhead();

  playing = TRUE;
  last_draw_time = 0.0;
}


void stop_playing() {
  update_position(NULL);
  playing = FALSE;
}


gboolean is_playing() {
  return playing;
}


double get_start_time() {
  if (Denemo.gui && Denemo.gui->si) {
    return Denemo.gui->si->start_time;
  } else {
    return 0.0;
  }
}


double get_end_time() {
  if (Denemo.gui && Denemo.gui->si) {
    return Denemo.gui->si->end_time;
  } else {
    return 0.0;
  }
}


smf_event_t *get_smf_event(double until_time) {
  smf_t *smf = Denemo.gui->si->smf;

  if (until_time > Denemo.gui->si->end_time) {
    until_time = Denemo.gui->si->end_time;
  }

  for (;;) {
    smf_event_t *event = smf_peek_next_event(smf);

    if (event == NULL || event->time_seconds >= until_time) {
      return NULL;
    }

    if (smf_event_is_metadata(event)) {
      // consume metadata event and continue with the next one
      event = smf_get_next_event(smf);
      continue;
    }

    // consume the event
    event = smf_get_next_event(smf);

    assert(event->midi_buffer_length <= 3);

    return event;
  }
}




gdouble get_time() {
  GTimeVal tv;
  double seconds;

  g_get_current_time(&tv);

  seconds = tv.tv_sec + tv.tv_usec / 1000000.0;
  return seconds;
}


void generate_midi() {
  if((Denemo.gui->si->smf==NULL) || (Denemo.gui->si->smfsync!=Denemo.gui->si->changecount)) {
    exportmidi(NULL, Denemo.gui->si, 0, 0);
  }

  if (Denemo.gui->si->smf == NULL) {
    g_critical("Loading SMF failed.");
  }
}


/* return the time of the last event on the list events */
gdouble get_midi_off_time(GList *events) {
  smf_event_t *event = g_list_last(events)->data;
return event->time_seconds;
}

/* return the time of the first event on the list events */
gdouble get_midi_on_time(GList *events) {
  smf_event_t *event = events->data;
  return event->time_seconds;
}



DenemoObject *get_obj_for_start_time(smf_t *smf, gdouble time) {
  if(time<0.0)
      time=0.0;
  static smf_event_t *event;
  static guint smfsync;
  static DenemoScore *last_si = NULL;
  static gdouble last_time=-1.0;
  if( fabs(time-last_time)>0.001 || (last_si!=Denemo.gui->si) || (smfsync!=Denemo.gui->si->smfsync)) {
    smf_event_t *initial = smf_peek_next_event(smf);

    gdouble total = smf_get_length_seconds(smf);
    time = (time>total?total:time);
    gint error = smf_seek_to_seconds(smf, time);
    do {
      event = smf_get_next_event(smf);
    } while(event && (!(event->midi_buffer[0] & MIDI_NOTE_ON) || !event->user_pointer));
    if(initial)
      error = smf_seek_to_event(smf, initial);
    last_si = Denemo.gui->si;
    smfsync = Denemo.gui->si->smfsync;
    last_time = time;
  }
  if(event)
    return (DenemoObject *)(event->user_pointer);
  return NULL;
}

DenemoObject *get_obj_for_end_time(smf_t *smf, gdouble time) {
  if(time<0.0)
      time=0.0;
  static smf_event_t *event = NULL;
  static guint smfsync;
  static DenemoScore * last_si = NULL;
  static gdouble last_time=-1.0;
  if( fabs(time-last_time)>0.001 || (last_si!=Denemo.gui->si) || (smfsync!=Denemo.gui->si->smfsync)) {
    smf_event_t *initial = smf_peek_next_event(smf);

    gdouble total = smf_get_length_seconds(smf);
    time = (time>total?total:time);
    gint error = smf_seek_to_seconds(smf, time);
    do {
      event = smf_get_next_event(smf);
    } while(event && (!(event->midi_buffer[0] & MIDI_NOTE_OFF) || !event->user_pointer));
    if(initial)
      error = smf_seek_to_event(smf, initial);
    last_si = Denemo.gui->si;
    smfsync = Denemo.gui->si->smfsync;
    last_time = time;
  }
  if(event)
    return (DenemoObject *)(event->user_pointer);
  return NULL;
}



/**
 * action_note_into_score
  enters ( or (if mode==INPUTEDIT and appending) edits the note at the cursor)
 * the parameters specify which note
 * @mid_c_offset
 * @enshift enharmonic adjustment -1 is one flat etc.. 
 * @octave
 */
static void action_note_into_score (gint mid_c_offset, gint enshift, gint octave) {
  DenemoGUI *gui = Denemo.gui;
  gui->last_source = INPUTMIDI;
  gui->si->cursor_y = gui->si->staffletter_y = mid_c_offset;
  gui->si->cursor_y += 7*octave; 
  shiftcursor(gui, mid_c_offset);
  setenshift(gui->si, enshift);
  displayhelper (gui);
}
static void add_note_to_chord (gint mid_c_offset, gint enshift, gint octave) {
  DenemoGUI *gui = Denemo.gui;
  gui->last_source = INPUTMIDI;
  gui->si->cursor_y = gui->si->staffletter_y = mid_c_offset;
  gui->si->cursor_y += 7*octave; 
  insert_chordnote(gui);
  // shiftcursor(gui, mid_c_offset);
  setenshift(gui->si, enshift);
  displayhelper (gui);
}
typedef struct enharmonic
{
  gint mid_c_offset;
  gint enshift;
  gint octave;
}enharmonic;


//Add the passed midi to a recording in Denemo.gui->si
void record_midi(gchar *buf, gdouble time) {
  smf_event_t *event = smf_event_new_from_pointer(buf, 3);
  if(event && smf_event_is_valid(event)) {
    if(Denemo.gui->si->recorded_midi_track && ((smf_track_t *)Denemo.gui->si->recorded_midi_track)->smf ) {
      smf_track_add_event_seconds(Denemo.gui->si->recorded_midi_track, event, time);
    } else {
      smf_event_delete(event);
      gdk_beep();
    }
  }
}

static void 	      
do_one_note(gint mid_c_offset, gint enshift, gint notenum) {
  DenemoGUI *gui = Denemo.gui;
  if((Denemo.keyboard_state&ADDING_MASK) && (Denemo.keyboard_state&CHORD_MASK)) {
    
    add_note_to_chord(mid_c_offset, enshift, notenum);
  }
  else {
    DenemoObject *curobj = NULL;
    //check for non-printing notes - back up to the first non-printing note.
    gboolean non_printing_note = FALSE;
    PushPosition(NULL, NULL);
    while(cursor_to_prev_note()) {
      curobj=Denemo.gui->si->currentobject->data;
      if(!curobj->isinvisible)
	break;
      else
	non_printing_note = TRUE;
    }
    if(Denemo.gui->si->currentobject) {
        curobj=Denemo.gui->si->currentobject->data;
        if(non_printing_note) {
        if(!curobj->isinvisible)
            cursor_to_next_note();
        pop_position();
        } else 
        PopPosition(NULL, NULL);
    }   
    action_note_into_score(mid_c_offset, enshift, notenum);
    if(Denemo.keyboard_state&ADDING_MASK)
      Denemo.keyboard_state |= CHORD_MASK;
    set_midi_in_status();
  }
}


static gboolean get_current(enharmonic *enote) {
  DenemoObject *curObj=NULL;
   if(Denemo.gui->si->currentobject) {
    curObj = Denemo.gui->si->currentobject->data;
    if(curObj && curObj->type==CHORD) {
      chord *thechord = (chord *)  curObj->object;
      if(thechord->notes) {
        note *thenote = (note *) thechord->notes->data;
          enote->mid_c_offset = offsettonumber(thenote->mid_c_offset);
          enote->enshift = thenote->enshift;
          return TRUE;
      }
    }
   }
return FALSE;
}

static gboolean get_previous(enharmonic *enote) {
  DenemoObject *curObj=NULL;
   if(Denemo.gui->si->currentobject) {
     if(Denemo.gui->si->currentobject->prev) 
      curObj = Denemo.gui->si->currentobject->prev->data;
      else {
      if(Denemo.gui->si->currentmeasure->prev && Denemo.gui->si->currentmeasure->prev->data) {
        curObj = g_list_last(Denemo.gui->si->currentmeasure->prev->data)->data;
      }
    }
   }
  if(curObj && curObj->type==CHORD) {
    chord *thechord = (chord *)  curObj->object;
    if(thechord->notes) {
        note *thenote = (note *) thechord->notes->data;
        enote->mid_c_offset = offsettonumber(thenote->mid_c_offset);
        enote->enshift = thenote->enshift;
        return TRUE;
      }
    }
return FALSE;
}


/*  take an action for the passed note. Enter/edit/check the score following the mode and keyboard state. */
static gint midiaction(gint notenum) {

  DenemoGUI *gui = Denemo.gui;
  if(gui==NULL)
    return TRUE;
  if(gui->si==NULL)
    return TRUE;
  DenemoStaff *curstaffstruct = (DenemoStaff *) gui->si->currentstaff->data;
  enharmonic enote, prevenote;
  gboolean have_previous;
  //g_print("Keyboard state %x, mask %x %x %x\n", Denemo.keyboard_state, CHECKING_MASK, GDK_CONTROL_MASK, GDK_MOD2_MASK);
  notenum2enharmonic (notenum, &enote.mid_c_offset, &enote.enshift, &enote.octave);

  if(Denemo.gui->si->cursor_appending)
    have_previous = get_current(&prevenote);
  else
    have_previous = get_previous(&prevenote);
    
  if( !(Denemo.keyboard_state&CHECKING_MASK))
   stage_undo(gui->si, ACTION_STAGE_END);//undo is a queue so this is the end :)

  if((gui->mode & INPUTEDIT) || (Denemo.keyboard_state&CHECKING_MASK))
    {
      static gboolean beep = FALSE;
      gboolean is_tied = FALSE;
      gint measure = gui->si->currentmeasurenum;
      if(Denemo.gui->si->currentobject) {
	DenemoObject *curObj = Denemo.gui->si->currentobject->data;
	if(curObj->type==CHORD) {
	  do {
	    curObj = Denemo.gui->si->currentobject->data;
	    chord *thechord = (chord *)  curObj->object;
	    is_tied = thechord->is_tied;
	    
#define check_midi_note(a,b,c,d) ((a->mid_c_offset==b)&&(a->enshift==c))?playnote(a,curstaffstruct->midi_channel):gdk_beep();

	    //g_print("check %d %d %d %d %d\n", a->mid_c_offset, a->enshift, b, c, d);
	    if( (Denemo.keyboard_state&CHECKING_MASK) && thechord->notes) {
	      //later - find note nearest cursor and
	      note *thenote = (note*)thechord->notes->data;
//	      check_midi_note(thenote, enote.mid_c_offset + 7 *(enote.octave), enote.enshift, enote.octave);
	      if((!curObj->isinvisible)&&(thenote->mid_c_offset== (enote.mid_c_offset + 7 *( enote.octave)))&&(thenote->enshift==enote.enshift)) {
                // FIXME
//		playnote(thenote,curstaffstruct->midi_channel);
	      } else {
		gdk_beep();
		break;//do not move on to next note
	      }
	    }
	    else {

	      do_one_note(enote.mid_c_offset, enote.enshift, enote.octave);
		
	    }
	    if(Denemo.gui->si->cursor_appending)
	      break;
	  } while((!(Denemo.keyboard_state&ADDING_MASK)) && next_editable_note() && is_tied);
	} else {
	  if(gui->si->cursor_appending)
	    do_one_note(enote.mid_c_offset, enote.enshift, enote.octave);
	  else
	    gdk_beep();
	}
	if(gui->mode & INPUTRHYTHM) {
	  //g_print("measure was %d now %d with appending %d\n", measure, gui->si->currentmeasurenum, gui->si->cursor_appending);
	  if(!beep && (measure != gui->si->currentmeasurenum) && !gui->si->cursor_appending)
	    beep=TRUE;
	  else if(beep) signal_measure_end(), beep=FALSE;
	}
      } else {// no current object
	  do_one_note(enote.mid_c_offset, enote.enshift, enote.octave);
      }
    } else {// not INPUTEDIT    
      action_note_into_score(enote.mid_c_offset, enote.enshift, enote.octave);
  }
  if( !(Denemo.keyboard_state&CHECKING_MASK)) {
    stage_undo(gui->si, ACTION_STAGE_START);
  }
  gtk_widget_queue_draw (Denemo.scorearea);//just for advancing the cursor.
 if(!(Denemo.keyboard_state&CHECKING_MASK)) {
    if(Denemo.prefs.immediateplayback) {
      gint channel = curstaffstruct->midi_channel;
      
      if(have_previous && check_interval(enote.mid_c_offset, enote.enshift, prevenote.mid_c_offset, prevenote.enshift))
        channel = Denemo.prefs.pitchspellingchannel;
              
      // TODO
//      if (Denemo.prefs.midi_audio_output == Portaudio)
//        playpitch(midi2hz(notenum), 0.3, 0.5, 0);
//      if (Denemo.prefs.midi_audio_output == Jack)
//        jack_playpitch(notenum, 300 /*duration*/);
//      else if (Denemo.prefs.midi_audio_output == Fluidsynth)
//        fluid_playpitch(notenum, 300 /*duration*/,  channel, 0);
    }
  }

  return TRUE;
}


gboolean set_midi_capture(gboolean set) {
  gboolean ret = midi_capture_on;
  midi_capture_on = set;
  return ret;
}


#define command ((*buf)&0xF0)
#define notenumber ((*(buf+1))&0x7F)
#define velocity ((*(buf+2))&0x7F)
void
adjust_midi_velocity(gchar *buf, gint percent) {
  if(command==MIDI_NOTE_ON)
    buf[2]=127 - (gint)((127-buf[2])*percent/100.0);
} 


void process_midi_event(gchar *buf) {
  //g_print("process midi (%s) %x %x %x\n",divert_midi_event?"diverted":"straight", command, notenumber, velocity);
//  if(divert_midi_event &&  divert_midi_id==Denemo.gui->id){
//    // this is only good for one endianness - FIXME
//    *divert_midi_event = 0;//clear 4th byte
//    memcpy(divert_midi_event, buf, 3);//midi events are up to three bytes long
//    gtk_main_quit();    
//    return;//this *is* reached
//  }
#if 0
  //already done upstream
  if(command==MIDI_NOTEON && velocity==0) {//Zero velocity NOTEON is used as NOTEOFF by some MIDI controllers
    buf[0]=MIDI_NOTEOFF;
    buf[2]=128;//FIXME 127
  }
#endif  
   if (command==MIDI_CONTROL_CHANGE && (notenumber == 0x40)){
	if (velocity == 0x7F)
	  //PEDAL DOWN
	  Denemo.keyboard_state |= ADDING_MASK;
	else {
	  Denemo.keyboard_state &= ~(CHORD_MASK|ADDING_MASK);
	  next_editable_note();
	}
	set_midi_in_status();
	displayhelper(Denemo.gui);
      }


  if(midi_capture_on) {
    if(command!=MIDI_NOTE_OFF) {
      gdk_beep();
      g_warning("MIDI event dropped");
    }
  } else {
    if(command==MIDI_NOTE_ON)
      midiaction(notenumber);
    else if(command==MIDI_CONTROL_CHANGE) {
      gchar *command_name = get_midi_control_command(notenumber, velocity);
      if(command_name) {  
        execute_callback_from_name(Denemo.map, command_name);
        g_free(command_name);
      }
    } else if(command==MIDI_PITCH_BEND) {
      gchar *command_name = get_midi_pitch_bend_command((notenumber<<8) + velocity);
      if(command_name) {
        execute_callback_from_name(Denemo.map, command_name);
      g_free(command_name);
      }
    }
  }
}


static void initialize_clock() {
  if(Denemo.gui->si->currentobject ) {
    DenemoObject *obj = Denemo.gui->si->currentobject->data;
    if(obj->type==CHORD) {
      chord *thechord = obj->object;
      if(thechord->notes) {
	note *thenote = thechord->notes->data;      
	//gboolean thetime = get_time();
	//Denemo.gui->si->start_player =  thetime - obj->earliest_time;
	playalong_time = obj->latest_time; 
      }
    }
  }
}

//test if the midi event in buf is a note-on for the current note
//if so set a time delta so that a call to GET_TIME() will return get_time()- lost_time
//unless this time is greater than the start time of the next note when GET_TIME() will stick
//advance cursor to next note
static void advance_clock(gchar *buf) {
  if(Denemo.gui->si->currentobject) {
    DenemoObject *obj = Denemo.gui->si->currentobject->data;
    if(obj->type!=CHORD) 
      if(cursor_to_next_chord()) 
	obj = Denemo.gui->si->currentobject->data;
    
    if(Denemo.gui->si->currentobject && obj->type==CHORD) {
      chord *thechord = obj->object;
      if(thechord->notes) {
	note *thenote = thechord->notes->data;
	if( ((buf[0]&0xf0)==MIDI_NOTE_ON) && buf[2] && buf[1] == (dia_to_midinote (thenote->mid_c_offset) + thenote->enshift)) {
	  gdouble thetime = get_time();
	  Denemo.gui->si->start_player = thetime -  obj->earliest_time;
	  
	  if(thechord->is_tied && cursor_to_next_note()) {
	    obj = Denemo.gui->si->currentobject->data;	   
	  }
	  //playalong_time = obj->latest_time;
	  //IF THE NEXT OBJ IS A REST ADVANCE OVER IT/THEM
	  do {
	    if(!cursor_to_next_note())	//if(!cursor_to_next_chord())	   	      
	      {
		playalong_time = Denemo.gui->si->end_time + 1.0;
		break;
	      }
	    else {
	      obj = Denemo.gui->si->currentobject->data;
	      thechord = obj->object;
	      playalong_time = obj->earliest_time;
	    }
	  } 
	  while(!thechord->notes);	    
	}
      }
    } else
      g_warning("Not on a chord");
  } else
    g_warning("Not on an object");
}




#define EDITING_MASK (GDK_SHIFT_MASK)  
void handle_midi_event(gchar *buf) {
  //g_print("%x : %x %x %x %x\n", Denemo.keyboard_state, GDK_CONTROL_MASK, GDK_SHIFT_MASK, GDK_MOD1_MASK, GDK_LOCK_MASK);

  if( (Denemo.gui->midi_destination & MIDIRECORD) ||
      (Denemo.gui->midi_destination & (MIDIPLAYALONG|MIDICONDUCT))) {
    if(Denemo.gui->midi_destination & MIDIRECORD)
      record_midi(buf,  get_time() - Denemo.gui->si->start_player);
    if(Denemo.gui->midi_destination & (MIDIPLAYALONG))
      advance_clock(buf);
//    fluid_output_midi_event(buf);
    play_midi_event(DEFAULT_BACKEND, 0, buf);
  } else {
    if((Denemo.keyboard_state==(GDK_SHIFT_MASK|GDK_LOCK_MASK)) ||
       Denemo.keyboard_state==(GDK_CONTROL_MASK) ||
       Denemo.keyboard_state==(ADDING_MASK) ||
       Denemo.keyboard_state==((ADDING_MASK)|(CHORD_MASK)) ||
       Denemo.keyboard_state==(GDK_CONTROL_MASK|GDK_LOCK_MASK) ||
       (Denemo.keyboard_state==0))
      process_midi_event(buf);
    else
      if(Denemo.keyboard_state==(GDK_SHIFT_MASK) ||
	 Denemo.keyboard_state==(GDK_LOCK_MASK)) {
//	fluid_output_midi_event(buf);
        play_midi_event(DEFAULT_BACKEND, 0, buf);
        adjust_midi_velocity(buf, 100 - Denemo.prefs.dynamic_compression);
      }
  }
}


// FIXME: not quite sure what to do with these yet

gint midi_init () { return 0; }

gint init_midi_input() { return 0; }
void start_midi_input() { }
gint stop_midi_input() { return 0; }

gint get_midi_channel() { return 0; }
gint get_midi_prognum() { return 0; }

gboolean intercept_midi_event(gint *midi) { return FALSE; }

void change_tuning(gdouble *cents) { }
