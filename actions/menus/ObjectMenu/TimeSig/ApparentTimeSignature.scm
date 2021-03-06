;;ApparentTimeSignature
(let ((real (d-GetPrevailingTimesig)))
	(d-InsertTimeSig)
	(d-MoveCursorLeft)	
	(if (equal? (d-GetType) "TIMESIG")
		(let ((timesig (d-GetPrevailingTimesig)))
			(d-InsertTimeSig real)
			(d-DirectivePut-timesig-postfix "ApparentTimeSignature" (string-append "\\set Staff.timeSignatureFraction = " timesig " "))
			(d-DirectivePut-timesig-override "ApparentTimeSignature" DENEMO_OVERRIDE_LILYPOND)
			(d-DirectivePut-timesig-display "ApparentTimeSignature" timesig)
			(d-SetSaved #f))))