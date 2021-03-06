;Unmeasured
(let ((tag "Unmeasured"))
	(define (do-it)
		  (d-InitialTimeSig "256/1")
		   (d-DirectivePut-timesig-postfix tag "\\cadenzaOn ")
		   (d-DirectivePut-timesig-override tag (logior DENEMO_OVERRIDE_GRAPHIC DENEMO_OVERRIDE_LILYPOND))
		   (d-DirectivePut-timesig-graphic tag "\n \n"))
	(define (undo-it)
		(d-DirectiveDelete-timesig tag))
	(while (d-StaffUp))
	(if (d-Directive-timesig? tag)
		(begin
			(d-DeactivateTimeSignatureStencil 'delete)
			(undo-it)
			(while (d-StaffDown) (undo-it))
			(d-InitialTimeSig "4/4")
			(d-InitialTimeSig))
		(begin
		 	(d-DeactivateTimeSignatureStencil 'install)
			(do-it)
			(while (d-StaffDown) (do-it))))
            (d-SetSaved #f))