/* drawingprims.h
 * header file for the drawing primitive .c files,
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 1999-2005 Matthew Hiller
 */


#include <denemo/denemo.h> 



void
draw_rest (cairo_t *cr, gint baseduration, gint numdots,
	   gint xx, gint y);

void
draw_notehead (cairo_t *cr,
	       note *thenote, gint baseduration,
	       gint numdots, gint xx, gint y, gint *accs, gint is_stemup);

void
draw_ledgers (cairo_t *cr,
	      gint greaterheight, gint lesserheight,
	      gint xx, gint y, gint width);

void
draw_chord (cairo_t * cr,
	    objnode *curobj, gint xx, gint y, gint mwidth, gint *accs, gboolean selected);
void
draw_tuplet (cairo_t *cr,
	     objnode *curobj, gint xx, gint y, gint mwidth, gint *accs);

void
draw_clef (cairo_t *cr, gint xx, gint y, clef* type);

gint
draw_key (cairo_t *cr, gint xx, gint y,
	  gint number, gint prevnumber, gint dclef, gint wetrun);

void
draw_timesig (cairo_t *cr,
	      gint xx, gint y, gint time1, gint time2);

void
draw_tupbracket (cairo_t *cr,
		 gint xx, gint y, DenemoObject *theobj);

void
draw_cursor (cairo_t *cr, DenemoScore *si,
	     gint xx, gint y, gint last_gap, input_mode mode, gint dclef);

void
draw_accidental (cairo_t *cr,
		 gint xx, gint aggheight, gint enshift);

void
draw_selection (cairo_t *cr, gint x1, gint y1,
		gint x2, gint y2);

void
draw_stem_directive (cairo_t *cr,
		     gint xx, gint y, DenemoObject * theobj);

void
draw_dynamic(cairo_t *cr,
	      gint xx, gint y, DenemoObject *theobj);

void
draw_lily_dir(cairo_t *cr,
	      gint xx, gint y, gint highy, gint lowy, DenemoObject *theobj, gboolean selected);

void
draw_gracebracket (cairo_t *cr,
		   gint xx, gint y, DenemoObject *theobj);


gint calc_offset(chord thechord, gint stemdir);
void draw_articulations(cairo_t *cr,
			chord thechord, gint xx, gint y);

void draw_lyric(cairo_t *cr,
                gint xx, gint y, gchar *syllable);

void draw_figure(cairo_t *cr,
                        gint xx, gint y, DenemoObject *theobj);

void draw_fakechord(cairo_t *cr,
                        gint xx, gint y, DenemoObject *theobj);

void
drawbarline(cairo_t *cr,gint xx, gint top_y,gint y, gint type);
