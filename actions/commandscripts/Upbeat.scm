;;;Upbeat
(let script ((upbeat "Upbeat"))

  ; How many ticks are in a 100% filled measure?
  (define MaxTicks (* 1536 (GetPrevailingTimeSig #t) )) 
  (define EndTick #f)
  ;Upbeat is only for underful measures
  (define (warning)
   (Help::TimedNotice (_ "Upbeat/Short Measure can only be used in an underful, non-empty measures"))   
   #f)
 
 ; Define Upbeat-Directive Subprogram
 (define (createUpbeat)
   (define remainingTicks (- MaxTicks EndTick))
   (define partialDuration (number->string (/ EndTick 6 )))
   (GoToMeasureBeginning)
   (StandAloneDirectiveProto (cons upbeat (string-append "\\partial 256*" partialDuration 	" "))  #f #f upbeat)
   (d-SetDurationInTicks remainingTicks)
   (d-DirectivePut-standalone-override upbeat DENEMO_OVERRIDE_DURATION)
   (d-RefreshDisplay)
   (GoToMeasureEnd)
   (if  (not (d-MoveToMeasureRight))
	(d-AddMeasure)))
	
(define (ComputeAndCreate)
      	(begin	
      		; Save how many ticks are in this measure
		(GoToMeasureEnd)
		(set! EndTick (d-GetEndTick))
		; Cond what to do, only create Upbeat if the measure is not full, else give warning. 	
		(cond 
 			((not EndTick) (warning)) ; empty
 			((< EndTick MaxTicks) (createUpbeat)) ; underful
 			((= MaxTicks EndTick) (warning))  ; 100% filled
 			((< MaxTicks EndTick) (warning)) ; >100% filled
			(else  (warning)) ; ?
		)))
	
 

  (GoToMeasureBeginning)
  
  (if Upbeat::params ;;; non-interactive call, this used to take the params as the tag to be used - is that useful?
   	(d-DirectiveDelete-standalone upbeat))
   	
  (if (d-DirectiveGet-standalone-display upbeat); if upbeat is present
   		(let ( (choice (d-GetOption  (string-append (_ "Help") stop (_ "Re-calculate") stop (_ "Delete") stop (_ "Advanced") stop))))
   		 (cond
    			 ((boolean? choice)
    			 	 (d-WarningDialog (_ "Operation cancelled")))
    			 ((equal? choice (_ "Help"))      				
      				(d-InfoDialog (_ "This object fills up the duration of this measure, so that the notes in the measure form an upbeat. It needs to be renewed if you change the duration of the notes in the measure - use Re-calculate for this, or simply delete it and re-run the Upbeat command,")))
      			((equal? choice (_ "Re-calculate"))
      				(d-DirectiveDelete-standalone upbeat)
      				(ComputeAndCreate))		
			 ((equal? choice (_ "Delete"))
	 			(d-DeleteObject))
	 		((equal? choice (_ "Advanced"))
	 			(if (not (d-DirectiveTextEdit-standalone upbeat))
	 				(d-DirectiveDelete-standalone upbeat))
	 			)))
	 	;;;if upbeat not already present
		(ComputeAndCreate)))
	 		
		

