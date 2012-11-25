;;; Warning!!! This file is derived from those in actions/menus/... do not edit here
;;TextAbove
(let ((text #f) (tag "TextAbove")(markup #f)(current #f))
	(set! current (d-DirectiveGet-chord-display tag))
	(if (not current)
		(set! current "Tutti"))
	(set! text (d-GetUserInputWithSnippets (_ "Text") (_ "Give text to appear above music: ") current))
	(if text 
	  (begin
	  	(set! markup (cdr text))
	  	(set! text (car text))
		(d-DirectivePut-chord-display tag  text )
		(d-DirectivePut-chord-postfix tag  (string-append "^\\markup { \\override  #'(line-width . 40) " markup "}"))
		(d-DirectivePut-chord-minpixels  tag 20)
		(d-SetSaved #f))
	  (begin
	  	(let ((confirm (d-GetUserInput (d-DirectiveGet-chord-display tag) (_ "Delete this text?") (_ "y"))))
	  	 (if (and confirm (equal? confirm (_ "y")))
	  		(begin
	  			(d-DirectiveDelete-chord tag)
	  			(d-SetSaved #f))
			(d-InfoDialog (_ "Cancelled")))))))
		