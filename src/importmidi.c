/*importmidi.c
 * midi file import functions
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 2003-2005 AJAnderson
 *
 * 	TODO
 *
 *  multi voice support
 *  lyrics 
 *  triplet support
 *
 */

#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <denemo/denemo.h>
#include "importmidi.h"
#include "staffops.h"
#include "smf.h"
#include "view.h"
#include "utils.h"
#include "file.h"
#include "commandfuncs.h"
#include "processstaffname.h"

#define TEXT			0x01
#define COPYRIGHT		0X02
#define META_TRACK_NAME         0x03
#define META_INSTR_NAME		0x04

#define META_TEMPO              0x51
#define META_TIMESIG            0x58
#define META_KEYSIG             0x59
#define NOTE_OFF                0x80
#define NOTE_ON                 0x90
#define AFTERTOUCH		0xA0
#define CTRL_CHANGE             0xB0
#define PGM_CHANGE              0xC0
#define CHNL_PRESSURE		0xD0
#define PCH_WHEEL		0xE0
#define SYS_EXCLUSIVE_MESSAGE1  0xF0
#define META_EVENT              0xFF

typedef struct notetype
{
	gint notetype;
	gint numofdots;
}notetype;

typedef struct harmonic
{
	gint pitch;
	gint enshift;
}harmonic;

gint PPQN;

static void
note_from_int(char *buf, int note_number)
{
	int note, octave;
	char *names[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

	octave = note_number / 12 - 1;
	note = note_number % 12;

	sprintf(buf, "%s%d", names[note], octave);
}

static harmonic
enharmonic (gint input, gint key)
{
  harmonic local;
  local.pitch = (input / 12) - 5;
  local.enshift = input % 12;
  
  switch (local.enshift)
    {
    case 0:			//c
      {
	local.pitch = (key > 6) ? (-1 + local.pitch * 7) : (local.pitch * 7);
	local.enshift = (key > 6) ? (1) : (0);
	break;
      }
    case 1:			//c#
      {
	local.pitch = (key < -3) ? (1 + local.pitch * 7) : (local.pitch * 7);
	local.enshift = (key < -3) ? (-1) : (1);
	break;
      }
    case 2:			//D
      {
	local.pitch = 1 + local.pitch * 7;
	local.enshift = 0;
	break;
      }
    case 3:			//D#
      {
	local.pitch =
	  (key < -1) ? (2 + local.pitch * 7) : (1 + local.pitch * 7);
	local.enshift = (key < -1) ? (-1) : (1);
	break;
      }
    case 4:			//E
      {
	local.pitch =
	  (key < -6) ? (3 + local.pitch * 7) : (2 + local.pitch * 7);
	local.enshift = (key < -6) ? (-1) : (0);
	break;
      }
    case 5:			//F
      {
	local.pitch =
	  (key > 5) ? (2 + local.pitch * 7) : (3 + local.pitch * 7);
	local.enshift = (key > 5) ? (1) : (0);
	break;
      }
    case 6:			//F#
      {
	local.pitch =
	  (key < -4) ? (4 + local.pitch * 7) : (3 + local.pitch * 7);
	local.enshift = (key < -4) ? (-1) : (1);
	break;
      }
    case 7:			//G
      {
	local.pitch = 4 + local.pitch * 7;
	local.enshift = 0;
	break;
      }
    case 8:			//G#
      {
	local.pitch =
	  (key < -2) ? (5 + local.pitch * 7) : (4 + local.pitch * 7);
	local.enshift = (key < -2) ? (-1) : (1);
	break;
      }
    case 9:			//A
      {
	local.pitch = 5 + local.pitch * 7;
	local.enshift = 0;
	break;
      }
    case 10:			//A#
      {
	local.pitch =
	  (key < 0) ? (6 + local.pitch * 7) : (5 + local.pitch * 7);
	local.enshift = (key < 0) ? (-1) : (1);
	break;
      }
    case 11:			//B
      {
	local.pitch =
	  (key < -5) ? (7 + local.pitch * 7) : (6 + local.pitch * 7);
	local.enshift = (key < -5) ? (-1) : (0);
	break;
      }
    };
  return local;
}

static notetype 
ConvertLength(gint duration){
 /*convert length to 2 = quarter, 1 = half, 0 = whole etc...... */
	/* quarter = 384, half = 768, whole = 1536*/
  notetype gnotetype;
  gint notetype = 0;
  gint numofdots = 0;
  gint leftover = 0;
  gint dsq = (4 * PPQN);
  g_debug("\nDuration = %d ticks\n", duration);
  
  while ((dsq >> notetype) > duration)
	  notetype++;
	  
  leftover = duration - (dsq >> notetype); 

  while (leftover >= (dsq >> (notetype +1))){
  	leftover -= (dsq >> (notetype +1));
	numofdots++;
  }
  
  gnotetype.notetype = notetype;
  gnotetype.numofdots = numofdots;
  return gnotetype;
}

/**
 * Insert time signature into current staff 
 *
 */
static void
dotimesig (gint numerator, gint denominator)
{
  DenemoGUI *gui = Denemo.gui;
  /*only does initial TS */
  DenemoStaff *curstaffstruct = (DenemoStaff *) gui->si->currentstaff->data;

  curstaffstruct->timesig.time1 = numerator;
  curstaffstruct->timesig.time2 = denominator;
}

/**
 * Insert key signature into the current staff
 *
 */
static void
dokeysig (gint isminor, gint key)
{
  DenemoGUI *gui = Denemo.gui;
  if (key > 7)
    key = key - 256;		/*get flat key num, see keysigdialog.cpp */
#ifdef DEBUG
  g_print("\nkey = %d\n", key); 
#endif
  DenemoStaff *curstaffstruct = (DenemoStaff *) gui->si->currentstaff->data;
  curstaffstruct->keysig.number = key;
  curstaffstruct->keysig.isminor = isminor;
  dnm_setinitialkeysig (curstaffstruct, key, isminor);
}

static void
dotempo (gint tempo)
{ 
  DenemoGUI *gui = Denemo.gui;
  gui->si->tempo = (gint) (6.0e7 / (double) tempo);
}

static void
dotrackname (gchar *name)
{
  DenemoGUI *gui = Denemo.gui;
  DenemoStaff *curstaffstruct = (DenemoStaff *) gui->si->currentstaff->data;
  
  curstaffstruct->denemo_name->str = g_strdup(name);
}

static void
doinstrname (gchar* name)
{
  DenemoGUI *gui = Denemo.gui;
  DenemoStaff *curstaffstruct = (DenemoStaff *) gui->si->currentstaff->data;

  curstaffstruct->midi_instrument->str = g_strdup(name);
}

static void
insert_rest_into_score(notetype length)
{
  DenemoGUI *gui = Denemo.gui;
  gint i;

  switch (length.notetype)
  {
     case 0:
       insert_rest_0key (gui);
       break;
     case 1:
       insert_rest_1key (gui);
       break;
     case 2:
       insert_rest_2key (gui);
       break;
     case 3:
       insert_rest_3key (gui);
       break;
     case 4:
       insert_rest_4key (gui);
       break;
     case 5:
       insert_rest_5key (gui);
       break;
     case 6:
       insert_rest_6key (gui);
       break;
     default:
       insert_rest_2key (gui);
       break;
  }
  displayhelper (gui);

  /* add dots */
  for (i=0;i<length.numofdots;i++)
    add_dot_key (gui);
}

static void 
AddRest(gint duration)
{
  gint rest;
  gint ticks;
  if (duration == 0)
    return;
  rest = duration;

  while (rest){
    notetype length = ConvertLength(rest);
    insert_rest_into_score(length);
    ticks = ConvertNoteType2ticks(&length);
    rest -= ticks;
  }
}

static void
insert_note_into_score(gint pitch, notetype length)
{
  DenemoGUI *gui = Denemo.gui;
  DenemoStaff *curstaffstruct = (DenemoStaff *) gui->si->currentstaff->data;
  gint i;

  /* 0-8 accepted bellow */
  DenemoScriptParam param;
  switch(length.notetype) {
  case 0:
      InsertDur0(NULL, &param);
      break;
  case 1:
      InsertDur1(NULL, &param);
      break;
  case 2:
      InsertDur2(NULL, &param);
      break;
  case 3:
      InsertDur3(NULL, &param);
      break;
  case 4:
      InsertDur4(NULL, &param);
      break;
  case 5:
      InsertDur5(NULL, &param);
      break;
  case 6:
      InsertDur6(NULL, &param);
      break;
  case 7:
      InsertDur7(NULL, &param);
      break;
  default:
      InsertDur2(NULL, &param);
      g_warning("Cannot handle size %d",length.notetype);
      break;
  }
#if 0 
  g_debug("DenemoScriptParam = %d",param.status);
  /* get correct note name */
  gint key = curstaffstruct->keysig.number;
  harmonic enote = enharmonic (pitch, (gint) key);
  gchar *name =  mid_c_offsettolily (enote.pitch, enote.enshift);
  
  /* Rename note to the correct note */
  gchar *accidental = g_strdup_printf("(d-ChangeChordNotes \"%s\")", name);
  call_out_to_guile(accidental);
  g_free(accidental);
#endif
  /* Add dots */
  for (i=0;i<length.numofdots;i++)
    add_dot_key (gui);
}

static void
AddNote(gint pitch, gint duration)
{
  if (duration == 0)
    return;
  notetype length = ConvertLength(duration);
  insert_note_into_score(pitch, length);
}

static smf_t*
cmd_load(gchar *file_name)
{
  smf_t *smf;
	
  smf = smf_load(file_name);
  if (smf == NULL) {
    g_critical("Couldn't load '%s'.", file_name);
    return NULL;
  }
  PPQN = smf->ppqn;
  g_message("File '%s' loaded.", file_name);
  g_message("%s.", smf_decode(smf));

  return smf;
}

#define BUFFER_SIZE 1024

void
decode_metadata(const smf_event_t *event)
{
	int off = 0, mspqn, flats, isminor;
	char *buf;

	static const char *const major_keys[] = {"Fb", "Cb", "Gb", "Db", "Ab",
		"Eb", "Bb", "F", "C", "G", "D", "A", "E", "B", "F#", "C#", "G#"};

	static const char *const minor_keys[] = {"Dbm", "Abm", "Ebm", "Bbm", "Fm",
		"Cm", "Gm", "Dm", "Am", "Em", "Bm", "F#m", "C#m", "G#m", "D#m", "A#m", "E#m"};

	//assert(smf_event_is_metadata(event));

	switch (event->midi_buffer[1]) {
		case TEXT:
			//return smf_event_decode_textual(event, "Text");

		case COPYRIGHT:
			//return smf_event_decode_textual(event, "Copyright");

		case META_TRACK_NAME:
			//return smf_event_decode_textual(event, "Sequence/Track Name");
			dotrackname(smf_event_extract_text(event));

		case META_INSTR_NAME:
			//printf("\nInstrument text = %s\n", smf_string_from_event(event));
			doinstrname(smf_event_extract_text(event));
		case 0x05:
			//return smf_event_decode_textual(event, "Lyric");

		case 0x06:
			//return smf_event_decode_textual(event, "Marker");

		case 0x07:
			//return smf_event_decode_textual(event, "Cue Point");

		case 0x08:
			//return smf_event_decode_textual(event, "Program Name");

		case 0x09:
			//return smf_event_decode_textual(event, "Device (Port) Name");

		default:
			break;
	}

	buf = malloc(BUFFER_SIZE);
	if (buf == NULL) {
		g_critical("smf_event_decode_metadata: malloc failed.");
	}

	switch (event->midi_buffer[1]) {
		case 0x00:
			off += snprintf(buf + off, BUFFER_SIZE - off, "Sequence number");
			break;

		/* http://music.columbia.edu/pipermail/music-dsp/2004-August/061196.html */
		case 0x20:
			if (event->midi_buffer_length < 4) {
				g_critical("smf_event_decode_metadata: truncated MIDI message.");
				goto error;
			}

			off += snprintf(buf + off, BUFFER_SIZE - off, "Channel Prefix: %d.", event->midi_buffer[3]);
			break;

		case 0x21:
			if (event->midi_buffer_length < 4) {
				g_critical("smf_event_decode_metadata: truncated MIDI message.");
				goto error;
			}

			off += snprintf(buf + off, BUFFER_SIZE - off, "Midi Port: %d.", event->midi_buffer[3]);
			break;

		case 0x2F:
			off += snprintf(buf + off, BUFFER_SIZE - off, "End Of Track");
			break;

		case META_TEMPO:
			if (event->midi_buffer_length < 6) {
				g_critical("smf_event_decode_metadata: truncated MIDI message.");
				goto error;
			}

			mspqn = (event->midi_buffer[3] << 16) + (event->midi_buffer[4] << 8) + event->midi_buffer[5];
		        
			dotempo(mspqn);

			break;

		case 0x54:
			off += snprintf(buf + off, BUFFER_SIZE - off, "SMPTE Offset");
			break;

		case META_TIMESIG:
			if (event->midi_buffer_length < 7) {
				g_critical("smf_event_decode_metadata: truncated MIDI message.");
				goto error;
			}

			dotimesig(event->midi_buffer[3], (int)pow(2, event->midi_buffer[4]));
			break;

		case META_KEYSIG:
			if (event->midi_buffer_length < 5) {
				g_critical("smf_event_decode_metadata: truncated MIDI message.");
				goto error;
			}

			flats = event->midi_buffer[3];
			isminor = event->midi_buffer[4];

			if (isminor != 0 && isminor != 1) {
				g_critical("smf_event_decode_metadata: last byte of the Key Signature event has invalid value %d.", isminor);
				goto error;
			}

			dokeysig(isminor, flats);
			break;

		case 0x7F:
			off += snprintf(buf + off, BUFFER_SIZE - off, "Proprietary (aka Sequencer) Event, length %d",
				event->midi_buffer_length);
			break;

		default:
			goto error;
	}

error:
	free(buf);
}

/** 
 * extremely simple quantizer that rounds 
 * to the closest granule size
 */
static gint
round2granule(gint tick)
{
  gint smallestgrain = 48; 
  gdouble div = ((gdouble) tick / (gdouble) smallestgrain);
  return smallestgrain * (gint) round(div);
}

/**
 * Process note off command
 */
void
donoteoff (const smf_event_t *event)
{
  gint duration;
  
  duration = event->delta_time_pulses;
  duration = round2granule(duration);
  AddNote(event->midi_buffer[1], duration);
}

/**
 * Process note on command 
 */
void
donoteon (const smf_event_t *event)
{
  if (event->delta_time_pulses)
    AddRest(round2granule(event->delta_time_pulses));
}

void 
decode_midi_event(const smf_event_t *event)
{
	gint off = 0, channel;
	gchar note[5];

	/* + 1, because user-visible channels used to be in range <1-16>. */
	channel = (event->midi_buffer[0] & 0x0F) + 1;

	switch (event->midi_buffer[0] & SYS_EXCLUSIVE_MESSAGE1) {
		case NOTE_OFF:
			note_from_int(note, event->midi_buffer[1]);
			g_debug("\nNote Off channel %d note %s velocity %d\n", 
					channel, note, event->midi_buffer[2]);
			donoteoff (event);
			break;

		case NOTE_ON:
			note_from_int(note, event->midi_buffer[1]);
			g_debug("\nNote On channel %d note %s velocity %d\n", 
					channel, note, event->midi_buffer[2]);

		    	if (event->midi_buffer[2])
		      	  donoteon(event);
		    	else
		          donoteoff(event);
			break;

		case AFTERTOUCH:
			note_from_int(note, event->midi_buffer[1]);
			g_debug("\nAftertouch channel %d note %s velocity %d\n", 
					channel, note, event->midi_buffer[2]);
			break;

		case CTRL_CHANGE:
			g_debug("\nController channel %d controller %d value %d\n", 
					channel, event->midi_buffer[1], event->midi_buffer[2]);
			break;

		case PGM_CHANGE:
			g_debug("\nProgram Change channel %d controller %d\n", 
					channel, event->midi_buffer[1]);
			break;

		case CHNL_PRESSURE:
			g_debug("\nChannel Pressure channel %d pressure %d\n", 
					channel, event->midi_buffer[1]);
			break;

		case PCH_WHEEL:
			g_debug("\nPitch Wheel channel %d value %d\n", 
					channel, ((int)event->midi_buffer[2] << 7) | (int)event->midi_buffer[2]);
			break;

		default:
			break;
	}
}

static int
process_midi(smf_event_t *event)
{
  if (smf_event_is_metadata(event))
    decode_metadata(event);
  else
    decode_midi_event(event);
		
  return 0;
}

gint
process_track(smf_track_t *track) 
{
  smf_event_t *event;

  while (event = smf_track_get_next_event(track)) 
    process_midi(event); 
}

void AddStaff(){
  call_out_to_guile("(d-AddAfter)");
}

static gint
readtrack(smf_t *smf)
{
  smf_event_t *event;
  smf_track_t *selected_track;
  gint track;

  smf_rewind(smf); 
       
  for (track=1;track <= smf->number_of_tracks;track++){	
    selected_track = smf_get_track_by_number(smf, track);
    process_track(selected_track);
    if (track+1 <= smf->number_of_tracks)
      AddStaff();
  }
  return 0;
}

gint
ConvertNoteType2ticks(notetype *gnotetype){
  gint ticks;
  gint notetype = (int) gnotetype->notetype;
  gint numofdots = (int) gnotetype->numofdots;
  gint dsq = (4 * PPQN);
  gint i = 0;

  ticks = dsq >> notetype;
  while (i++ < numofdots) 
    ticks += dsq >> (notetype + 1);

  return ticks;
}

gint
importMidi (gchar *filename, DenemoGUI *gui)
{
  smf_t *smf;
  gint ret = 0;	// (-1 on failure)

  /* delete old data in the score */
  deletescore (NULL, gui);
  /* load the file */
  smf = cmd_load(filename);
  if(!smf)
    return -1;
  /* Read Track Data */ 
  if (readtrack(smf))
    return -1;
  /* Delete data...Does it free it? */
  smf_delete(smf);
  return ret;
}
