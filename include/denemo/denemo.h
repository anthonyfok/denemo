/* denemo.h
 *
 * the important data structures and defines for denemo, a gtk+ frontend to
 * Lilypond, the GNU music typesetter
 *
 * (c) 1999-2005 Matthew Hiller, Adam Tee
 *
 */

#ifndef DENEMO_DATASTRUCTURES
#define DENEMO_DATASTRUCTURES
#ifdef __cplusplus
extern "C" {
#endif

/* Include the internationalization stuff.  */
#include <libintl.h>
#include <locale.h>

#define _(String) gettext (String)
#ifndef gettext_noop
# define gettext_noop(String) String
#endif
#define N_(String) gettext_noop (String)


#include <glib.h>
#include <gmodule.h>

#include "denemo_api.h"



/*Set maximum number of undos*/
#define MAX_UNDOS 50

/* This unit of memory size is used all over the place.  */
#define SEVENGINTS (sizeof (gint) * 7)

#define CHECKING_MASK (GDK_CONTROL_MASK) 
#define ADDING_MASK (GDK_MOD1_MASK)  //Alt
#define CHORD_MASK (1<<25)  //Not used by GDK

#define DENEMO_INITIAL_MEASURE_WIDTH 160
#define DENEMO_INITIAL_STAFF_HEIGHT 100

#define DENEMO_FILE_SUFFIX  ".denemo"
#define XML_EXT             ".xml"
#define SCM_EXT             ".scm"
#define LILYPOND_EXT        ".ly"
#define KEYMAP_EXT          ".commands"

#define SCHEME_INIT         "denemo.scm"
#define DENEMO_NO_COMMAND (-1) /* command id for no command */

#define g_malloc(a) g_malloc0(a) /* for extensible debuggable code */
#if 0
  //use this to detect bad access to G_OBJECT
#define g_object_set_data(a,b,c) (G_IS_OBJECT((a))? g_object_set_data((a),(b),(c)):((gpointer)fprintf(stderr,"Bad G_OBJECT at %s line %d\n",__FILE__, __LINE__), NULL))
#define g_object_get_data(a,b) (G_IS_OBJECT(a)? g_object_get_data((a),(b)):((gpointer)fprintf(stderr,"Bad G_OBJECT at %s line %d\n",__FILE__, __LINE__), NULL))
#define gtk_action_get_name(a) (GTK_IS_ACTION(a)? gtk_action_get_name(a):((gpointer)fprintf(stderr,"Bad GTK_ACTION at %s line %d\n",__FILE__, __LINE__), NULL))


#endif
#if 0
  // use these to test for bad frees.
#define g_list_free
#define g_error_free
#define gtk_tree_path_free
#define g_free(a) (a)
#define g_string_free(a, b) (((GString*)(a))->str)
#endif

#if GTK_MAJOR_VERSION == 2
#else
//we always have homogeneous FALSE which is the default
//so this is not needed. static GtkWidget *vbox_setter (gboolean homogeneous, gint spacing) {GtkWidget *ret = gtk_box_new(GTK_ORIENTATION_VERTICAL, spacing);gtk_box_set_homogeneous (ret, homogeneous);return ret;}
#define gtk_vbox_new(homogeneous, spacing) gtk_box_new(GTK_ORIENTATION_VERTICAL, spacing)
#define gtk_hbox_new(homogeneous, spacing) gtk_box_new(GTK_ORIENTATION_HORIZONTAL, spacing)
#define gtk_hscale_new(adj) gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, adj)
#define gtk_hscrollbar_new(adj) gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, adj)
#define gtk_vscrollbar_new(adj) gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, adj)
#define g_strcasecmp g_ascii_strcasecmp
#endif

#define DEFAULT_KEYMAP "Default"
#define DEFAULT_KEYBINDINGS "Default.shortcuts"
#ifdef G_OS_WIN32
#define PREFS_FILE "denemorcV2"
#else
#define PREFS_FILE "denemorc"
#endif
  
#ifdef G_OS_WIN32
#define mswin g_print
#else
#define mswin
#endif

extern const gchar *None;
extern const gchar *Jack;
extern const gchar *Portaudio;
extern const gchar *Fluidsynth;

#define COMMANDS_DIR   "actions"
#define UI_DIR         "ui"
#define SOUNDFONTS_DIR "soundfonts"
#define PIXMAPS_DIR    "pixmaps"
#define FONTS_DIR      "fonts"
#define LOCALE_DIR     "po"
#define LILYPOND_INCLUDE_DIR "actions/lilypond"

typedef enum{
  DENEMO_DIR_COMMANDS,
  DENEMO_DIR_UI,
  DENEMO_DIR_SOUNDFONTS,
  DENEMO_DIR_PIXMAPS,
  DENEMO_DIR_FONTS,
  DENEMO_DIR_LOCALE,
  DENEMO_DIR_BIN,
  DENEMO_DIR_LILYPOND_INCLUDE
} DenemoDirectory;

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef DENEMO_DATASTRUCTURES  */
