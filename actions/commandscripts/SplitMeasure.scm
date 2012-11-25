;;; Warning!!! This file is derived from those in actions/menus/... do not edit here
;SplitMeasure:splits a measure 
;leave cursor at the beginning of the new measure (after the barline)
(let split ()
	(define count -1)
	(d-PushClipboard)
	(d-UnsetMark)
	(d-SetMark)
	(if   (d-MeasureRight)
	  (begin
	  	(d-CursorLeft)  
		(d-Cut)
		(d-CursorRight)
		(d-InsertMeasureBefore))
	  (begin
		(d-GoToEnd)
		(d-Cut)
		(d-AppendMeasure)
		(d-MoveToMeasureRight)))
	(d-UnsetMark)
	(RepeatUntilFail (lambda () (set! count (1+ count)) (d-PutClipObj 0 count)))
	(GoToMeasureBeginning)	
	(d-PopClipboard))