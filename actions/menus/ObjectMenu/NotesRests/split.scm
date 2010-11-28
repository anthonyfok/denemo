; Splits the current chord/note to several others of the same pitch. Their sum equals the original duration. Will create tuplets if necessary
; So far only (assuming quarter) 8th, 8th-triplet, 16th, 16-fifthlet, 16th-sextlet, 16th-Seventhlet, 32th and 32th-ninthlet
; TODO: What about if someone tries to divide too small? The score will get broken.

(define* (SplitChord #:optional (howmany #f))
(define  (split-divide)
	(duration::ChangeNoteDurationInTicks (/ (duration::GetBaseDurationInTicks) 2) (duration::GetNumberOfDotsInLilypond (d-GetNoteDuration))))

(if (and (d-GetNotes) (not (string-ci=?  (d-GetType) "Appending"))) ; DuplicateChord only works ON notes, not in the appending position. 

(begin

(if (not howmany)
	(begin
		 (set! howmany (d-GetUserInput "Split in how many notes?" "Enter the amount of fraction of equal duration in their sum:" "5" ) )
		 (if howmany (set! howmany (string->number howmany)) #f) ; If the user pressed "Abort" or Esc howmany will be #f which must be catched here
	)
)

(case howmany
	((1) #t) ; Already original duration in one note.
	((2) (begin  ;8th
		 (split-divide) 
		 (d-DuplicateChord)
		 (d-MoveCursorRight)
		 #t))
	((3) 	(begin ;8th-triplet
		(d-StartTriplet)
		(split-divide)
		(d-DuplicateChord)
		(d-DuplicateChord)
		(d-MoveCursorRight)
		(d-EndTuplet)
		#t))
	((4) (begin  ;16th
		 (split-divide)
		 (split-divide)
		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-MoveCursorRight)
		 #t))
 
	((5) (begin  ;16-fifthlet
		 (split-divide)
		 (split-divide)
	 	 (d-StartTriplet)
		 (d-MoveCursorLeft)
		 (d-SetTuplet "4/5")	
	 	 (d-MoveCursorRight)
		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-MoveCursorRight)
		 (d-EndTuplet)
		 #t))
	 
	((6) (begin  ;16th-sextlet,
		 (split-divide)
		 (split-divide) 
	 	 (d-StartTriplet)
		 (d-DuplicateChord)
		 (d-DuplicateChord)
 		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-DuplicateChord)
 		 (d-MoveCursorRight)
		 (d-EndTuplet)
		 #t))
		 
	((7) (begin  ;16-Seventhlet
		 (split-divide)
		 (split-divide)
	 	 (d-StartTriplet)
		 (d-MoveCursorLeft)
		 (d-SetTuplet "4/7")	
	 	 (d-MoveCursorRight)
		 (d-DuplicateChord)
 		 (d-DuplicateChord)
 		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-MoveCursorRight)
		 (d-EndTuplet)
		 #t))

	((8) (begin  ;32th
		 (split-divide)
		 (split-divide)
 		 (split-divide) 
		 (d-DuplicateChord)
 		 (d-DuplicateChord)
 		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-MoveCursorRight)
		 #t))
		 
	((9) (begin  ;32th-ninthlet
		 (split-divide)
		 (split-divide)
 		 (split-divide)
	 	 (d-StartTriplet)
		 (d-MoveCursorLeft)
		 (d-SetTuplet "8/9")	
	 	 (d-MoveCursorRight)
		 (d-DuplicateChord)
 		 (d-DuplicateChord)
 		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-DuplicateChord)
		 (d-MoveCursorRight)
		 (d-EndTuplet)
		 #t))
		 
	(else #f); out of range
)) 
 #f ; it was no note or the appending position
 )) 
