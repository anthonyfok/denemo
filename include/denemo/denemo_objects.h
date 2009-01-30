/**
 * denemo_objects.h
 *
 * Description: Contains definitions for denemo data structures
 *
 *
 * Author: Adam Tee <adam@ajtee.plus.com>, (C) 2005
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */
 
#include <gtk/gtk.h>

#ifndef DENEMOOBJECTS_H
#define DENEMOOBJECTS_H



/**
 * enum containing notehead 
 * definitions
 */
typedef enum headtype
{ 
  DENEMO_NORMAL_NOTEHEAD, /*!< Enum value DENEMO_NORMAL_NOTEHEAD */ 
  DENEMO_CROSS_NOTEHEAD,
  DENEMO_HARMONIC_NOTEHEAD,
  DENEMO_DIAMOND_NOTEHEAD
}headtype;

/**
 * structure encapsulating a 
 * grace note
 */
typedef struct grace
{
  gboolean on_beat;
  gint duration;
}grace;
  
  
/**
 * structure encapsulating a 
 * musical note
 */
typedef struct note
{
  gint mid_c_offset;/**< This is used to define (the pitch of) a note. 
		       A positive number is the number of half 
		       steps above middle c. A negative number is below middle c.*/
  gint enshift;/**< Enharmonic shift. Should the note be notated as sharp (1) double-sharp (2) or flat (-1) double-flat (-2) or natural (0). No other values are legal. */
  gboolean reversealign;
  gboolean showaccidental;/**< This tells denemo to show all possible accidentals?? cautionary accidental?? */
  gint position_of_accidental; /**< Holds number of pixels to the left of the notehead that the
  				   accidental should be drawn at.  */
  gint y; /**< Holds y co-ordinate for drawing rather than recalculating it each
  		   run through the drawing loop. see calculateheight  */
  enum headtype noteheadtype; /**< Holds note head type.  */

  GString *prefix;  /**< LilyPond text to be inserted before the note */
  GString *postfix; /**< LilyPond text to be inserted after the note */
  GString *display; /**< something to display to describe the LilyPond attached to the note */
}
note;

/**
 * Enum defining ornament types
 * 
 */
typedef enum ornament {
  STACCATO=1,
  D_ACCENT,
  CODA,
  FERMATA,
  TENUTO,
  TRILL,
  TURN,
  REVERSETURN,
  MORDENT,
  STACCATISSIMO,
  MARCATO,
  UBOW,
  DBOW,
  RHEEL,
  LHEEL,
  RTOE,
  LTOE,
  FLAGEOLET,
  OPEN,
  PRALLMORDENT,
  PRALL,
  PRALLPRALL,
  SEGNO,
  SFORZATO,/*unused is a dynamic */
  STOPPED,
  THUMB,
  TRILL_ELEMENT,
  TRILLELEMENT,
  UPPRALL,
  D_ARPEGGIO
}Ornament;
 

/**
 * Structure describing a chord
 * 1;7B
 * 
 */
typedef struct chord
{
  GList *notes;	/**< NULL if the chord is a rest 
		   else Glist of the notes of the chord 
		   (in order of mid_c_offset value) 
		   notes->data fields are of type note*
		*/
  GList *dynamics;  /**< test for new dynamic handling */
  gboolean has_dynamic;
  gint highestpitch;
  gint lowestpitch;
  gint highesty;
  gint lowesty;
  gint baseduration; /**< Duration of chord in ticks */
  gint numdots; /**< Number of dots that are applied to the note */
  gint sum_mid_c_offset;
  gint numnotes;
  gboolean chordize;/**< TRUE if this chord is to be treated as a multi-note chord even if it only has one note */

  gboolean is_tied;
  gboolean is_stemup;
  gboolean is_reversealigned;
  gboolean slur_begin_p; /**< Is this note a beginning of a slur? */
  gboolean slur_end_p; /**< Is this note an end of a slur? */
  gboolean crescendo_begin_p;
  gboolean crescendo_end_p;
  gboolean diminuendo_begin_p;
  gboolean diminuendo_end_p;
  gboolean hasanacc;
  gboolean is_grace;  /**< Flag to show whether note is a grace note */
  gboolean struck_through; /**< Flag for a struck through stem*/
  gint stemy;		/**< Stores the y position of the end of the stem */
  GList *ornamentlist;
  gboolean is_highlighted; /**< If set true changes the notes color */
  GString *lyric; /**< Pointer to the lyrics applied to that chord */
  gboolean is_syllable; /**< Is the lyric a syllable? */
  gboolean center_lyric; /**< Should the lyrics be centered or 
			    should it be directly under the note?
			    that it is attatched to? */

  gboolean is_figure; /**< the reason for using this boolean is to exploit 
			 the fact that all the spacing and printing of 
			 figures can use the code for the CHORD type */
  gpointer figure; /**< when this chord is a bass note 
		      (figure !=NULL && is_figure==FALSE) this
		      pointer points to an objnode in a FiguredBassStaff.
		      That objnode's data is a DenemoObject of type CHORD.
		      It is linked into the corresponding FiguredBassStaff if
		      one exists.
		      When this chord is_figure then figure is a 
		      GString* containing the
		      figures in lilypond format. */
  GList *tone_node; /**< which tone this note was extracted from */
  gboolean is_fakechord; /**< This is the actual value of the fake chord if is_fakechord */
  gpointer fakechord; /**< This is the actual value of the fake chord if is_fakechord */
  gpointer fakechord_extension; /**< This is the extension to the fake chord. It may be a 7, sus4, 9, 11, 13 etc... */
  GString *prefix; /**< LilyPond text to be inserted before the chord */
  GString *postfix;/**< LilyPond text to be inserted after the chord */
  GString *display; /**< something to display to describe the LilyPond attached to the chord */
}
chord;

/**
 * Structure defining an indicator that a tuplet is starting 
 */
typedef struct tupopen
{
  gint numerator;
  gint denominator;
}
tupopen;

/**
 * Enum defining clef values
 * 
 */
typedef enum clefs
{
  DENEMO_TREBLE_CLEF,
  DENEMO_BASS_CLEF,
  DENEMO_ALTO_CLEF,
  DENEMO_G_8_CLEF,
  DENEMO_TENOR_CLEF,
  DENEMO_SOPRANO_CLEF,
  DENEMO_INVALID_CLEF
}clefs;

/**
 * Indicator for a clef change 
 */
typedef struct clef
{
  enum clefs type;
}
clef;

/**
 * Indicator for a time-signature change. Only appears at the
 * beginning of a measure 
 */
typedef struct timesig
{
  gint time1; /**< This is the numerator for a time signature */
  gint time2; /**< This is the denominator for a time signature */
}
timesig;

/** 
 * Indicator for a key-signature change. 
 */
typedef struct keysig
{
  gint number; /**< key number -7 to 7 for major/minor 0 to 40 for mode */
  gint isminor; /**< Type of key 0-major 1-minor 2-mode */
  gint mode;  /**< Mode indicator */
  gint accs[7];
}
keysig;

/* Dynamic */

typedef struct dynamic 
{
  GString *type;
}
dynamic;

/**
 * Enum defining barline types
 * 
 */
typedef enum barline_type {
  ORDINARY_BARLINE,
  DOUBLE_BARLINE, /**< Double Bar */
  END_BARLINE,
  OPENREPEAT_BARLINE,
  CLOSE_REPEAT,
  OPEN_CLOSE_REPEAT
} barline_type;

/**
 * Structure encapsulating a barline
 * 
 */
typedef struct barline
{
  barline_type type;
}
barline;



/**
 * Lyric datastructure 
 */
typedef struct lyric
{
  GString *lyrics;  /**< This is the text string containing the lyrics */
  gint position;
  gboolean is_syllable; /**< Is the lyric a syllable? */
  gboolean center_lyric; /**< Should the lyrics be centered? */
}lyric;


/* Data structure for an arbitrary directive to be passed to
 * Lilypond that Denemo doesn't understand. This type is useful
 * in that the user can insert such directives into a LilyPond file
 * by hand and have Denemo respect them when it loads a file and
 * write them back when it saves it */

typedef struct lilydirective
{
  GString *directive;/**< the LilyPond text */
  gboolean locked;/**< If true the directive cannot be deleted easily */
  GString *display;/**< Something for Denemo to display (to indicate what the directive is doing*/
  gint x;/**< horizontal offset of display text */
  gint y;/**< vertical offset of display text */
}
lilydirective;


/**
 * Enum defining stem direction values
 * 
 */
typedef enum stemdirections
{
  DENEMO_STEMDOWN=1,
  DENEMO_STEMBOTH,
  DENEMO_STEMUP
}stemdirections;

/**
 * Indicator that the following music should be all stemup, all
 * stemdown, or stemmed normally 
 */
typedef struct stemdirective
{
  enum stemdirections type;
}
stemdirective;


/**
 * a note and duration (e.g. obtained by pitch recognition)
 * plus field to indicate if the tone is spurious
 */
typedef struct tone
{
  gint duration;
  gint step;
  gint octave;
  gint enshift;
  gboolean valid;
}
tone;

#endif
