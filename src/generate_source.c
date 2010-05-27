/* 
 * generate_source.c
 *
 * Program for generating source code from the old unmenued commands
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (C) 2007 Richard Shann
 *
 * License: this file may be used under the FSF GPL version 2
 */


#include <stdio.h>


#define N_

#define CMD_CATEGORY_NAVIGATION		0
#define CMD_CATEGORY_NOTE_ENTRY		1
#define CMD_CATEGORY_REST_ENTRY		2
#define CMD_CATEGORY_ARTICULATION	3
#define CMD_CATEGORY_EDIT		4
#define CMD_CATEGORY_MEASURE		5
#define CMD_CATEGORY_STAFF		6
#define CMD_CATEGORY_PLAYBACK		7
#define CMD_CATEGORY_OTHER		8

#define CMD_CLASS(cat) (cat&0xFF)
#define CMD_CATEGORY_DIRECT		0x100    /* does not require a wrapper to the callback */    
#define CMD_CATEGORY_BOOLEAN		0x200    /* call returns boolean */    



char *catname[9] = {N_("Navigation"),	
		   N_("Note entry"),	
		   N_("Rest entry"),	
		   N_("Articulation"),	
		   N_("Edit"),		
		   N_("Measure"),		
		   N_("Staff"),		
		   N_("Playback"),		
		   N_("Other") };







struct name_and_function
{
  unsigned category;
  /** Command name */
  char *icon;
  //char *menu_label;
  char *tooltip;
  char *name;
  char* function;
  char *menu_label;
  char *initial_setting;/*of radio/check items */
};


struct name_and_function denemo_commands[] = {

  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Moves the cursor one object left, altering the selection if any",	N_("CursorLeft"), "cursorleft", N_("Cursor Left")},
  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Moves the cursor one object left, without altering the selection",	N_("MoveCursorLeft"), "movecursorleft", N_("Move Cursor Left")},
  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Moves the cursor one scale step down",	N_("CursorDown"), "cursordown", N_("Cursor Down")},
  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Moves the cursor one scale step up",	N_("CursorUp"), "cursorup", N_("Cursor Up")},
  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Moves the cursor one object right, altering the selection if any",	N_("CursorRight"), "cursorright", N_("Cursor Right")},
  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Moves the cursor one object right, without altering the selection",	N_("MoveCursorRight"), "movecursorright", N_("Move Cursor Right")},

  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_DIRECT, NULL, "Moves the cursor to the Mark without altering the selection",	N_("GoToMark"), "goto_mark", N_("To Mark")},

  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_DIRECT, NULL, "Swaps the active end of the selection",	N_("SwapPointAndMark"), "swap_point_and_mark", N_("Swap Ends of Selection")},



  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_DIRECT, NULL, "Moves the cursor to the first object in the selection without altering the selection. returns #f if no selection",	N_("GoToSelectionStart"), "goto_selection_start", N_("To Selection Start")},

  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_DIRECT, NULL, "Pushes the current cursor position onto a stack",	N_("PushPosition"), "PushPosition", N_("Push Position")},
  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_DIRECT, NULL, "Pops a position from the stack of cursor positions, moving the cursor there",	N_("PopPosition"), "PopPosition", N_("Pop Position")},
  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_DIRECT, NULL, "Pops a position from the stack of cursor positions, pushes the current position, then moves the cursor to the popped position",	N_("PopPushPosition"), "PopPushPosition", N_("Pop and Push Position")},


  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_DIRECT, NULL, "Hides/Shows menus, panes etc. The ones shown are those checked in the view menu.", N_("ToggleReduceToDrawingArea"), "ToggleReduceToDrawingArea", N_("Hide/Show Menus")},


  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Moves the cursor to the staff above, extending selection if any",	N_("StaffUp"), "staffup", N_("Staff Up")},
  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Moves the cursor to the staff below, extending selection if any",	N_("StaffDown"), "staffdown", N_("Staff Down")},

  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Moves the cursor to the staff above without altering selection",	N_("MoveToStaffUp"), "movetostaffup", N_("Move to Staff Up")},
  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Moves the cursor to the staff below  without altering selection",	N_("MoveToStaffDown"), "movetostaffdown", N_("Move to Staff Down")},



  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Moves the cursor to the first object in the next measure, extending selection if any",	N_("MeasureLeft"), "measureleft", N_("Measure Left")},
  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Moves the cursor to the first object in the previous measure, extending selection if any",	N_("MeasureRight"), "measureright", N_("Measure Right")},

  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Moves the cursor to the first object in the next measure leaving selection, if any, unchanged",	N_("MoveToMeasureLeft"), "movetomeasureleft", N_("Move to Measure Left")},
  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Moves the cursor to the first object in the previous measureleaving selection, if any, unchanged",	N_("MoveToMeasureRight"), "movetomeasureright", N_("Move to Measure Right")},



  {CMD_CATEGORY_NAVIGATION, NULL, "Action for note A (Insert, Edit or Move Cursor, depending on Mode)",	N_("A"), "go_to_A_key", N_("A")},
  {CMD_CATEGORY_NAVIGATION, NULL, "Action for note B (Insert, Edit or Move Cursor, depending on Mode)",	N_("B"), "go_to_B_key", N_("B")},
  {CMD_CATEGORY_NAVIGATION, NULL, "Action for note C (Insert, Edit or Move Cursor, depending on Mode)",	N_("C"), "go_to_C_key", N_("C")},
  {CMD_CATEGORY_NAVIGATION, NULL, "Action for note D (Insert, Edit or Move Cursor, depending on Mode)",	N_("D"), "go_to_D_key", N_("D")},
  {CMD_CATEGORY_NAVIGATION, NULL, "Action for note E (Insert, Edit or Move Cursor, depending on Mode)",	N_("E"), "go_to_E_key", N_("E")},
  {CMD_CATEGORY_NAVIGATION, NULL, "Action for note F (Insert, Edit or Move Cursor, depending on Mode)",	N_("F"), "go_to_F_key", N_("F")},
  {CMD_CATEGORY_NAVIGATION, NULL, "Action for note G (Insert, Edit or Move Cursor, depending on Mode)",	N_("G"), "go_to_G_key", N_("G")},
  {CMD_CATEGORY_NAVIGATION, NULL, "Octave Up",	N_("OctaveUp"), "octave_up_key", N_("Octave Up")},
  {CMD_CATEGORY_NAVIGATION, NULL, "Octave Down",	N_("OctaveDown"), "octave_down_key", N_("Octave Down")},

  {CMD_CATEGORY_NOTE_ENTRY, NULL, "Insert whole-note",	N_("WholeNote"), "insert_chord_0key", "\"MUSIC_FONT(\"0\")\"", N_("Insert Whole Note")},
  {CMD_CATEGORY_NOTE_ENTRY,NULL , "Insert half-note",	N_("HalfNote"), "insert_chord_1key", "\"MUSIC_FONT(\"1\")\"", N_("Insert Half Note")},
  {CMD_CATEGORY_NOTE_ENTRY,NULL, "Insert quarter-note",	N_("QuarterNote"), "insert_chord_2key", "\"MUSIC_FONT(\"2\")\"", N_("Insert Quarter Note")},
  {CMD_CATEGORY_NOTE_ENTRY,NULL, "Insert eighth-note",	N_("EighthNote"), "insert_chord_3key", "\"MUSIC_FONT(\"3\")\"", N_("Insert Eigth Note")},
  {CMD_CATEGORY_NOTE_ENTRY,NULL, "Insert sixteenth-note",	N_("SixteenthNote"), "insert_chord_4key", "\"MUSIC_FONT(\"4\")\"", N_("Insert Sixteenth Note")},
  {CMD_CATEGORY_NOTE_ENTRY,NULL, "Insert thirty-second-note",	N_("ThirtysecondNote"), "insert_chord_5key", "\"MUSIC_FONT(\"5\")\"", N_("Insert Thirty Second Note")},
  {CMD_CATEGORY_NOTE_ENTRY,NULL , "Insert sixty-fourth-note",	N_("SixtyfourthNote"), "insert_chord_6key", "\"MUSIC_FONT(\"6\")\"", N_("Insert Sixty Forth Note")},
  {CMD_CATEGORY_NOTE_ENTRY,NULL , "Insert 128th-note",	N_("OneHundredTwentyEighthNote"), "insert_chord_7key", "128th", N_("Insert Sixty Forth Note")},
  {CMD_CATEGORY_NOTE_ENTRY,NULL , "Insert 256th-note",	N_("TwoHundredFiftySixthNote"), "insert_chord_8key", "256th", N_("Insert Two Hundred Sixth Fourth Note")},



  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("InsertBlankWholeNote"), "insert_blankchord_0key", N_("Insert Blank Whole Note")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("InsertBlankHalfNote"), "insert_blankchord_1key", N_("Insert Blank Half Note")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("InsertBlankQuarterNote"), "insert_blankchord_2key", N_("Insert Blank Quarter Note")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("InsertBlankEighthNote"), "insert_blankchord_3key", N_("Insert Blank Eigth Note")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("InsertBlankSixteenthNote"), "insert_blankchord_4key", N_("Insert Blank Sixteenth Note")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("InsertBlankThirtysecondNote"), "insert_blankchord_5key", N_("Insert Blank Thirty Second Note")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("InsertBlankSixtyfourthNote"), "insert_blankchord_6key", N_("Insert Blank Sixty Forth Note")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("InsertBlankOneHundredTwentyEighthNote"), "insert_blankchord_7key", N_("Insert Blank one hundred and twenty eighth Note")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("InsertBlankTwoHundredFiftySixthNote"), "insert_blankchord_8key", N_("Insert Blank two hundred and fifty sixth Note")},




  {CMD_CATEGORY_EDIT, NULL, "No Tooltip yet",	N_("ToggleRestMode"), "rest_toggle_key", N_("Toggle Rest Mode")},
  {CMD_CATEGORY_EDIT, NULL, "No Tooltip yet",	N_("ToggleBlankMode"), "toggle_blank", N_("Toggle Blank Mode")},

  {CMD_CATEGORY_REST_ENTRY, NULL, "Insert whole-note rest",  N_("InsertWholeRest"), "insert_rest_0key","\"MUSIC_FONT(\"r\")\"", N_("Insert Whole Rest")},
  {CMD_CATEGORY_REST_ENTRY,  NULL, "Insert half-note rest",	N_("InsertHalfRest"), "insert_rest_1key","\"MUSIC_FONT(\"s\")\"", N_("Insert Half Rest")},
  {CMD_CATEGORY_REST_ENTRY,  NULL, "Insert quarter-note rest",	N_("InsertQuarterRest"), "insert_rest_2key","\"MUSIC_FONT(\"t\")\"", N_("Insert Quarter Rest")},
  {CMD_CATEGORY_REST_ENTRY,  NULL, "Insert eighth-note rest",	N_("InsertEighthRest"), "insert_rest_3key","\"MUSIC_FONT(\"u\")\"", N_("Insert Eigth Rest")},
  {CMD_CATEGORY_REST_ENTRY,  NULL, "Insert sixteenth-note rest",	N_("InsertSixteenthRest"), "insert_rest_4key","\"MUSIC_FONT(\"v\")\"", N_("Insert Sixteenth Rest")},
  {CMD_CATEGORY_REST_ENTRY,  NULL, "Insert thirty-second note rest",	N_("InsertThirtysecondRest"), "insert_rest_5key","\"MUSIC_FONT(\"w\")\"", N_("Insert Thirty-second Rest")},
  {CMD_CATEGORY_REST_ENTRY,  NULL, "Insert sixty-fourth note rest",	N_("InsertSixtyfourthRest"), "insert_rest_6key","\"MUSIC_FONT(\"x\")\"", N_("Insert Sixty-forth Rest")},


  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("InsertDuplet"), "insert_duplet", N_("Insert Duplet")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("InsertTriplet"), "insert_triplet", N_("Insert Triplet")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("StartTriplet"), "start_triplet", N_("Start Triplet")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("EndTuplet"), "end_tuplet", N_("End Tuplet")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("InsertQuadtuplet"), "insert_quadtuplet", N_("Insert Quadtuplet")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("InsertQuintuplet"), "insert_quintuplet", N_("Insert Quintuplet")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("InsertSextuplet"), "insert_sextuplet", N_("Insert Sextuplet")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("InsertSeptuplet"), "insert_septuplet", N_("Insert Septuplet")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "Add a note to the current chord\\nThe cursor position determines which note to add",	N_("AddNoteToChord"), "add_tone_key","Add note", N_("Add Tone")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "Remove a note from the current chord",	N_("RemoveNoteFromChord"), "remove_tone_key","Remove note", N_("Remove Tone")},

  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("Sharpen"), "sharpen_key", "Sharpen", N_("Sharpen Note")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("Flatten"), "flatten_key", "Flatten", N_("Flatten Note")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "Alters a StemNeutral object to stem up.",	N_("StemUp"), "stem_up", "StemUp", N_("Stem Up")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "Alters a StemNeutral object to stem down.",	N_("StemDown"), "stem_down", "StemDown", N_("Stem Down")},

  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("AddDot"), "add_dot_key", N_("Add Dot")},
  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("RemoveDot"), "remove_dot_key", N_("Remove Dot")},

  {CMD_CATEGORY_ARTICULATION,  NULL, "Inserts a duplicate of the current note, tied",	N_("InsertTiedNote"), "tie_notes_key","Tied note", N_("Insert Tied Note")},
  {CMD_CATEGORY_ARTICULATION|CMD_CATEGORY_DIRECT,  NULL, "Ties/unties the note at the cursor",	N_("ToggleTie"), "toggle_tie","Toggle Tie", N_("Toggle Tied Note")},

  {CMD_CATEGORY_EDIT, NULL, "Delete the object at the cursor",	N_("DeleteObject"), "deleteobject", N_("Delete Object")},
  {CMD_CATEGORY_EDIT, NULL, "Delete to the left of the cursor.",	N_("DeletePreviousObject"), "deletepreviousobject", N_("Delete Previous Object")},

  {CMD_CATEGORY_MEASURE, NULL, "Insert a blank measure before the current one (in all staffs)",	N_("InsertMeasure"), "insert_measure_key", N_("Insert Measure Before")},
  {CMD_CATEGORY_MEASURE, NULL, "Insert a blank measure after the current one (in all staffs)",	N_("AddMeasure"), "addmeasureafter", N_("Insert Measure After")},



  {CMD_CATEGORY_MEASURE, NULL, "Insert a blank measure before the current one (in current staff)",	N_("InsertMeasureBefore"), "insertmeasurebefore", N_("Staff Insert Measure Before")},
  {CMD_CATEGORY_MEASURE, NULL, "Insert a blank measure in current staff after the current measure",	N_("InsertMeasureAfter"), "insertmeasureafter", N_("Staff Insert Measure After")},


  {CMD_CATEGORY_MEASURE, NULL, "No Tooltip yet",	N_("AppendMeasure"), "append_measure_key", N_("Staff Append Measure")},
  {CMD_CATEGORY_MEASURE, NULL, "Delete the current measure in this staff, leaving the staff short",	N_("DeleteMeasure"), "deletemeasure", N_("Staff Delete Measure")},
  {CMD_CATEGORY_MEASURE, NULL, "Delete the current measure in all staffs",	N_("DeleteMeasureAllStaffs"), "deletemeasureallstaffs", N_("Delete Measure All Staffs")},
  {CMD_CATEGORY_MEASURE, NULL, "No Tooltip yet",	N_("ShrinkMeasures"), "adjust_measure_less_width_key", N_("Shrink Measure")},
  {CMD_CATEGORY_MEASURE, NULL, "No Tooltip yet",	N_("WidenMeasures"), "adjust_measure_more_width_key", N_("Widen Measures")},

  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("ShorterStaffs"), "adjust_staff_less_height_key", N_("Shorter Staffs")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("TallerStaffs"), "adjust_staff_more_height_key", N_("Taller Staffs")},

  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertTrebleClef"), "newcleftreble", N_("New Treble Clef")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertBassClef"), "newclefbass", N_("New Bass Clef")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Insertg8clef"), "newclefg8", N_("New G8 Clef")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertAltoClef"), "newclefalto", N_("New Alto Clef")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertTenorClef"), "newcleftenor", N_("New Tenor Clef")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertSopranoClef"), "newclefsoprano", N_("New Soprano Clef")},

  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialTrebleClef"), "setcleftreble", N_("Set Treble Clef")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialBassClef"), "setclefbass", N_("Set Bass Clef")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialg8clef"), "setclefg8", N_("Set G8 Clef")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialAltoClef"), "setclefalto", N_("Set Alto Clef")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialTenorClef"), "setcleftenor", N_("Set Tenor Clef")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialSopranoClef"), "setclefsoprano", N_("Set Soprano Clef")},

  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Insert22Time"), "newtimesig22", N_("Insert 2/2 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Insert32Time"), "newtimesig32", N_("Insert 3/2 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Insert42Time"), "newtimesig42", N_("Insert 4/2 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Insert44Time"), "newtimesig44", N_("Insert 4/4 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Insert34Time"), "newtimesig34", N_("Insert 3/4 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Insert24Time"), "newtimesig24", N_("Insert 2/4 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Insert64Time"), "newtimesig64", N_("Insert 6/4 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Insert38Time"), "newtimesig38", N_("Insert 3/8 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Insert68Time"), "newtimesig68", N_("Insert 6/8 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Insert128Time"), "newtimesig128", N_("Insert 12/8 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Insert98Time"), "newtimesig98", N_("Insert 9/8 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Set22Time"), "settimesig22", N_("Set 2/2 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Set32Time"), "settimesig32", N_("Set 3/2 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Set42Time"), "settimesig42", N_("Set 4/2 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Set44Time"), "settimesig44", N_("Set 4/4 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Set34Time"), "settimesig34", N_("Set 3/4 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Set24Time"), "settimesig24", N_("Set 2/4 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Set64Time"), "settimesig64", N_("Set 6/4 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Set38Time"), "settimesig38", N_("Set 3/8 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Set68Time"), "settimesig68", N_("Set 6/8 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Set128Time"), "settimesig128", N_("Set 12/8 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("Set98Time"), "settimesig98", N_("Set 9/8 Time")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertCmaj"), "newkeysigcmaj", N_("Insert Cmaj")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertGmaj"), "newkeysiggmaj", N_("Insert Gmaj")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertDmaj"), "newkeysigdmaj", N_("Insert Dmaj")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertAmaj"), "newkeysigamaj", N_("Insert Amaj")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertEmaj"), "newkeysigemaj", N_("Insert Emaj")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertBmaj"), "newkeysigbmaj", N_("Insert Bmaj")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertFSharpmaj"), "newkeysigfsharpmaj", N_("Insert F# Major")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertCSharpmaj"), "newkeysigcsharpmaj", N_("Insert C# Major")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertFmaj"), "newkeysigfmaj", N_("Insert F Major")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertBflatmaj"), "newkeysigbflatmaj", N_("Insert Bb Major")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertEflatmaj"), "newkeysigeflatmaj", N_("Insert Eb Major")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertAflatmaj"), "newkeysigaflatmaj", N_("Insert Ab Major")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertDflatmaj"), "newkeysigdflatmaj", N_("Insert Db Major")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertGflatmaj"), "newkeysiggflatmaj", N_("Insert Gb Major")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertCflatmaj"), "newkeysigcflatmaj", N_("Insert Cb Major")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertAmin"), "newkeysigamin", N_("Insert A Minor")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertEmin"), "newkeysigemin", N_("Insert E Minor")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertBmin"), "newkeysigbmin", N_("Insert B Minor")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertFSharpmin"), "newkeysigfsharpmin", N_("Insert F# Minor")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertCSharpmin"), "newkeysigcsharpmin", N_("Insert C# Minor")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertGSharpmin"), "newkeysiggsharpmin", N_("Insert G# Minor")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertDSharpmin"), "newkeysigdsharpmin", N_("Insert D# Minor")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertASharpmin"), "newkeysigasharpmin", N_("Insert A# Minor")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertDmin"), "newkeysigdmin", N_("Insert D Minor")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertGmin"), "newkeysiggmin", N_("Insert G Minor")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertCmin"), "newkeysigcmin", N_("Insert C Minor")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertFmin"), "newkeysigfmin", N_("Insert F Minor")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertBflatmin"), "newkeysigbflatmin", N_("Insert Bb Minor")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertEflatmin"), "newkeysigeflatmin", N_("Insert Eb Minor")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("InsertAflatmin"), "newkeysigaflatmin", N_("Insert Ab Minor")},

  //Functions to Set Initial Key Sig
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialCmaj"), "setkeysigcmaj", N_("Set Initial Keysig to C Major")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialGmaj"), "setkeysiggmaj", N_("Set Initial Keysig to G Major")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialDmaj"), "setkeysigdmaj", N_("Set D Major as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialAmaj"), "setkeysigamaj", N_("Set A Major as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialEmaj"), "setkeysigemaj", N_("Set E Major as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialBmaj"), "setkeysigbmaj", N_("Set B Major as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialFSharpmaj"), "setkeysigfsharpmaj", N_("Set F# Major as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialCSharpmaj"), "setkeysigcsharpmaj", N_("Set C# Major as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialFmaj"), "setkeysigfmaj", N_("Set F Major as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialBflatmaj"), "setkeysigbflatmaj", N_("Set Bb Major as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialEflatmaj"), "setkeysigeflatmaj", N_("Set Eb Major as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialAflatmaj"), "setkeysigaflatmaj", N_("Set Ab Major as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialDflatmaj"), "setkeysigdflatmaj", N_("Set Db Major as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialGflatmaj"), "setkeysiggflatmaj", N_("Set Gb Major as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialCflatmaj"), "setkeysigcflatmaj", N_("Set Cb Major as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialAmin"), "setkeysigamin", N_("Set A Minor as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialEmin"), "setkeysigemin", N_("Set E Minor as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialBmin"), "setkeysigbmin", N_("Set B Minor as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialFSharpmin"), "setkeysigfsharpmin", N_("Set F# Minor as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialCSharpmin"), "setkeysigcsharpmin", N_("Set C# Minor as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialGSharpmin"), "setkeysiggsharpmin", N_("Set G# Minor as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialDSharpmin"), "setkeysigdsharpmin", N_("Set D# Minor as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialASharpmin"), "setkeysigasharpmin", N_("Set A# Minor as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialDmin"), "setkeysigdmin", N_("Set D Minor as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialGmin"), "setkeysiggmin", N_("Set G Minor as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialCmin"), "setkeysigcmin", N_("Set C Minor as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialFmin"), "setkeysigfmin", N_("Set F Minor as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialBflatmin"), "setkeysigbflatmin", N_("Set Bb Minor as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialEflatmin"), "setkeysigeflatmin", N_("Set Eb Minor as Initial Keysig")},
  {CMD_CATEGORY_STAFF, NULL, "No Tooltip yet",	N_("SetInitialAflatmin"), "setkeysigaflatmin", N_("Set Ab Minor as Initial Keysig")},


  {CMD_CATEGORY_NAVIGATION, NULL, "Sets the start point for a selection,\\nthe end point of the selection is unaltered",	N_("SetMark"), "set_mark", N_("Set Mark")},
  {CMD_CATEGORY_NAVIGATION, NULL, "Gets rid of the selection.",	N_("UnsetMark"), "unset_mark", N_("Unset Mark")},
  {CMD_CATEGORY_NAVIGATION, NULL, "Extends the selection to the current cursor position",	N_("SetPoint"), "set_point", N_("Set Point")},
  {CMD_CATEGORY_ARTICULATION, NULL, "Insert/delete begin slur on this note",	N_("ToggleBeginSlur"), "toggle_begin_slur", N_("Begin Slur")},
  {CMD_CATEGORY_ARTICULATION, NULL, "Insert/delete end slur on this note",	N_("ToggleEndSlur"), "toggle_end_slur", N_("End Slur")},

  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleStartCrescendo"), "toggle_start_crescendo", N_("Start Crescendo")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleEndCrescendo"), "toggle_end_crescendo", N_("End Crescendo")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleStartDiminuendo"), "toggle_start_diminuendo", N_("Start Diminuendo")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleEndDiminuendo"), "toggle_end_diminuendo", N_("End Diminuendo")},

  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleAccent"), "add_accent", N_("Accent (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleFermata"), "add_fermata", N_("Fermata (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleStaccato"), "add_staccato", N_("Staccato (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleTenuto"), "add_tenuto", N_("Tenuto (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleTrill"), "add_trill", N_("Trill  (Off/On")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleTurn"), "add_turn", N_("Turn (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleMordent"), "add_mordent", N_("Mordent (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleStaccatissimo"), "add_staccatissimo", N_("Staccatissimo  (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleCoda"), "add_coda", N_("Coda (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleFlageolet"), "add_flageolet", N_("Flageolet  (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleOpen"), "add_open", N_("Open (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("TogglePrallMordent"), "add_prallmordent", N_("Prall Mordent  (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("TogglePrallPrall"), "add_prallprall", N_("Prall Prall  (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("TogglePrall"), "add_prall", N_("Prall (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleReverseTurn"), "add_reverseturn", N_("Reverse Turn  (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleSegno"), "add_segno", N_("Segno  (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleSforzato"), "add_sforzato", N_("Sforzato  (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleStopped"), "add_stopped", N_("Stopped  (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleThumb"), "add_thumb", N_("Thumb  (Off/On)")},
  /*{CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleTrillElement"), "add_trillelement", N_("Toggle Trill")},
     {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleTrill_Element"), "add_trill_element", N_("Trill (Off/On)")}, */
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleUpprall"), "add_upprall", N_("Up Prall (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ToggleArpeggio"), "add_arpeggio", N_("Apreggio (Off/On)")},
  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("SetGrace"), "set_grace", N_("Set Grace")},
  {CMD_CATEGORY_ARTICULATION|CMD_CATEGORY_DIRECT, NULL, "Makes the note at the cursor a grace note, if it is one, makes it normal",	N_("ToggleGrace"), "toggle_grace", N_("Grace Note Off/On")},

  {CMD_CATEGORY_ARTICULATION, NULL, "No Tooltip yet",	N_("ForceCaution"), "force_cautionary", N_("Force Cautionary Accidental")},

  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("ChangePitch"), "change_pitch", N_("Change Pitch")},

  {CMD_CATEGORY_NOTE_ENTRY, NULL, "No Tooltip yet",	N_("InsertRhythm"), "insert_rhythm_pattern", N_("Insert Rhythm")},
  {CMD_CATEGORY_OTHER, NULL, "Make next rhythm pattern\\nthe prevailing rhythm.\\nNotes entered will follow this pattern",	N_("NextRhythm"), "nextrhythm", N_("Next Rhythm")},
  {CMD_CATEGORY_MEASURE, NULL, "Appends a blank measure to every staff in this movement",	N_("AppendMeasureAllStaffs"), "append_measure_score", N_("Append Measure All Staffs")},
  

 {CMD_CATEGORY_DIRECT, NULL, "Execute the scheme code from the scripting window",	N_("ExecuteScheme"), "execute_scheme", N_("Execute Scheme")},



  /* from view.c menu_entries[]  */
  {CMD_CATEGORY_DIRECT, NULL, "Creating, saving, loading, displaying and printing musical scores", N_("FileMenu"), NULL, "File", N_("File Menu")},
  {CMD_CATEGORY_DIRECT, NULL, "Creating, saving places in musical scores", N_("Bookmarks"), NULL, "Bookmarks", N_("Bookmarks")},
  {CMD_CATEGORY_DIRECT, NULL, "Different keyboard and MIDI entry modes", N_("ModeMenu"), NULL, "Mode"},
  {CMD_CATEGORY_DIRECT, NULL, "General editing commands", N_("EditMenu"), NULL, "Edit", N_("Edit Menu")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Control which tools are to be shown", N_("ViewMenu"), NULL, "View", N_("View Menu")}, 

  {CMD_CATEGORY_DIRECT, NULL, "Staffs and voices", N_("StaffMenu"), NULL,"Staffs/Voices", N_("Staff Menu")},
  {CMD_CATEGORY_DIRECT, NULL, "Movements in a score", N_("MovementMenu"), NULL,"Movements", N_("Movement Menu")},
  {CMD_CATEGORY_DIRECT, NULL, "Help with denemo", N_("HelpMenu"), NULL, "Help", N_("Help Menu")}, 
  {CMD_CATEGORY_DIRECT, NULL, "input audio or midi", N_("InputMenu"), NULL, "Input", N_("Input Menu")},
 {CMD_CATEGORY_DIRECT, NULL, "Shifts the set of accidentals one step sharper", N_("SharpenEnharmonicSet"), "set_sharper", N_("Shift Accidentals Sharpwise")}, 
 {CMD_CATEGORY_DIRECT, NULL, "Shifts the set of accidentals one step flatter", N_("FlattenEnharmonicSet"), "set_flatter", N_("Shift Accidentals Flatwise")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Playing the music through midi file", N_("PlaybackMenu"), NULL, "Playback", N_("Playback Menu")}, 



  {CMD_CATEGORY_DIRECT, "GTK_STOCK_NEW", "Start a new musical score", N_("New"), "file_newwrapper", N_("New File")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_OPEN", "Open a file containing a music score for editing", N_("Open"), "file_open_with_check", N_("Open")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_OPEN", "Add staffs from a Denemo file", N_("AddStaffs"), "file_add_staffs", N_("Add Staffs")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_OPEN", "Add movements from a Denemo file", N_("AddMovements"), "file_add_movements", N_("Add Movement")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_PROPERTIES", "Change properties of this movement", N_("MovementProps"), "movement_props_dialog", N_("Change Properties")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_OPEN", "Open a file containing a music score for editing in a separate working area (tab", N_("OpenNewWindow"), "openinnew", N_("Open In New")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_SAVE", "Save the score", N_("Save"), "file_savewrapper", N_("Save")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_SAVE_AS", "Save the score under a new name", N_("SaveAs"), "file_saveaswrapper", N_("Save As")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_OPEN", "Start a new score from a built-in template file", N_("OpenTemplate"), "system_template_open_with_check", N_("Open Template")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_OPEN", "Start a new score from a built-in example", N_("OpenExample"), "system_example_open_with_check", N_("Open Example")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_OPEN", "Start a new score from one of your own template files", N_("OpenMyTemplate"), "local_template_open_with_check", N_("Open custom template")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_SAVE_AS", "Save the score as a template for re-use as a starting point for new scores", N_("SaveTemplate"), "template_save", N_("Save Template")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Create working area (tab with an empty score in it)", N_("NewWindow"), "newview", N_("New Tab")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Insert a new movement before the current one", N_("InsertMovementBefore"), "insert_movement_before", N_("Insert Movement Before")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Insert a new movement after the current one", N_("InsertMovementAfter"), "insert_movement_after", N_("Insert Movement After")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Create a new movement, usign any default template", N_("NewMovement"), "append_new_movement", N_("New Movement")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_SAVE_AS", "Save Parts: each staff becomes a file in lilypond format", N_("SaveParts"), "file_savepartswrapper", N_("Save Parts")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_SAVE_AS", "Export the score as a PDF document file", N_("ExportPDF"), "export_pdf_action", N_("Export PDF")},

  {CMD_CATEGORY_DIRECT, "GTK_STOCK_PROPERTIES", "Start up a wizard to create a new score. This allows you to set various properties of the score", N_("ConfigureScore"), "scorewizard", N_("Score Wizard")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_PRINT_PREVIEW", "Displays the final finished score in your pdf viewer", N_("PrintPreview"), "printpreview_cb", N_("Print Preview")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_PRINT", "Displays selected music from score in your pdf viewer", N_("PrintSelection"), "printselection_cb", N_("Print Selection")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_PRINT_PREVIEW", "Displays a musical excerpt in your image viewer", N_("PrintExcerptPreview"), "printexcerptpreview_cb", N_("Print Excerpt")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_PRINT", "Displays the final finished score in a pdf viewer. From this you can print the file using the print command of the viewer", N_("Print"), "printall_cb", N_("Print")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_PRINT", "Displays the final finished score for the current part (that is current staff", N_("PrintPart"), "printpart_cb", N_("Print Part")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_CLOSE", "Close the current score. Other windows will stay open", N_("Close"), "close_gui_with_check", N_("Close Score")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_QUIT", "Quit the Denemo program", N_("Quit"), "closewrapper", N_("Quit")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_UNDO", "Undo", N_("Undo"), "undowrapper", N_("Undo")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_REDO", "Redo", N_("Redo"), "redowrapper", N_("Redo")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Selecting stretches of notes", N_("Select"), NULL, N_("Select")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Extend the selection", N_("ExtendSelect"), NULL, N_("Extend Selection")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_COPY", "Copy", N_("Copy"), "copywrapper", N_("Copy")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_CUT", "Cut", N_("Cut"), "cutwrapper", N_("Cut")}, 
  // {CMD_CATEGORY_DIRECT, "GTK_STOCK_PASTE", "Paste the selected music", N_("Paste"), "pastewrapper", N_("Paste")},
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_PASTE", "Paste LilyPond notes from the text clipboard", "PasteClipboard", "paste_clipboard", N_("Paste LilyPond notes")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_PROPERTIES", "Change some of the properties of the current score. This will start up a dialog window", N_("ScoreProperties"), "score_properties_dialog", N_("Score Properties")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Save the selected music. Not sure if this is working", N_("SaveSelection"), "saveselwrapper", N_("Save Selection")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_PREFERENCES", "Set and save your preferences for how Denemo operates on startup. Edit .denemo/denemorc for missing ones", N_("Preferences"), "preferences_change", N_("Change Preferences")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Set actions to take in response to keypresses", N_("KeyBindings"), NULL, "Customize Commands, Shortcuts ...", N_("Customize Key Bindings")},
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_SAVE", "Save the current commands and keyboard shortcuts as the default", N_("SaveAccels"), "save_default_keymap_file_wrapper", "Save Command Set", N_("Save Command Set")}, 
  {CMD_CATEGORY_DIRECT, NULL, "View help, change and save keyboard shortcuts", "CommandManagement", "configure_keyboard_dialog", "Manage Command Set", N_("Manage Command Set")}, 

  {CMD_CATEGORY_DIRECT, NULL, "Swap this staff with the one higher up. Note this actually swaps voices.", N_("SwapStaffs"), "swapstaffs", N_("Swap Staffs")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Split off the next voice as a separate staff", N_("SplitVoices"), "splitstaffs", N_("Split Voices")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Merge this staff as a voice on the previous staff", N_("JoinVoices"), "joinstaffs", N_("Join Voices")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Swap this movement with the one before", N_("SwapMovements"), "swapmovements", N_("Swap Movements")}, 
  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Go to the higher numbered voice on staff, extending selection if any", N_("VoiceUp"), "voiceup", N_("Voice Up")}, 
  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Go to the lower numbered voice on this staff, extending selection if any", N_("VoiceDown"), "voicedown", N_("Voice Down")}, 

  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Go to the higher numbered voice on staff without altering selection", N_("MoveToVoiceUp"), "movetovoiceup", N_("Move to Voice Up")}, 
  {CMD_CATEGORY_NAVIGATION|CMD_CATEGORY_BOOLEAN, NULL, "Go to the lower numbered voice on this staff without altering selection", N_("MoveToVoiceDown"), "movetovoicedown", N_("Move to Voice Down")}, 



  {CMD_CATEGORY_DIRECT, NULL, "Inserts a new staff before the current staff", N_("AddBefore"), "newstaffbefore", N_("Add Staff Before")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Inserts/Adds a new staff after the current staff", N_("AddAfter"), "dnm_newstaffafter", N_("Add Staff After")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Inserts a new staff at the top of the score", N_("AddInitial"), "newstaffinitial", N_("Add Initial Staff")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Inserts a new staff at the end of the score", N_("AddLast"), "newstafflast", N_("Add Last Staff")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Deletes the staff before the current staff", N_("DeleteBefore"), "delete_staff_before", N_("Delete Staff Before")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Deletes the current staff", N_("DeleteStaff"), "delete_staff_current", N_("Delete Current Staff")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Deletes the staff after the current staff", N_("DeleteAfter"), "delete_staff_after", N_("Delete Staff After")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Adds a new voice (part), to the current staff. It is tricky to switch between the voices. Suggest to use merge staffs", "AddVoice", "dnm_newstaffvoice", N_("Add Voice")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_PROPERTIES", "Change the properties of the current staff", N_("StaffProperties"), "staff_properties_change_cb", N_("Staff Properties")},
  {CMD_CATEGORY_DIRECT, NULL, "Insert", N_("InsertMenu"), NULL},  
  {CMD_CATEGORY_DIRECT, NULL, "Clef", N_("Clef"), NULL}, 
  {CMD_CATEGORY_DIRECT, NULL, "Change the initial clef of the current staff", N_("InitialClef"), "clef_change_initial", N_("Initial Clef")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Insert/Edit a change of clef at the cursor", N_("InsertClef"), "clef_change_insert", N_("Clef Change")}, 


  {CMD_CATEGORY_DIRECT, NULL, "insert change key signature or set the initial key", N_("Key"), NULL, N_("Keys")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Set the initial key signature of the current staff", N_("InitialKey"), "key_change_initial", N_("Initial Key")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Insert/Edit a key change at the cursor position", N_("InsertKey"), "key_change_insert", N_("Key Signature Change")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Manage the time signature changes and initial value", N_("TimeSig"), NULL,  N_("Time Signatures")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Set the initial time signature of the current staff", N_("InitialTimeSig"), "timesig_change_initial", N_("Inital Time Signature")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Edit/Insert a time signature change for the current measure", N_("InsertTimeSig"), "timesig_change_insert", N_("Time Signature Change")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Change the type of notehead for the current note", N_("ChangeNotehead"), "set_notehead", N_("Set Notehead")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Inserts a stem neutral object. After this automatic stem directions are active. You can click on this tag and use Sharpen/StemUp etc commands to change stem direction", N_("InsertStem"), "stem_directive_insert", N_("Auto Stemming")},
  {CMD_CATEGORY_DIRECT, NULL, "Add a verse of lyrics", "AddVerse", "add_verse", "Add Lyric Verse", NULL}, 
  {CMD_CATEGORY_DIRECT, NULL, "Deletes current verse of lyrics from current voice", "DeleteVerse", "delete_verse", "Delete Verse", NULL}, 
  {CMD_CATEGORY_DIRECT, NULL, "Add a bass figure to the current note. Use | sign to split the duration of a note so as to have multiple figures on one note. See Lilypond docs for other notation", N_("EditFiguredBass"), "figure_insert", "Insert/Edit Figured Bass", N_("Edit Figured Bass")},
  {CMD_CATEGORY_DIRECT, NULL, "Delete the figured bass on the current staff", N_("DeleteFiguredBass"), "delete_figured_bass", "Delete Figures", N_("Delete Figures")},
  {CMD_CATEGORY_DIRECT, NULL, "Hide the figured bass on the current staff on printing", N_("HideFiguredBass"), "hide_figured_bass", "Hide Figures (Print)", N_("Hide Figures")},
  {CMD_CATEGORY_DIRECT, NULL, "Allows chord symbols to be added to the current note. E.G.cis:dim7 for c-sharp diminished 7th. See Lilypond docs for notation", N_("EditChords"), "fakechord_insert", N_("Edit Chord Symbols")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Inserts a dynamic marking at the cursor position", N_("InsertDynamic"), "insert_dynamic", N_("Insert Dynamics")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Insert or edit a directive in the LilyPond music typesetting language. This can be used for extra spacing, transposing or almost anything. See LilyPond documentation for ideas.", N_("InsertLilyDirective"), "standalone_directive", N_("Insert Lilypond")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Edit the object at the cursor.", N_("EditObject"), "edit_object", N_("Edit Object")},

  {CMD_CATEGORY_DIRECT, NULL, "Edit any directives attached to chord/note at cursor.", N_("EditDirective"), "edit_object_directive", N_("Edit Directives")},
  {CMD_CATEGORY_DIRECT, NULL, "Edit any directives attached to staff.", N_("EditStaffDirective"), "edit_staff_directive", N_("Edit Staff Directives")},
  {CMD_CATEGORY_DIRECT, NULL, "Edit any directives attached to voice.", N_("EditVoiceDirective"), "edit_voice_directive", N_("Edit Voice Directives")},
  {CMD_CATEGORY_DIRECT, NULL, "Edit any directives attached to score.", N_("EditScoreDirective"), "edit_score_directive", N_("Edit Score Directives")},
  {CMD_CATEGORY_DIRECT, NULL, "Edit any directives attached to movement.", N_("EditMovementDirective"), "edit_movement_directive", N_("Edit Movement Directives")},


  {CMD_CATEGORY_DIRECT, NULL, "Edit any directives attached to clef.", N_("EditClefDirective"), "edit_clef_directive", N_("Edit Clef Directives")},
  {CMD_CATEGORY_DIRECT, NULL, "Edit any directives attached to time signature.", N_("EditTimesigDirective"), "edit_timesig_directive", N_("Edit Time Signature Directives")},
  {CMD_CATEGORY_DIRECT, NULL, "Edit any directives attached to key signature.", N_("EditKeysigDirective"), "edit_keysig_directive", N_("Edit Key Signature Directives")},

  {CMD_CATEGORY_DIRECT, NULL, "Delete a directive attached to chord/note at cursor.", N_("DeleteDirective"), "delete_object_directive", N_("Delete a Directive")},

 
  {CMD_CATEGORY_DIRECT, NULL, "Attach or edit attached LilyPond text to the note at the cursor. This can be used for guitar fingerings, cautionary accidentals and much more. See LilyPond documentation.",N_("AttachLilyToNote"), "note_directive", N_("Attach Lilypond to Note")},
  {CMD_CATEGORY_DIRECT, NULL, "Attach or edit attached LilyPond text to the chord at the cursor. This can be used for attaching and placing text and much more. See LilyPond documentation.",N_("AttachLilyToChord"), "chord_directive", N_("Attach Lilypond to Chord")},
  {CMD_CATEGORY_DIRECT, NULL, "Inserts specialized barline at the cursor position. Mostly not working", N_("InsertBarline"), "insert_barline", N_("Insert Barline")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Getting around the score", N_("NavigationMenu"), NULL, "Navigation", N_("Navigation Menu")},
  {CMD_CATEGORY_DIRECT, NULL, "Opens a dialog for going to a numbered measure", N_("GoToMeasure"), "tomeasurenum", "Go to Measure", N_("Go To Measure")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_GOTO_FIRST", "Cursor to start of staff/voice, extending selection if any", N_("GoToBeginning"), "tohome","Go to Beginning", N_("Go To Beginning")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_GOTO_LAST",  "Cursor to end of staff/voice, extending selection if any", N_("GoToEnd"), "toend",  "Go to End", N_("Go to End")},

  {CMD_CATEGORY_DIRECT, "GTK_STOCK_GOTO_FIRST", "Cursor to start of staff/voice, without extending selection if any" , N_("MoveToBeginning"), "movetostart","Move to Beginning", N_("Move to Beginning")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_GOTO_LAST", "Cursor to end of staff/voice, without extending selection if any", N_("MoveToEnd"), "movetoend",  "Move to End", N_("Move to End")},



 
  {CMD_CATEGORY_DIRECT, NULL, "Go to the next movement", N_("NextMovement"), "next_movement", N_("Next Movement")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Go to the previous movement", N_("PreviousMovement"), "prev_movement", N_("Previous Movement")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Delete the current movement", N_("DeleteMovement"), "delete_movement", N_("Delete Movement")}, 


  {CMD_CATEGORY_DIRECT, "GTK_STOCK_MEDIA_PLAY", "Play", N_("Play"), "ext_midi_playback", N_("Play")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_MEDIA_STOP", "Stop", N_("Stop"), "stop_midi_playback", N_("Stop")}, 


  {CMD_CATEGORY_DIRECT, "GTK_STOCK_MEDIA_PLAY", "Play using CSound...", N_("PlayCSound"), "csoundplayback", N_("Csound Playback")}, 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_PROPERTIES", "Allows you to specify properties used in playing back (midi and csound", N_("PlaybackProperties"), "playback_properties_change", "Playback Properties", N_("Playback Properties")}, 

  {CMD_CATEGORY_DIRECT, NULL, "Opens a browser on the user manual", N_("Help"), "browse_manual", N_("Browse Manual")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Gives the version number etc of this program", N_("About"), "about", N_("About")}, 


  {CMD_CATEGORY_DIRECT, NULL, "Allows choosing extra commands/menu items from disk", N_("MoreMenu"), NULL, N_("More")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Allows choosing standard extra commands/menu items", N_("MoreCommands"), "morecommands", N_("More Commands")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Allows choosing extra commands/menu items from your own collection of extras", N_("MyCommands"), "mycommands", N_("My Commands")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Refreshes the set of commands available from Denemo.org.\\nUse More Commands after this has finished", N_("FetchCommands"), "fetchcommands", N_("Update Commands from Internet")}, 




 
  {CMD_CATEGORY_DIRECT, "GTK_STOCK_OPEN", "Open previously used files", N_("OpenRecent"), NULL, N_("Open Recent")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Toggle between current mode and edit mode", N_("ToggleEdit"), "toggle_edit_mode", N_("Toggle Edit Mode")},
  {CMD_CATEGORY_DIRECT, NULL, "Toggle between note entry and rest entry", N_("ToggleRest"),  "toggle_rest_mode", N_("Toggle Rest Mode")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Toggle audible feedback on/off", N_("ToggleRhythm"),  "toggle_rhythm_mode", N_("Toggle Audible Feedback")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Clear the list of pitches that overlay the notes", N_("ClearOverlay"),  "clear_overlay", N_("Clear Overlay")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Copy selection as music sample or rhythm pattern for notes to follow as they are entered", N_("CreateRhythm"), "create_rhythm_cb", N_("Create Sample")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Delete the selected music sample/rhythm pattern", N_("DeleteRhythm"), "delete_rhythm_cb", N_("Delete Sample")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Moving the cursor and inserting notes or rests there", N_("ClassicModeNote"), NULL, N_("Classic Mode")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Moving the cursor to the nearest ...", N_("SelectNote"), NULL, N_("Select Note")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Actions for notes: inserting, deleting, etc.", N_("InsertModeNote"), NULL, N_("Insert")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Actions to control the stem up/down", N_("StemControl"), NULL, N_("Stem Direction")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Different types of notehead", N_("NoteheadControl"), NULL, N_("Notehead Types")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Creating Tied Notes", N_("TiedNotes"), NULL, N_("Tied Notes")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Effects that are only for the Denemo display,\\nnot affecting the printed page.", N_("DisplayEffects"), NULL, N_("Display Effects")}, 

  {CMD_CATEGORY_DIRECT, NULL, "Articulations", N_("Articulations"), NULL, N_("Articulations")},
  {CMD_CATEGORY_DIRECT, NULL, "Slurs", N_("Slurs"), NULL, N_("Slurs")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Grace Notes", N_("GraceNotes"), NULL, N_("Grace Notes")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Score", N_("Score"), NULL, N_("Score")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Instruments", N_("Instruments"), NULL, N_("Instruments")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Markings", N_("Markings"), NULL, N_("Markings")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Text and symbol Markings", N_("TextMarks"), NULL, N_("Text/Symbol")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Commands for Stringed Instruments", "Strings", NULL, N_("Strings")}, 

  {CMD_CATEGORY_DIRECT, NULL, "Inserting the note ...", N_("InsertNote"), NULL, "Note Entry" , N_("Note Entry")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Lyrics", N_("Lyrics"), NULL, N_("Lyrics")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Standalone Directives", N_("Directives"), NULL, N_("Directives")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Moving around the piece", N_("Navigation"), NULL, N_("Navigation")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Entering notes", N_("NoteEntry"), NULL, N_("Note Entry")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Various expressive marks", N_("Articulation"), NULL, N_("Articulation")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Editing", N_("Edit"), NULL, N_("Edit")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Manipulating measures", N_("Measure"), NULL, N_("Measure")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Commands for staffs", N_("Staff"), NULL, N_("Staff")}, 

  {CMD_CATEGORY_DIRECT, NULL, "Playing the music through midi file", N_("Playback"), NULL, N_("Playback")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Changing the prevailing duration or rhythm pattern", N_("SelectDuration"), NULL, N_("Select Duration")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Appending, Changing, and deleting notes", N_("EditModeNote"), NULL, N_("Edit")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Changing the note at the cursor to the nearest ...", N_("EditNote"), NULL, N_("Edit Note")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Changing the duration of note at the cursor or appending a note of the given duration", N_("EditDuration"), NULL, N_("Edit Duration")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Moving the cursor", N_("Cursor"), NULL, N_("Cursor")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Moving the cursor to note positions", N_("CursorToNote"), NULL, N_("Cursor to Note")}, 

  {CMD_CATEGORY_DIRECT, NULL, "Insert/change clef Set initial clef", N_("ClefMenu"), NULL, N_("Clefs")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Adding notes to make chords", N_("ChordMenu"), NULL, "Chords", N_("Chords")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Adding Chord Symbols over music", N_("ChordSymbols"), NULL, "Chords Symbols", N_("Chord Symbols")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Adding Figured Bass Figures", N_("FiguredBass"), NULL, "Figured Bass", N_("Figured Bass")}, 


  {CMD_CATEGORY_DIRECT, NULL, "Measures: adding, deleting, navigating etc", N_("MeasureMenu"), NULL, N_("Measures")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Inserting notes, measures, staffs, keysigs etc", N_("Insert"), NULL, N_("Insert")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Insert a Staff relative to current staff", N_("InsertStaff"), NULL, N_("Insert Staff")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Insert a Movement relative to current movement", N_("InsertMovement"), NULL, N_("Insert Movement")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Inserting notes of a given duration", N_("InsertDuration"), NULL, N_("Insert Duration")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Changing properties of notes, measures, staffs, keysigs etc", N_("Change"), NULL, N_("Change")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Modeless actions on notes/rests", N_("ModelessNote"), NULL, N_("Notes/Durations")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Actions for notes/rests", N_("NotesRests"), NULL, N_("Notes/Rests")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Modeless entry of rests", N_("RestEntry"), NULL, N_("Rest Entry")}, 

  {CMD_CATEGORY_DIRECT, NULL, "Editing directives", N_("EditDirectivesMenu"), NULL, N_("Edit Directive(s)")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Changing the note at the cursor to the nearest ...", N_("ChangeNote"), NULL, N_("Change Note")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Changes the duration of the current note", N_("ChangeDuration"), NULL, N_("Change Duration")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Changes the duration of the current rest", N_("ChangeRest"), NULL, N_("Change Rest")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Dynamics, staccato, slurs, ties and other expressive marks", N_("ExpressionMarks"), NULL, "Expression Marks", N_("Expression Marks")},
  //  {CMD_CATEGORY_DIRECT, NULL, "Markings above and below the music", N_("Markings"), NULL, "Markings", N_("Markings")},
  {CMD_CATEGORY_DIRECT, NULL, "Dynamic markings", N_("Dynamics"), NULL, "Dynamics", N_("Dynamics")},
 
  {CMD_CATEGORY_DIRECT, NULL, "grace notes etc", N_("Ornaments"), NULL, N_("Ornaments")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Lyrics, chord symbols, figured basses etc", N_("Other"), NULL, N_("Other")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Less used actions", N_("Others"), NULL, N_("Others")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Customized LilyPond inserts. Store often-used inserts here labelled with what they do", N_("Favorites"), NULL, N_("Favorites")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Add a custom LilyPond insert to favorites menu", N_("AddFavorite"), NULL, N_("Add Favorite")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Entering triplets and other tuplets", N_("Tuplets"), NULL, N_("Tuplets")}, 
  {CMD_CATEGORY_DIRECT, NULL, "Deleting notes, measures staffs keysigs etc", N_("Delete"), NULL, N_("Delete")}, 

};






#define ni denemo_commands[i].name
#define ii denemo_commands[i].icon
#define ml denemo_commands[i].menu_label
#define ti denemo_commands[i].tooltip
#define fi denemo_commands[i].function
#define mi denemo_commands[i].category
int main() {
  FILE *callbacks, *entries, *xml, *scheme, *scheme_cb, *register_commands;
  scheme_cb = fopen("scheme_cb.h", "w");
  scheme = fopen("scheme.h", "w");
  callbacks = fopen("callbacks.h", "w");
  entries =  fopen("entries.h", "w");
  register_commands = fopen("register_commands.h", "w");
  xml = fopen("xml.fragment", "w");
  if(!callbacks || !entries || !xml || !scheme || !scheme_cb || !register_commands)
    return -1;
  fprintf(callbacks, "/******** generated automatically from generate_source. See generate_source.c */\n");
  fprintf(entries, "/******** generated automatically from generate_source. See generate_source.c */\n");

  int i,j;
  int n_denemo_commands = (sizeof (denemo_commands)
			   / sizeof (struct name_and_function));

  fprintf(scheme, "gchar *text;\n");
  for(i=0;i<n_denemo_commands;i++) {
    if (fi != NULL) {
      if(!(mi&CMD_CATEGORY_DIRECT)) {
	fprintf(callbacks, "/*%s %s*/\n",ni, fi);
	/*******************   create a callback for calling from a menuitem *******************/
	if(mi&CMD_CATEGORY_BOOLEAN) {
	  fprintf(callbacks, "static void %s_cb (GtkAction *action, DenemoScriptParam *param) {\n"
		  "%s (param);\ndisplayhelper (Denemo.gui);\n%s}\n",
		  fi, fi, (CMD_CLASS(mi)==CMD_CATEGORY_NAVIGATION)?"":"  score_status(gui, TRUE);\n");
	} else {
	  fprintf(callbacks, "static void %s_cb (GtkAction *action, gpointer param) {\n"
		  "  DenemoGUI *gui = Denemo.gui;\n"
		  "%s (gui);\ndisplayhelper (gui);\n%s}\n", fi, fi, CMD_CLASS(mi)==CMD_CATEGORY_NAVIGATION?"":"  score_status(gui, TRUE);\n");
	}
      }
      
      /*******************   create a procedure d-<name> in scheme to call scheme_<name>  *******************/
      fprintf(scheme, "/*%s %s*/\n",ni, fi);
      fprintf(scheme, "SCM scheme_%s(SCM optional);\ninstall_scm_function (\"d-%s\", scheme_%s);\n", ni, ni, ni);// for direct callback via (scheme_xxx)
      fprintf(scheme, "g_object_set_data(G_OBJECT(action_of_name(Denemo.map, \"%s\")), \"scm\", (gpointer)1);\n", ni); //define a property "scm" on the action to mean it scheme can call the action.

      /*******************   create a callback scheme_<name> for calling from a scheme procedure d-<name>  *******************/
      fprintf(scheme_cb, "SCM scheme_%s (SCM optional) {\ngboolean query=FALSE;\nDenemoScriptParam param;\nGString *gstr=NULL;\nint length;\n   char *str=NULL;\nif(scm_is_string(optional)){\nstr = scm_to_locale_stringn(optional, &length);\ngstr = g_string_new_len(str, length);\nif(!strncmp(\"query\",str,5)) query = TRUE;\
          }\n\
         param.string = gstr;\n\
         param.status = FALSE;\n\
         \n%s%s (NULL, &param);\n\
         if(param.status && query) return scm_makfrom0str (gstr->str);\
         if(gstr) g_string_free(gstr, TRUE);\nreturn SCM_BOOL(param.status);\n}\n", ni, fi, !(mi&CMD_CATEGORY_DIRECT)?"_cb":"");
      /****************** install the command in the hash table of commands (keymap) **************/
      fprintf(register_commands, "register_command(Denemo.map, gtk_action_group_get_action(action_group, \"%s\"), \"%s\", \"%s\", \"%s\", %s);\n",ni,ni, ml?ml:ni, ti?ti:ni,fi);
      /****************** install the command as an action in the menu system **************************/
      fprintf(entries,
	      "{\"%s\", %s, N_(\"%s\"), NULL,"
	      "N_(\"%s\"),"
	      "G_CALLBACK (%s%s)},\n",
	      ni, ii?ii:"NULL",ml?ml:ni, ti?ti:ni,fi,  (mi&CMD_CATEGORY_DIRECT)?"":"_cb");
    }
    else //no callback function - a menu rather than a menu item. It still needs to be added as an action in the menu system.
     fprintf(entries,
	      "{\"%s\", %s, N_(\"%s\"), NULL,"
	      "N_(\"%s\")},\n",
	      ni, ii?ii:"NULL",ml?ml:ni, ti?ti:ni);
  }

  /* generate source for duration callbacks - these were intercepted when
     typed at the keyboard to set prevailing rhythm, so the callback has to
     include code for this */

  for(i=0;i<9;i++) {
    /* callbacks for mode independent duration actions InsertRest0,1,2... ChangeRest0,1,2... InsertDur,ChangeDur0,1,2... SetDur0,1,2... */
    fprintf(callbacks, 
"static void InsertRest%d(GtkAction *action, gpointer param){\n"
"  DenemoGUI *gui = Denemo.gui;\n"
"  highlight_rest(gui, %d);\n"
"  gint mode = gui->mode;\n"
"  gui->mode = INPUTINSERT|INPUTREST;\n"
"  insert_chord_%dkey(gui);\n"
"  gui->mode = mode;\n"
"  score_status(gui, TRUE);\n"
"  displayhelper(gui);\n"
"}\n"

"static void ChangeRest%d(GtkAction *action, gpointer param){\n"
"  DenemoGUI *gui = Denemo.gui;\n"
"  gint mode = gui->mode;\n"
"  gboolean appending = gui->si->cursor_appending;\n"
"  if(appending)\n"
"    cursorleft(NULL);\n"
"  gui->mode = INPUTEDIT|INPUTREST;\n"
"  insert_chord_%dkey(gui);\n"
"  gui->mode = mode;\n"
"  if(appending)\n"
"    cursorright(NULL);\n"
"  score_status(gui, TRUE);\n"
"  displayhelper(gui);\n"
"}\n"

"void InsertDur%d(GtkAction *action, gpointer param){\n"
"  DenemoGUI *gui = Denemo.gui;\n"
"  highlight_duration(gui, %d);\n"
"  gint mode = gui->mode;\n"
"  gui->mode = INPUTINSERT|INPUTNORMAL;\n"
"  insert_chord_%dkey(gui);\n"
"  gui->mode = mode;\n"
"  score_status(gui, TRUE);\n"
"  displayhelper(gui);\n"
"}\n"

"static void ChangeDur%d(GtkAction *action, gpointer param){\n"
"  DenemoGUI *gui = Denemo.gui;\n"
"  gint mode = gui->mode;\n"
"  gboolean appending = gui->si->cursor_appending;\n"
"  if(appending)\n"
"    cursorleft(NULL);\n"
"  gui->mode = INPUTEDIT|INPUTNORMAL;\n"
"  insert_chord_%dkey(gui);\n"
"  gui->mode = mode;\n"
"  if(appending)\n"
"    cursorright(NULL);\n"
"  score_status(gui, TRUE);\n"
"  displayhelper(gui);\n"
"}\n"

"static void SetDur%d(GtkAction *action, gpointer param){\n"
"  DenemoGUI *gui = Denemo.gui;\n"
"  highlight_duration(gui, %d);\n"
"//  displayhelper(gui);\n"
"}\n"




,i,i,i,i,i,i,i,i,i,i, i, i);


    /* callbacks for mode sensitive  duration actions, Dur0,1,2 ... */
    fprintf(callbacks, 
	    "static void Dur%d  (GtkAction *action, gpointer param) {\n"
	    "  DenemoGUI *gui = Denemo.gui;\n"
	    " if(gui->mode&INPUTINSERT)\n"
	    "   highlight_duration(gui, %d);\n"
	    " else \n"
	    " if( (!gui->mode&INPUTRHYTHM) && (gui->mode&INPUTEDIT) && (!gui->si->cursor_appending))\n"
	    "   ChangeDur%d (action, param);\n"
	    "else {\n"
	    " insert_chord_%dkey(gui);\n"
	    "  score_status(gui, TRUE);\n"
	    " displayhelper(gui);\n"
	    " }\n"
	    "}\n", i , i, i, i);

    /* menu_entries for the mode sensitive duration actions, Dur0,1,2 ... */
    fprintf(entries,
  "{\"%d\", \"NULL\", N_(MUSIC_FONT(\"%d\")), NULL, N_(\"In insert mode, changes prevailing rhythm to \"MUSIC_FONT(\"%d\")\"\\nIn edit mode changes the current note to \"MUSIC_FONT(\"%d\")\"\\n or appends a \"MUSIC_FONT(\"%d\")\" if no current note\\nIn classic mode inserts a \"MUSIC_FONT(\"%d\")\" at the cursor\"),\n"
	    "G_CALLBACK (Dur%d)},\n"

  "{\"Change%d\", \"NULL\", N_(MUSIC_FONT(\"%d\")), NULL, N_(\"Change current note to a \"MUSIC_FONT(\"%d\")),\n"
	    "G_CALLBACK (ChangeDur%d)},\n"
  "{\"ChangeRest%d\", NULL, N_(\"Change duration\"), NULL, N_(\"Change durtion of current rest\"),\n"
    "G_CALLBACK (ChangeRest%d)},\n"
 "{\"Insert%d\", NULL, N_(\"Insert a \"MUSIC_FONT(\"%d\")\"\"), NULL, N_(\"Inserts a \"MUSIC_FONT(\"%d\")\" at cursor position\\nSets prevailing rhythm to \"MUSIC_FONT(\"%d\")),\n"
  "G_CALLBACK (InsertDur%d)},\n"
 "{\"InsertRest%d\", NULL, N_(\"Insert a \"MUSIC_FONT(\"%d\")\"rest\"), NULL, N_(\"Inserts a rest at cursor position\\nSets prevailing rhythm to \"MUSIC_FONT(\"%d\")),\n"
	    "G_CALLBACK (InsertRest%d)},\n"
 "{\"Set%d\", NULL, N_(\"Set Duration to \"MUSIC_FONT(\"%d\")\"\"), NULL, N_(\"Sets prevailing rhythm to \"MUSIC_FONT(\"%d\")),\n"
  "G_CALLBACK (SetDur%d)},\n"


     ,i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i,i,i  , i,i,i,i );
  }


  for(i=0;i<9;i++) 
    fprintf(xml, "<menuitem action=\"%d\"/>\n", i);
  for(i=0;i<9;i++) 
    fprintf(xml, "<menuitem action=\"Change%d\"/>\n", i);
  for(i=0;i<9;i++) 
    fprintf(xml, "<menuitem action=\"Insert%d\"/>\n", i);
  for(i=0;i<9;i++) 
    fprintf(xml, "<menuitem action=\"ChangeRest%d\"/>\n", i);
  for(i=0;i<9;i++) 
    fprintf(xml, "<menuitem action=\"InsertRest%d\"/>\n", i);
  
  for(i='A';i<='G';i++) 
    fprintf(xml, "<menuitem action=\"Insert%c\"/>\n", i);
  for(i='A';i<='G';i++) 
    fprintf(xml, "<menuitem action=\"ChangeTo%c\"/>\n", i);

  for(i=0;i<9;i++) 
    fprintf(xml, "<menuitem action=\"Set%d\"/>\n", i);

  /* menu_entries for the mode    note name    */
  for(i='A';i<='G';i++) {
    fprintf(entries,
" {\"Insert%c\", NULL, N_(\"Insert %c\"), NULL, N_(\"Inserts note %c before note at cursor\\nCursor determines which octave\\nNote is inserted in the prevailing rhythm\"),\n"
"  G_CALLBACK (Insert%c)},\n"
"  {\"ChangeTo%c\", NULL, N_(\"Change current note to %c\"), NULL, N_(\"Changes current note to the %c nearest cursor or (if no current note) inserts the note %c\\nCursor determines which octave\\nNote is inserted in the prevailing rhythm\"),\n"
"   G_CALLBACK (ChangeTo%c)},\n"
"  {\"MoveTo%c\", NULL, N_(\"Move cursor to step %c\"), NULL, N_(\"Moves the cursor to the %c nearest cursor\\nCurrent cursor position determines which octave.\"),\n"
"   G_CALLBACK (MoveTo%c)},\n"

	    ,i ,i ,i ,i ,i ,i ,i ,i ,i, i, i, i, i);

  }

  for(i='A';i<='G';i++) {
    fprintf(callbacks,
"static void ChangeTo%c(GtkAction *action, gpointer param){\n"
"  DenemoGUI *gui = Denemo.gui;\n"
"  gboolean appending = gui->si->cursor_appending;\n"
"  if(appending)\n"
"    cursorleft(NULL); \n"
"  gint mode = gui->mode;\n"
"  gui->mode = INPUTEDIT|INPUTNORMAL;\n"
"  go_to_%c_key(gui);\n"
"  gui->mode = mode;\n"
"  if(appending)\n"
"    cursorright(NULL);\n"
"  score_status(gui, TRUE);\n"
"  displayhelper(gui);\n"
"}\n", i, i);
  }


  for(i='A';i<='G';i++) {
    fprintf(callbacks,
"static void MoveTo%c(GtkAction *action, gpointer param){\n"
"  DenemoGUI *gui = Denemo.gui;\n"
"  gint mode = gui->mode;\n"
"  gui->mode = INPUTCLASSIC|INPUTNORMAL;\n"
"  go_to_%c_key(gui);\n"
"  gui->mode = mode;\n"
"  displayhelper(gui);\n"
"}\n", i, i);
  }


  for(i='A';i<='G';i++) {

    fprintf(callbacks,
"static void Insert%c(GtkAction *action, gpointer param){\n"
"  DenemoGUI *gui = Denemo.gui;\n"
"  gint mode = gui->mode;\n"
"  gui->mode = INPUTINSERT|INPUTNORMAL;\n"
"  go_to_%c_key(gui);\n"
"  gui->mode = mode;\n"
"  score_status(gui, TRUE);\n"
"  displayhelper(gui);\n"
	    "}\n", i, i);
  }

  for(i='A';i<='G';i++) {
    fprintf(register_commands,
	    "register_command(Denemo.map, gtk_action_group_get_action(action_group, \"Insert%c\"), \"Insert%c\", \"Insert %c\",\"Inserts note %c before note at cursor\\nCursor determines which octave\\nNote is inserted in the prevailing rhythm\",  Insert%c);\n", i,i,i,i,i);
      fprintf(scheme, "g_object_set_data(G_OBJECT(action_of_name(Denemo.map, \"Insert%c\")), \"scm\", (gpointer)1);\n", i); //define a property "scm" on the action to mean scheme can call the action.
      fprintf(scheme, "SCM scheme_Insert%c(SCM optional);\ninstall_scm_function (\"d-Insert%c\", scheme_Insert%c);\n", i, i, i);// for direct callback via (scheme_xxx)
      fprintf(scheme_cb, "SCM scheme_Insert%c (SCM optional) {\nInsert%c (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i,  i);



    fprintf(register_commands,
	    "register_command(Denemo.map, gtk_action_group_get_action(action_group, \"ChangeTo%c\"), \"ChangeTo%c\", N_(\"Change to %c\"),N_(\"Changes note at cursor to nearest note %c\\nRhythm is unchanged\"),  ChangeTo%c);\n", i,i,i,i,i);
      fprintf(scheme, "g_object_set_data(G_OBJECT(action_of_name(Denemo.map, \"ChangeTo%c\")), \"scm\", (gpointer)1);\n", i); //define a property "scm" on the action to mean scheme can call the action.
      fprintf(scheme, "SCM scheme_ChangeTo%c(SCM optional);\ninstall_scm_function (\"d-ChangeTo%c\", scheme_ChangeTo%c);\n", i, i, i);// for direct callback via (scheme_xxx)
      fprintf(scheme_cb, "SCM scheme_ChangeTo%c (SCM optional) {\nChangeTo%c (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i,  i);


    fprintf(register_commands,
	    "register_command(Denemo.map, gtk_action_group_get_action(action_group, \"MoveTo%c\"), \"MoveTo%c\", N_(\"Move to %c\"),N_(\"Moves cursor to nearest note %c\"),  MoveTo%c);\n", i,i,i,i,i);
      fprintf(scheme, "g_object_set_data(G_OBJECT(action_of_name(Denemo.map, \"MoveTo%c\")), \"scm\", (gpointer)1);\n", i); //define a property "scm" on the action to mean scheme can call the action.
      fprintf(scheme, "SCM scheme_MoveTo%c(SCM optional);\ninstall_scm_function (\"d-MoveTo%c\", scheme_MoveTo%c);\n", i, i, i);// for direct callback via (scheme_xxx)
      fprintf(scheme_cb, "SCM scheme_MoveTo%c (SCM optional) {\nMoveTo%c (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i,  i);

  }

  for(i=0;i<9;i++) {
    /* registering commands for mode independent duration actions InsertRest0,1,2... ChangeRest0,1,2... InsertDur,ChangeDur0,1,2... */
    fprintf(register_commands, 
	    "register_command(Denemo.map, gtk_action_group_get_action(action_group, \"%d\"), \"%d\", N_(MUSIC_FONT(\"%d\")), N_(\"In insert mode, changes prevailing rhythm to \"MUSIC_FONT(\"%d\")\"\\nIn edit mode changes the current note to \"MUSIC_FONT(\"%d\")\"\\n or appends a \"MUSIC_FONT(\"%d\")\" if no current note\\nIn classic mode inserts a \"MUSIC_FONT(\"%d\")\" at the cursor\"), Dur%d);\n", i, i, i, i, i, i, i, i);

    fprintf(register_commands, 
	    "register_command(Denemo.map, gtk_action_group_get_action(action_group, \"Change%d\"), \"Change%d\", N_(MUSIC_FONT(\"%d\")), N_(\"Change the current note to a \"MUSIC_FONT(\"%d\")), ChangeDur%d);\n", i, i, i, i, i);

    fprintf(register_commands, 
	    "register_command(Denemo.map, gtk_action_group_get_action(action_group, \"Insert%d\"), \"Insert%d\", N_(MUSIC_FONT(\"%d\")), N_(\"Insert a \"MUSIC_FONT(\"%d\")), InsertDur%d);\n", i, i, i, i, i);

    fprintf(register_commands, 
	    "register_command(Denemo.map, gtk_action_group_get_action(action_group, \"InsertRest%d\"), \"InsertRest%d\",  N_(\"Insert a \"MUSIC_FONT(\"%d\")\"rest\") ,  N_(\"Inserts a rest at cursor position\\nSets prevailing rhythm to \"MUSIC_FONT(\"%d\")), InsertRest%d);\n", i, i, i, i, i);

    fprintf(register_commands, 
	    "register_command(Denemo.map, gtk_action_group_get_action(action_group, \"ChangeRest%d\"), \"ChangeRest%d\",  N_(\"Change a \"MUSIC_FONT(\"%d\")\"rest\") ,  N_(\"Changes a rest at cursor position\\nSets prevailing rhythm to \"MUSIC_FONT(\"%d\")), ChangeRest%d);\n", i, i, i, i, i);

    fprintf(register_commands, 
	    "register_command(Denemo.map, gtk_action_group_get_action(action_group, \"Set%d\"), \"Set%d\", N_(MUSIC_FONT(\"%d\")), N_(\"Set the prevailing duration to \"MUSIC_FONT(\"%d\")), SetDur%d);\n", i, i, i, i, i);



      fprintf(scheme, "/*%d */\n", i);
      fprintf(scheme, "g_object_set_data(G_OBJECT(action_of_name(Denemo.map, \"%d\")), \"scm\", (gpointer)1);\n", i); //define a property "scm" on the action to mean scheme can call the action.

      fprintf(scheme, "SCM scheme_%d(SCM optional);\ninstall_scm_function (\"d-%d\", scheme_%d);\n", i, i, i);// for direct callback via (scheme_xxx)
      fprintf(scheme_cb, "SCM scheme_%d (SCM optional) {\nDur%d (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i,  i);

      fprintf(scheme, "g_object_set_data(G_OBJECT(action_of_name(Denemo.map, \"Insert%d\")), \"scm\", (gpointer)1);\n", i); //define a property "scm" on the action to mean scheme can call the action.
      fprintf(scheme, "SCM scheme_InsertDur%d(SCM optional);\ninstall_scm_function (\"d-Insert%d\", scheme_InsertDur%d);\n", i, i, i);// for direct callback via (scheme_xxx)
      fprintf(scheme_cb, "SCM scheme_InsertDur%d (SCM optional) {\nInsertDur%d (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i,  i);



      fprintf(scheme, "g_object_set_data(G_OBJECT(action_of_name(Denemo.map, \"Change%d\")), \"scm\", (gpointer)1);\n", i); //define a property "scm" on the action to mean scheme can call the action.
      fprintf(scheme, "SCM scheme_ChangeDur%d(SCM optional);\ninstall_scm_function (\"d-Change%d\", scheme_ChangeDur%d);\n", i, i, i);// for direct callback via (scheme_xxx)
      fprintf(scheme_cb, "SCM scheme_ChangeDur%d (SCM optional) {\nChangeDur%d (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i,  i);

      fprintf(scheme, "g_object_set_data(G_OBJECT(action_of_name(Denemo.map, \"Set%d\")), \"scm\", (gpointer)1);\n", i); //define a property "scm" on the action to mean scheme can call the action.
      fprintf(scheme, "SCM scheme_SetDur%d(SCM optional);\ninstall_scm_function (\"d-Set%d\", scheme_SetDur%d);\n", i, i, i);// for direct callback via (scheme_xxx)
      fprintf(scheme_cb, "SCM scheme_SetDur%d (SCM optional) {\nSetDur%d (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i,  i);



      fprintf(scheme, "g_object_set_data(G_OBJECT(action_of_name(Denemo.map, \"InsertRest%d\")), \"scm\", (gpointer)1);\n", i); //define a property "scm" on the action to mean scheme can call the action.
      fprintf(scheme, "SCM scheme_InsertRest%d(SCM optional);\ninstall_scm_function (\"d-InsertRest%d\", scheme_InsertRest%d);\n", i, i, i);// for direct callback via (scheme_xxx)
      fprintf(scheme_cb, "SCM scheme_InsertRest%d (SCM optional) {\nInsertRest%d (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i,  i);

      fprintf(scheme, "g_object_set_data(G_OBJECT(action_of_name(Denemo.map, \"ChangeRest%d\")), \"scm\", (gpointer)1);\n", i); //define a property "scm" on the action to mean scheme can call the action.
      fprintf(scheme, "SCM scheme_ChangeRest%d(SCM optional);\ninstall_scm_function (\"d-ChangeRest%d\", scheme_ChangeRest%d);\n", i, i, i);// for direct callback via (scheme_xxx)
      fprintf(scheme_cb, "SCM scheme_ChangeRest%d (SCM optional) {\nChangeRest%d (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i,  i);
  }



#ifdef GENERATE_XML_FRAGMENT
  fprintf(xml, "<menu action=\"AllOther\">\n");
  for(j=0;j<9;j++){
    fprintf(xml, "<menu action=\"%s\">\n", catname[j]);
    for(i=0;i<n_denemo_commands;i++) {
      if(mi == j) {
	fprintf(xml, "<menuitem action=\"%s\"/>\n", ni);
      }
      
    }
    fprintf(xml, "</menu>\n");
  }
  fprintf(xml, "</menu>\n");
#endif
  return 0;      
}
