SCM scheme_CursorLeft (SCM optional) {
  return scheme_call_callback(optional, cursorleft);
}
SCM scheme_MoveCursorLeft (SCM optional) {
  return scheme_call_callback(optional, movecursorleft);
}
SCM scheme_CursorDown (SCM optional) {
  return scheme_call_callback(optional, cursordown);
}
SCM scheme_CursorUp (SCM optional) {
  return scheme_call_callback(optional, cursorup);
}
SCM scheme_CursorRight (SCM optional) {
  return scheme_call_callback(optional, cursorright);
}
SCM scheme_MoveCursorRight (SCM optional) {
  return scheme_call_callback(optional, movecursorright);
}
SCM scheme_GoToMark (SCM optional) {
  return scheme_call_callback(optional, goto_mark);
}
SCM scheme_SwapPointAndMark (SCM optional) {
  return scheme_call_callback(optional, swap_point_and_mark);
}
SCM scheme_GoToSelectionStart (SCM optional) {
  return scheme_call_callback(optional, goto_selection_start);
}
SCM scheme_PushPosition (SCM optional) {
  return scheme_call_callback(optional, PushPosition);
}
SCM scheme_PopPosition (SCM optional) {
  return scheme_call_callback(optional, PopPosition);
}
SCM scheme_PopPushPosition (SCM optional) {
  return scheme_call_callback(optional, PopPushPosition);
}
SCM scheme_ToggleReduceToDrawingArea (SCM optional) {
  return scheme_call_callback(optional, ToggleReduceToDrawingArea);
}
SCM scheme_StaffUp (SCM optional) {
  return scheme_call_callback(optional, staffup);
}
SCM scheme_StaffDown (SCM optional) {
  return scheme_call_callback(optional, staffdown);
}
SCM scheme_MoveToStaffUp (SCM optional) {
  return scheme_call_callback(optional, movetostaffup);
}
SCM scheme_MoveToStaffDown (SCM optional) {
  return scheme_call_callback(optional, movetostaffdown);
}
SCM scheme_MeasureLeft (SCM optional) {
  return scheme_call_callback(optional, measureleft);
}
SCM scheme_MeasureRight (SCM optional) {
  return scheme_call_callback(optional, measureright);
}
SCM scheme_MoveToMeasureLeft (SCM optional) {
  return scheme_call_callback(optional, movetomeasureleft);
}
SCM scheme_MoveToMeasureRight (SCM optional) {
  return scheme_call_callback(optional, movetomeasureright);
}
SCM scheme_A (SCM optional) {
  return scheme_call_callback(optional, go_to_A_key);
}
SCM scheme_B (SCM optional) {
  return scheme_call_callback(optional, go_to_B_key);
}
SCM scheme_C (SCM optional) {
  return scheme_call_callback(optional, go_to_C_key);
}
SCM scheme_D (SCM optional) {
  return scheme_call_callback(optional, go_to_D_key);
}
SCM scheme_E (SCM optional) {
  return scheme_call_callback(optional, go_to_E_key);
}
SCM scheme_F (SCM optional) {
  return scheme_call_callback(optional, go_to_F_key);
}
SCM scheme_G (SCM optional) {
  return scheme_call_callback(optional, go_to_G_key);
}
SCM scheme_OctaveUp (SCM optional) {
  return scheme_call_callback(optional, octave_up_key);
}
SCM scheme_OctaveDown (SCM optional) {
  return scheme_call_callback(optional, octave_down_key);
}
SCM scheme_WholeNote (SCM optional) {
  return scheme_call_callback(optional, insert_chord_0key);
}
SCM scheme_HalfNote (SCM optional) {
  return scheme_call_callback(optional, insert_chord_1key);
}
SCM scheme_QuarterNote (SCM optional) {
  return scheme_call_callback(optional, insert_chord_2key);
}
SCM scheme_EighthNote (SCM optional) {
  return scheme_call_callback(optional, insert_chord_3key);
}
SCM scheme_SixteenthNote (SCM optional) {
  return scheme_call_callback(optional, insert_chord_4key);
}
SCM scheme_ThirtysecondNote (SCM optional) {
  return scheme_call_callback(optional, insert_chord_5key);
}
SCM scheme_SixtyfourthNote (SCM optional) {
  return scheme_call_callback(optional, insert_chord_6key);
}
SCM scheme_OneHundredTwentyEighthNote (SCM optional) {
  return scheme_call_callback(optional, insert_chord_7key);
}
SCM scheme_TwoHundredFiftySixthNote (SCM optional) {
  return scheme_call_callback(optional, insert_chord_8key);
}
SCM scheme_InsertWholeRest (SCM optional) {
  return scheme_call_callback(optional, insert_rest_0key);
}
SCM scheme_InsertHalfRest (SCM optional) {
  return scheme_call_callback(optional, insert_rest_1key);
}
SCM scheme_InsertQuarterRest (SCM optional) {
  return scheme_call_callback(optional, insert_rest_2key);
}
SCM scheme_InsertEighthRest (SCM optional) {
  return scheme_call_callback(optional, insert_rest_3key);
}
SCM scheme_InsertSixteenthRest (SCM optional) {
  return scheme_call_callback(optional, insert_rest_4key);
}
SCM scheme_InsertThirtysecondRest (SCM optional) {
  return scheme_call_callback(optional, insert_rest_5key);
}
SCM scheme_InsertSixtyfourthRest (SCM optional) {
  return scheme_call_callback(optional, insert_rest_6key);
}
SCM scheme_InsertBlankWholeNote (SCM optional) {
  return scheme_call_callback(optional, insert_blankchord_0key);
}
SCM scheme_InsertBlankHalfNote (SCM optional) {
  return scheme_call_callback(optional, insert_blankchord_1key);
}
SCM scheme_InsertBlankQuarterNote (SCM optional) {
  return scheme_call_callback(optional, insert_blankchord_2key);
}
SCM scheme_InsertBlankEighthNote (SCM optional) {
  return scheme_call_callback(optional, insert_blankchord_3key);
}
SCM scheme_InsertBlankSixteenthNote (SCM optional) {
  return scheme_call_callback(optional, insert_blankchord_4key);
}
SCM scheme_InsertBlankThirtysecondNote (SCM optional) {
  return scheme_call_callback(optional, insert_blankchord_5key);
}
SCM scheme_InsertBlankSixtyfourthNote (SCM optional) {
  return scheme_call_callback(optional, insert_blankchord_6key);
}
SCM scheme_InsertBlankOneHundredTwentyEighthNote (SCM optional) {
  return scheme_call_callback(optional, insert_blankchord_7key);
}
SCM scheme_InsertBlankTwoHundredFiftySixthNote (SCM optional) {
  return scheme_call_callback(optional, insert_blankchord_8key);
}
SCM scheme_ToggleRestMode (SCM optional) {
  return scheme_call_callback(optional, rest_toggle_key);
}
SCM scheme_ToggleBlankMode (SCM optional) {
  return scheme_call_callback(optional, toggle_blank);
}
SCM scheme_InsertDuplet (SCM optional) {
  return scheme_call_callback(optional, duplet_insert);
}
SCM scheme_InsertTriplet (SCM optional) {
  return scheme_call_callback(optional, triplet_insert);
}
SCM scheme_StartTriplet (SCM optional) {
  return scheme_call_callback(optional, triplet_start);
}
SCM scheme_EndTuplet (SCM optional) {
  return scheme_call_callback(optional, tuplet_end);
}
SCM scheme_InsertQuadtuplet (SCM optional) {
  return scheme_call_callback(optional, insert_quadtuplet);
}
SCM scheme_InsertQuintuplet (SCM optional) {
  return scheme_call_callback(optional, quintuplet_insert);
}
SCM scheme_InsertSextuplet (SCM optional) {
  return scheme_call_callback(optional, sextuplet_insert);
}
SCM scheme_InsertSeptuplet (SCM optional) {
  return scheme_call_callback(optional, septuplet_insert);
}
SCM scheme_AddNoteToChord (SCM optional) {
  return scheme_call_callback(optional, add_tone_key);
}
SCM scheme_RemoveNoteFromChord (SCM optional) {
  return scheme_call_callback(optional, remove_tone_key);
}
SCM scheme_Sharpen (SCM optional) {
  return scheme_call_callback(optional, sharpen_key);
}
SCM scheme_Flatten (SCM optional) {
  return scheme_call_callback(optional, flatten_key);
}
SCM scheme_PendingSharpen (SCM optional) {
  return scheme_call_callback(optional, pending_sharpen);
}
SCM scheme_PendingFlatten (SCM optional) {
  return scheme_call_callback(optional, pending_flatten);
}
SCM scheme_StemUp (SCM optional) {
  return scheme_call_callback(optional, stem_up);
}
SCM scheme_StemDown (SCM optional) {
  return scheme_call_callback(optional, stem_down);
}
SCM scheme_AddDot (SCM optional) {
  return scheme_call_callback(optional, add_dot_key);
}
SCM scheme_RemoveDot (SCM optional) {
  return scheme_call_callback(optional, remove_dot_key);
}
SCM scheme_InsertTiedNote (SCM optional) {
  return scheme_call_callback(optional, tie_notes_key);
}
SCM scheme_ToggleTie (SCM optional) {
  return scheme_call_callback(optional, toggle_tie);
}
SCM scheme_DeleteObject (SCM optional) {
  return scheme_call_callback(optional, deleteobject);
}
SCM scheme_DeletePreviousObject (SCM optional) {
  return scheme_call_callback(optional, deletepreviousobject);
}
SCM scheme_InsertMeasure (SCM optional) {
  return scheme_call_callback(optional, insert_measure_key);
}
SCM scheme_AddMeasure (SCM optional) {
  return scheme_call_callback(optional, addmeasureafter);
}
SCM scheme_InsertMeasureBefore (SCM optional) {
  return scheme_call_callback(optional, insertmeasurebefore);
}
SCM scheme_InsertMeasureAfter (SCM optional) {
  return scheme_call_callback(optional, insertmeasureafter);
}
SCM scheme_AppendMeasure (SCM optional) {
  return scheme_call_callback(optional, append_measure_key);
}
SCM scheme_DeleteMeasure (SCM optional) {
  return scheme_call_callback(optional, deletemeasure);
}
SCM scheme_DeleteMeasureAllStaffs (SCM optional) {
  return scheme_call_callback(optional, deletemeasureallstaffs);
}
SCM scheme_ShrinkMeasures (SCM optional) {
  return scheme_call_callback(optional, adjust_measure_less_width_key);
}
SCM scheme_WidenMeasures (SCM optional) {
  return scheme_call_callback(optional, adjust_measure_more_width_key);
}
SCM scheme_ShorterStaffs (SCM optional) {
  return scheme_call_callback(optional, adjust_staff_less_height_key);
}
SCM scheme_TallerStaffs (SCM optional) {
  return scheme_call_callback(optional, adjust_staff_more_height_key);
}
SCM scheme_InsertTrebleClef (SCM optional) {
  return scheme_call_callback(optional, clef_new_treble);
}
SCM scheme_InsertBassClef (SCM optional) {
  return scheme_call_callback(optional, clef_new_bass);
}
SCM scheme_Insertg8clef (SCM optional) {
  return scheme_call_callback(optional, clef_new_g8);
}
SCM scheme_InsertAltoClef (SCM optional) {
  return scheme_call_callback(optional, clef_new_alto);
}
SCM scheme_InsertTenorClef (SCM optional) {
  return scheme_call_callback(optional, clef_new_tenor);
}
SCM scheme_InsertSopranoClef (SCM optional) {
  return scheme_call_callback(optional, clef_new_soprano);
}
SCM scheme_SetInitialTrebleClef (SCM optional) {
  return scheme_call_callback(optional, clef_set_treble);
}
SCM scheme_SetInitialBassClef (SCM optional) {
  return scheme_call_callback(optional, clef_set_bass);
}
SCM scheme_SetInitialg8clef (SCM optional) {
  return scheme_call_callback(optional, clef_set_g8);
}
SCM scheme_SetInitialAltoClef (SCM optional) {
  return scheme_call_callback(optional, clef_set_alto);
}
SCM scheme_SetInitialTenorClef (SCM optional) {
  return scheme_call_callback(optional, clef_set_tenor);
}
SCM scheme_SetInitialSopranoClef (SCM optional) {
  return scheme_call_callback(optional, clef_set_soprano);
}
SCM scheme_Insert22Time (SCM optional) {
  return scheme_call_callback(optional, newtimesig22);
}
SCM scheme_Insert32Time (SCM optional) {
  return scheme_call_callback(optional, newtimesig32);
}
SCM scheme_Insert42Time (SCM optional) {
  return scheme_call_callback(optional, newtimesig42);
}
SCM scheme_Insert44Time (SCM optional) {
  return scheme_call_callback(optional, newtimesig44);
}
SCM scheme_Insert34Time (SCM optional) {
  return scheme_call_callback(optional, newtimesig34);
}
SCM scheme_Insert24Time (SCM optional) {
  return scheme_call_callback(optional, newtimesig24);
}
SCM scheme_Insert64Time (SCM optional) {
  return scheme_call_callback(optional, newtimesig64);
}
SCM scheme_Insert38Time (SCM optional) {
  return scheme_call_callback(optional, newtimesig38);
}
SCM scheme_Insert68Time (SCM optional) {
  return scheme_call_callback(optional, newtimesig68);
}
SCM scheme_Insert128Time (SCM optional) {
  return scheme_call_callback(optional, newtimesig128);
}
SCM scheme_Insert98Time (SCM optional) {
  return scheme_call_callback(optional, newtimesig98);
}
SCM scheme_Set22Time (SCM optional) {
  return scheme_call_callback(optional, settimesig22);
}
SCM scheme_Set32Time (SCM optional) {
  return scheme_call_callback(optional, settimesig32);
}
SCM scheme_Set42Time (SCM optional) {
  return scheme_call_callback(optional, settimesig42);
}
SCM scheme_Set44Time (SCM optional) {
  return scheme_call_callback(optional, settimesig44);
}
SCM scheme_Set34Time (SCM optional) {
  return scheme_call_callback(optional, settimesig34);
}
SCM scheme_Set24Time (SCM optional) {
  return scheme_call_callback(optional, settimesig24);
}
SCM scheme_Set64Time (SCM optional) {
  return scheme_call_callback(optional, settimesig64);
}
SCM scheme_Set38Time (SCM optional) {
  return scheme_call_callback(optional, settimesig38);
}
SCM scheme_Set68Time (SCM optional) {
  return scheme_call_callback(optional, settimesig68);
}
SCM scheme_Set128Time (SCM optional) {
  return scheme_call_callback(optional, settimesig128);
}
SCM scheme_Set98Time (SCM optional) {
  return scheme_call_callback(optional, settimesig98);
}
SCM scheme_InsertCmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_new_cmaj);
}
SCM scheme_InsertGmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_new_gmaj);
}
SCM scheme_InsertDmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_new_dmaj);
}
SCM scheme_InsertAmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_new_amaj);
}
SCM scheme_InsertEmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_new_emaj);
}
SCM scheme_InsertBmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_new_bmaj);
}
SCM scheme_InsertFSharpmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_new_fsharpmaj);
}
SCM scheme_InsertCSharpmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_new_csharpmaj);
}
SCM scheme_InsertFmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_new_fmaj);
}
SCM scheme_InsertBflatmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_new_bflatmaj);
}
SCM scheme_InsertEflatmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_new_eflatmaj);
}
SCM scheme_InsertAflatmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_new_aflatmaj);
}
SCM scheme_InsertDflatmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_new_dflatmaj);
}
SCM scheme_InsertGflatmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_new_gflatmaj);
}
SCM scheme_InsertCflatmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_new_cflatmaj);
}
SCM scheme_InsertAmin (SCM optional) {
  return scheme_call_callback(optional, keysig_new_amin);
}
SCM scheme_InsertEmin (SCM optional) {
  return scheme_call_callback(optional, keysig_new_emin);
}
SCM scheme_InsertBmin (SCM optional) {
  return scheme_call_callback(optional, keysig_new_bmin);
}
SCM scheme_InsertFSharpmin (SCM optional) {
  return scheme_call_callback(optional, keysig_new_fsharpmin);
}
SCM scheme_InsertCSharpmin (SCM optional) {
  return scheme_call_callback(optional, keysig_new_csharpmin);
}
SCM scheme_InsertGSharpmin (SCM optional) {
  return scheme_call_callback(optional, keysig_new_gsharpmin);
}
SCM scheme_InsertDSharpmin (SCM optional) {
  return scheme_call_callback(optional, keysig_new_dsharpmin);
}
SCM scheme_InsertASharpmin (SCM optional) {
  return scheme_call_callback(optional, keysig_new_asharpmin);
}
SCM scheme_InsertDmin (SCM optional) {
  return scheme_call_callback(optional, keysig_new_dmin);
}
SCM scheme_InsertGmin (SCM optional) {
  return scheme_call_callback(optional, keysig_new_gmin);
}
SCM scheme_InsertCmin (SCM optional) {
  return scheme_call_callback(optional, keysig_new_cmin);
}
SCM scheme_InsertFmin (SCM optional) {
  return scheme_call_callback(optional, keysig_new_fmin);
}
SCM scheme_InsertBflatmin (SCM optional) {
  return scheme_call_callback(optional, keysig_new_bflatmin);
}
SCM scheme_InsertEflatmin (SCM optional) {
  return scheme_call_callback(optional, keysig_new_eflatmin);
}
SCM scheme_InsertAflatmin (SCM optional) {
  return scheme_call_callback(optional, keysig_new_aflatmin);
}
SCM scheme_SetInitialCmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_set_cmaj);
}
SCM scheme_SetInitialGmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_set_gmaj);
}
SCM scheme_SetInitialDmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_set_dmaj);
}
SCM scheme_SetInitialAmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_set_amaj);
}
SCM scheme_SetInitialEmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_set_emaj);
}
SCM scheme_SetInitialBmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_set_bmaj);
}
SCM scheme_SetInitialFSharpmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_set_fsharpmaj);
}
SCM scheme_SetInitialCSharpmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_set_csharpmaj);
}
SCM scheme_SetInitialFmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_set_fmaj);
}
SCM scheme_SetInitialBflatmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_set_bflatmaj);
}
SCM scheme_SetInitialEflatmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_set_eflatmaj);
}
SCM scheme_SetInitialAflatmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_set_aflatmaj);
}
SCM scheme_SetInitialDflatmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_set_dflatmaj);
}
SCM scheme_SetInitialGflatmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_set_gflatmaj);
}
SCM scheme_SetInitialCflatmaj (SCM optional) {
  return scheme_call_callback(optional, keysig_set_cflatmaj);
}
SCM scheme_SetInitialAmin (SCM optional) {
  return scheme_call_callback(optional, keysig_set_amin);
}
SCM scheme_SetInitialEmin (SCM optional) {
  return scheme_call_callback(optional, keysig_set_emin);
}
SCM scheme_SetInitialBmin (SCM optional) {
  return scheme_call_callback(optional, keysig_set_bmin);
}
SCM scheme_SetInitialFSharpmin (SCM optional) {
  return scheme_call_callback(optional, keysig_set_fsharpmin);
}
SCM scheme_SetInitialCSharpmin (SCM optional) {
  return scheme_call_callback(optional, keysig_set_csharpmin);
}
SCM scheme_SetInitialGSharpmin (SCM optional) {
  return scheme_call_callback(optional, keysig_set_gsharpmin);
}
SCM scheme_SetInitialDSharpmin (SCM optional) {
  return scheme_call_callback(optional, keysig_set_dsharpmin);
}
SCM scheme_SetInitialASharpmin (SCM optional) {
  return scheme_call_callback(optional, keysig_set_asharpmin);
}
SCM scheme_SetInitialDmin (SCM optional) {
  return scheme_call_callback(optional, keysig_set_dmin);
}
SCM scheme_SetInitialGmin (SCM optional) {
  return scheme_call_callback(optional, keysig_set_gmin);
}
SCM scheme_SetInitialCmin (SCM optional) {
  return scheme_call_callback(optional, keysig_set_cmin);
}
SCM scheme_SetInitialFmin (SCM optional) {
  return scheme_call_callback(optional, keysig_set_fmin);
}
SCM scheme_SetInitialBflatmin (SCM optional) {
  return scheme_call_callback(optional, keysig_set_bflatmin);
}
SCM scheme_SetInitialEflatmin (SCM optional) {
  return scheme_call_callback(optional, keysig_set_eflatmin);
}
SCM scheme_SetInitialAflatmin (SCM optional) {
  return scheme_call_callback(optional, keysig_set_aflatmin);
}
SCM scheme_SetMark (SCM optional) {
  return scheme_call_callback(optional, set_mark);
}
SCM scheme_UnsetMark (SCM optional) {
  return scheme_call_callback(optional, unset_mark);
}
SCM scheme_SetPoint (SCM optional) {
  return scheme_call_callback(optional, set_point);
}
SCM scheme_ToggleBeginSlur (SCM optional) {
  return scheme_call_callback(optional, toggle_begin_slur);
}
SCM scheme_ToggleEndSlur (SCM optional) {
  return scheme_call_callback(optional, toggle_end_slur);
}
SCM scheme_ToggleStartCrescendo (SCM optional) {
  return scheme_call_callback(optional, toggle_start_crescendo);
}
SCM scheme_ToggleEndCrescendo (SCM optional) {
  return scheme_call_callback(optional, toggle_end_crescendo);
}
SCM scheme_ToggleStartDiminuendo (SCM optional) {
  return scheme_call_callback(optional, toggle_start_diminuendo);
}
SCM scheme_ToggleEndDiminuendo (SCM optional) {
  return scheme_call_callback(optional, toggle_end_diminuendo);
}
SCM scheme_ToggleGrace (SCM optional) {
  return scheme_call_callback(optional, toggle_grace);
}
SCM scheme_ToggleAcciaccatura (SCM optional) {
  return scheme_call_callback(optional, toggle_acciaccatura);
}
SCM scheme_ForceCaution (SCM optional) {
  return scheme_call_callback(optional, force_cautionary);
}
SCM scheme_ChangePitch (SCM optional) {
  return scheme_call_callback(optional, change_pitch);
}
SCM scheme_InsertRhythm (SCM optional) {
  return scheme_call_callback(optional, insert_rhythm_pattern);
}
SCM scheme_NextRhythm (SCM optional) {
  return scheme_call_callback(optional, nextrhythm);
}
SCM scheme_AppendMeasureAllStaffs (SCM optional) {
  return scheme_call_callback(optional, append_measure_score);
}
SCM scheme_ExecuteScheme (SCM optional) {
  return scheme_call_callback(optional, execute_scheme);
}
SCM scheme_SharpenEnharmonicSet (SCM optional) {
  return scheme_call_callback(optional, set_sharper);
}
SCM scheme_FlattenEnharmonicSet (SCM optional) {
  return scheme_call_callback(optional, set_flatter);
}
SCM scheme_New (SCM optional) {
  return scheme_call_callback(optional, file_newwrapper);
}
SCM scheme_NewScore (SCM optional) {
  return scheme_call_callback(optional, new_score_cb);
}
SCM scheme_Open (SCM optional) {
  return scheme_call_callback(optional, file_open_with_check);
}
SCM scheme_ImportLilypond (SCM optional) {
  return scheme_call_callback(optional, file_import_lilypond_with_check);
}
SCM scheme_ImportMidi (SCM optional) {
  return scheme_call_callback(optional, file_import_midi_with_check);
}
SCM scheme_ImportMusicXml (SCM optional) {
  return scheme_call_callback(optional, file_import_musicxml_with_check);
}
SCM scheme_AddStaffs (SCM optional) {
  return scheme_call_callback(optional, file_add_staffs);
}
SCM scheme_AddMovements (SCM optional) {
  return scheme_call_callback(optional, file_add_movements);
}
SCM scheme_MovementProps (SCM optional) {
  return scheme_call_callback(optional, movement_props_dialog);
}
SCM scheme_OpenNewWindow (SCM optional) {
  return scheme_call_callback(optional, openinnew);
}
SCM scheme_Save (SCM optional) {
  return scheme_call_callback(optional, file_savewrapper);
}
SCM scheme_SaveAs (SCM optional) {
  return scheme_call_callback(optional, file_saveaswrapper);
}
SCM scheme_SaveCopy (SCM optional) {
  return scheme_call_callback(optional, file_copy_save);
}
SCM scheme_OpenTemplate (SCM optional) {
  return scheme_call_callback(optional, system_template_open_with_check);
}
SCM scheme_OpenExample (SCM optional) {
  return scheme_call_callback(optional, system_example_open_with_check);
}
SCM scheme_OpenMyTemplate (SCM optional) {
  return scheme_call_callback(optional, local_template_open_with_check);
}
SCM scheme_SaveTemplate (SCM optional) {
  return scheme_call_callback(optional, template_save);
}
SCM scheme_NewWindow (SCM optional) {
  return scheme_call_callback(optional, newview);
}
SCM scheme_InsertMovementBefore (SCM optional) {
  return scheme_call_callback(optional, insert_movement_before);
}
SCM scheme_InsertMovementAfter (SCM optional) {
  return scheme_call_callback(optional, insert_movement_after);
}
SCM scheme_NewMovement (SCM optional) {
  return scheme_call_callback(optional, append_new_movement);
}
SCM scheme_SaveParts (SCM optional) {
  return scheme_call_callback(optional, file_savepartswrapper);
}
SCM scheme_ExportMUDELA (SCM optional) {
  return scheme_call_callback(optional, export_mudela_action);
}
SCM scheme_ExportPDF (SCM optional) {
  return scheme_call_callback(optional, export_pdf_action);
}
SCM scheme_ExportPNG (SCM optional) {
  return scheme_call_callback(optional, export_png_action);
}
SCM scheme_ExportMIDI (SCM optional) {
  return scheme_call_callback(optional, export_midi_action);
}
SCM scheme_PrintView (SCM optional) {
  return scheme_call_callback(optional, show_print_view);
}
SCM scheme_PrintSelection (SCM optional) {
  return scheme_call_callback(optional, printselection_cb);
}
SCM scheme_PrintExcerptPreview (SCM optional) {
  return scheme_call_callback(optional, printexcerptpreview_cb);
}
SCM scheme_PrintMovement (SCM optional) {
  return scheme_call_callback(optional, printmovement_cb);
}
SCM scheme_Print (SCM optional) {
  return scheme_call_callback(optional, printall_cb);
}
SCM scheme_PrintPart (SCM optional) {
  return scheme_call_callback(optional, printpart_cb);
}
SCM scheme_Close (SCM optional) {
  return scheme_call_callback(optional, close_gui_with_check);
}
SCM scheme_Quit (SCM optional) {
  return scheme_call_callback(optional, closewrapper);
}
SCM scheme_Undo (SCM optional) {
  return scheme_call_callback(optional, undowrapper);
}
SCM scheme_Redo (SCM optional) {
  return scheme_call_callback(optional, redowrapper);
}
SCM scheme_Copy (SCM optional) {
  return scheme_call_callback(optional, copywrapper);
}
SCM scheme_Cut (SCM optional) {
  return scheme_call_callback(optional, cutwrapper);
}
SCM scheme_Paste (SCM optional) {
  return scheme_call_callback(optional, pastewrapper);
}
SCM scheme_PasteClipboard (SCM optional) {
  return scheme_call_callback(optional, paste_clipboard);
}
SCM scheme_ScoreProperties (SCM optional) {
  return scheme_call_callback(optional, score_properties_dialog);
}
SCM scheme_SaveSelection (SCM optional) {
  return scheme_call_callback(optional, saveselwrapper);
}
SCM scheme_Preferences (SCM optional) {
  return scheme_call_callback(optional, preferences_change);
}
SCM scheme_SaveAccels (SCM optional) {
  return scheme_call_callback(optional, save_default_keymap_file_wrapper);
}
SCM scheme_CommandManagement (SCM optional) {
  return scheme_call_callback(optional, configure_keyboard_dialog);
}
SCM scheme_SwapStaffs (SCM optional) {
  return scheme_call_callback(optional, swapstaffs);
}
SCM scheme_SplitVoices (SCM optional) {
  return scheme_call_callback(optional, splitstaffs);
}
SCM scheme_JoinVoices (SCM optional) {
  return scheme_call_callback(optional, joinstaffs);
}
SCM scheme_SwapMovements (SCM optional) {
  return scheme_call_callback(optional, swapmovements);
}
SCM scheme_VoiceUp (SCM optional) {
  return scheme_call_callback(optional, voiceup);
}
SCM scheme_VoiceDown (SCM optional) {
  return scheme_call_callback(optional, voicedown);
}
SCM scheme_MoveToVoiceUp (SCM optional) {
  return scheme_call_callback(optional, movetovoiceup);
}
SCM scheme_MoveToVoiceDown (SCM optional) {
  return scheme_call_callback(optional, movetovoicedown);
}
SCM scheme_AddBefore (SCM optional) {
  return scheme_call_callback(optional, staff_new_before);
}
SCM scheme_AddAfter (SCM optional) {
  return scheme_call_callback(optional, staff_new_after);
}
SCM scheme_AddInitial (SCM optional) {
  return scheme_call_callback(optional, staff_new_initial);
}
SCM scheme_AddLast (SCM optional) {
  return scheme_call_callback(optional, staff_new_last);
}
SCM scheme_DeleteBefore (SCM optional) {
  return scheme_call_callback(optional, delete_staff_before);
}
SCM scheme_DeleteStaff (SCM optional) {
  return scheme_call_callback(optional, delete_staff_current);
}
SCM scheme_DeleteAfter (SCM optional) {
  return scheme_call_callback(optional, delete_staff_after);
}
SCM scheme_AddVoice (SCM optional) {
  return scheme_call_callback(optional, staff_new_voice);
}
SCM scheme_StaffProperties (SCM optional) {
  return scheme_call_callback(optional, staff_properties_change_cb);
}
SCM scheme_InitialClef (SCM optional) {
  return scheme_call_callback(optional, clef_change_initial);
}
SCM scheme_InsertClef (SCM optional) {
  return scheme_call_callback(optional, clef_change_insert);
}
SCM scheme_InitialKey (SCM optional) {
  return scheme_call_callback(optional, key_change_initial);
}
SCM scheme_InsertKey (SCM optional) {
  return scheme_call_callback(optional, key_change_insert);
}
SCM scheme_InitialTimeSig (SCM optional) {
  return scheme_call_callback(optional, timesig_change_initial);
}
SCM scheme_InsertTimeSig (SCM optional) {
  return scheme_call_callback(optional, timesig_change_insert);
}
SCM scheme_ChangeNotehead (SCM optional) {
  return scheme_call_callback(optional, set_notehead);
}
SCM scheme_InsertStem (SCM optional) {
  return scheme_call_callback(optional, stem_directive_insert);
}
SCM scheme_AddVerse (SCM optional) {
  return scheme_call_callback(optional, add_verse);
}
SCM scheme_DeleteVerse (SCM optional) {
  return scheme_call_callback(optional, delete_verse);
}
SCM scheme_EditFiguredBass (SCM optional) {
  return scheme_call_callback(optional, figure_insert);
}
SCM scheme_DeleteFiguredBass (SCM optional) {
  return scheme_call_callback(optional, delete_figured_bass);
}
SCM scheme_DeleteChordSymbols (SCM optional) {
  return scheme_call_callback(optional, delete_fakechords);
}
SCM scheme_HideFiguredBass (SCM optional) {
  return scheme_call_callback(optional, hide_figured_bass);
}
SCM scheme_ShowFiguredBass (SCM optional) {
  return scheme_call_callback(optional, show_figured_bass);
}
SCM scheme_EditChords (SCM optional) {
  return scheme_call_callback(optional, fakechord_insert);
}
SCM scheme_EditObject (SCM optional) {
  return scheme_call_callback(optional, edit_object);
}
SCM scheme_EditDirective (SCM optional) {
  return scheme_call_callback(optional, edit_object_directive);
}
SCM scheme_EditStaffDirective (SCM optional) {
  return scheme_call_callback(optional, edit_staff_directive);
}
SCM scheme_EditVoiceDirective (SCM optional) {
  return scheme_call_callback(optional, edit_voice_directive);
}
SCM scheme_EditScoreDirective (SCM optional) {
  return scheme_call_callback(optional, edit_score_directive);
}
SCM scheme_EditMovementDirective (SCM optional) {
  return scheme_call_callback(optional, edit_movement_directive);
}
SCM scheme_EditClefDirective (SCM optional) {
  return scheme_call_callback(optional, edit_clef_directive);
}
SCM scheme_EditTimesigDirective (SCM optional) {
  return scheme_call_callback(optional, edit_timesig_directive);
}
SCM scheme_EditKeysigDirective (SCM optional) {
  return scheme_call_callback(optional, edit_keysig_directive);
}
SCM scheme_DeleteDirective (SCM optional) {
  return scheme_call_callback(optional, delete_chord_or_note_directive);
}
SCM scheme_AttachLilyToNote (SCM optional) {
  return scheme_call_callback(optional, note_directive);
}
SCM scheme_AttachLilyToChord (SCM optional) {
  return scheme_call_callback(optional, chord_directive);
}
SCM scheme_GoToMeasure (SCM optional) {
  return scheme_call_callback(optional, tomeasurenum);
}
SCM scheme_GoToBeginning (SCM optional) {
  return scheme_call_callback(optional, tohome);
}
SCM scheme_GoToEnd (SCM optional) {
  return scheme_call_callback(optional, toend);
}
SCM scheme_MoveToBeginning (SCM optional) {
  return scheme_call_callback(optional, movetostart);
}
SCM scheme_MoveToEnd (SCM optional) {
  return scheme_call_callback(optional, movetoend);
}
SCM scheme_NextMovement (SCM optional) {
  return scheme_call_callback(optional, next_movement);
}
SCM scheme_PreviousMovement (SCM optional) {
  return scheme_call_callback(optional, prev_movement);
}
SCM scheme_DeleteMovement (SCM optional) {
  return scheme_call_callback(optional, delete_movement);
}
SCM scheme_Play (SCM optional) {
  return scheme_call_callback(optional, ext_midi_playback);
}
SCM scheme_Stop (SCM optional) {
  return scheme_call_callback(optional, stop_midi_playback);
}
SCM scheme_PlaybackProperties (SCM optional) {
  return scheme_call_callback(optional, playback_properties_change);
}
SCM scheme_Help (SCM optional) {
  return scheme_call_callback(optional, browse_manual);
}
SCM scheme_About (SCM optional) {
  return scheme_call_callback(optional, about);
}
SCM scheme_Shortcuts (SCM optional) {
  return scheme_call_callback(optional, display_shortcuts);
}
SCM scheme_MoreCommands (SCM optional) {
  return scheme_call_callback(optional, morecommands);
}
SCM scheme_MyCommands (SCM optional) {
  return scheme_call_callback(optional, mycommands);
}
SCM scheme_FetchCommands (SCM optional) {
  return scheme_call_callback(optional, fetchcommands);
}
SCM scheme_ToggleEdit (SCM optional) {
  return scheme_call_callback(optional, toggle_edit_mode);
}
SCM scheme_ToggleRest (SCM optional) {
  return scheme_call_callback(optional, toggle_rest_mode);
}
SCM scheme_ToggleRhythm (SCM optional) {
  return scheme_call_callback(optional, toggle_rhythm_mode);
}
SCM scheme_ClearOverlay (SCM optional) {
  return scheme_call_callback(optional, clear_overlay);
}
SCM scheme_CreateRhythm (SCM optional) {
  return scheme_call_callback(optional, create_rhythm_cb);
}
SCM scheme_DeleteRhythm (SCM optional) {
  return scheme_call_callback(optional, delete_rhythm_cb);
}
SCM scheme_InsertA (SCM optional) {
InsertA (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_AddNoteA (SCM optional) {
AddNoteA (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_AddA (SCM optional) {
AddA (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_ChangeToA (SCM optional) {
ChangeToA (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_MoveToA (SCM optional) {
MoveToA (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertB (SCM optional) {
InsertB (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_AddNoteB (SCM optional) {
AddNoteB (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_AddB (SCM optional) {
AddB (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_ChangeToB (SCM optional) {
ChangeToB (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_MoveToB (SCM optional) {
MoveToB (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertC (SCM optional) {
InsertC (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_AddNoteC (SCM optional) {
AddNoteC (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_AddC (SCM optional) {
AddC (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_ChangeToC (SCM optional) {
ChangeToC (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_MoveToC (SCM optional) {
MoveToC (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertD (SCM optional) {
InsertD (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_AddNoteD (SCM optional) {
AddNoteD (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_AddD (SCM optional) {
AddD (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_ChangeToD (SCM optional) {
ChangeToD (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_MoveToD (SCM optional) {
MoveToD (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertE (SCM optional) {
InsertE (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_AddNoteE (SCM optional) {
AddNoteE (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_AddE (SCM optional) {
AddE (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_ChangeToE (SCM optional) {
ChangeToE (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_MoveToE (SCM optional) {
MoveToE (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertF (SCM optional) {
InsertF (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_AddNoteF (SCM optional) {
AddNoteF (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_AddF (SCM optional) {
AddF (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_ChangeToF (SCM optional) {
ChangeToF (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_MoveToF (SCM optional) {
MoveToF (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertG (SCM optional) {
InsertG (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_AddNoteG (SCM optional) {
AddNoteG (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_AddG (SCM optional) {
AddG (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_ChangeToG (SCM optional) {
ChangeToG (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_MoveToG (SCM optional) {
MoveToG (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_0 (SCM optional) {
Dur0 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertDur0 (SCM optional) {
InsertDur0 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_ChangeDur0 (SCM optional) {
ChangeDur0 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_SetDur0 (SCM optional) {
SetDur0 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertRest0 (SCM optional) {
InsertRest0 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_1 (SCM optional) {
Dur1 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertDur1 (SCM optional) {
InsertDur1 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_ChangeDur1 (SCM optional) {
ChangeDur1 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_SetDur1 (SCM optional) {
SetDur1 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertRest1 (SCM optional) {
InsertRest1 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_2 (SCM optional) {
Dur2 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertDur2 (SCM optional) {
InsertDur2 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_ChangeDur2 (SCM optional) {
ChangeDur2 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_SetDur2 (SCM optional) {
SetDur2 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertRest2 (SCM optional) {
InsertRest2 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_3 (SCM optional) {
Dur3 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertDur3 (SCM optional) {
InsertDur3 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_ChangeDur3 (SCM optional) {
ChangeDur3 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_SetDur3 (SCM optional) {
SetDur3 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertRest3 (SCM optional) {
InsertRest3 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_4 (SCM optional) {
Dur4 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertDur4 (SCM optional) {
InsertDur4 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_ChangeDur4 (SCM optional) {
ChangeDur4 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_SetDur4 (SCM optional) {
SetDur4 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertRest4 (SCM optional) {
InsertRest4 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_5 (SCM optional) {
Dur5 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertDur5 (SCM optional) {
InsertDur5 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_ChangeDur5 (SCM optional) {
ChangeDur5 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_SetDur5 (SCM optional) {
SetDur5 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertRest5 (SCM optional) {
InsertRest5 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_6 (SCM optional) {
Dur6 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertDur6 (SCM optional) {
InsertDur6 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_ChangeDur6 (SCM optional) {
ChangeDur6 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_SetDur6 (SCM optional) {
SetDur6 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertRest6 (SCM optional) {
InsertRest6 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_7 (SCM optional) {
Dur7 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertDur7 (SCM optional) {
InsertDur7 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_ChangeDur7 (SCM optional) {
ChangeDur7 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_SetDur7 (SCM optional) {
SetDur7 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertRest7 (SCM optional) {
InsertRest7 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_8 (SCM optional) {
Dur8 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertDur8 (SCM optional) {
InsertDur8 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_ChangeDur8 (SCM optional) {
ChangeDur8 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_SetDur8 (SCM optional) {
SetDur8 (NULL, NULL);
return SCM_BOOL(TRUE);
}
SCM scheme_InsertRest8 (SCM optional) {
InsertRest8 (NULL, NULL);
return SCM_BOOL(TRUE);
}
