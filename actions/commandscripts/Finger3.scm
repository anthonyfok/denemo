;;; Warning!!! This file is derived from those in actions/menus/... do not edit here
(ToggleDirective "note" "postfix" "Fingering" "-3 ")
(if (d-Directive-note? "Fingering")
	(d-DirectivePut-note-display"Fingering" "3"))
(d-Chordize #t)
