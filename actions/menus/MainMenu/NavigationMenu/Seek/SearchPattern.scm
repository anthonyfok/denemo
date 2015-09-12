;;;SearchPattern
(define-once SearchPattern::pattern '())
(define-once SearchPattern::MatchEnd #f)
(let ((params SearchPattern::params) (pattern '()))
   (if (d-GoToMark)
        (let ((choice 
            (if (not (null? SearchPattern::pattern))
                (RadioBoxMenu (cons (_ "New search pattern from selection") 'new)
                                    (cons (_ "Resume previous search") 'resume))
                'new)))
            (case choice
                ((new)
                    (let loop () 
                        (if (d-IsInSelection)
                            (begin
                                (if (Music?)
                                    (begin
                                        (set!  pattern (cons (cons (d-GetNote) (if (d-IsTied) (- 0 (d-GetDurationInTicks))(d-GetDurationInTicks))) pattern))
                                        (if (d-NextObject)
                                            (loop)))
                                    (if (d-NextObject)
                                            (loop))))))
                    (set! pattern (reverse pattern))
                    (set!  SearchPattern::pattern pattern))
                ((resume)
                    (set! pattern  SearchPattern::pattern))
                (else
                    (set! SearchPattern::pattern '()))))
        (set! pattern  SearchPattern::pattern))
    (set! SearchPattern::MatchEnd #f)
    (d-UnsetMark)
    (if (null? pattern)
        (d-WarningDialog (_ "Make a selection to search for"))
        (let outermost-loop ()
            (define continuing #t)
            (let ((outer-position #f))
                (let loop ((index 0))
                    (while (and (not (Music?)) (d-MoveCursorRight)))
                    (if (= index 0)
                        (set! outer-position (GetPosition)))
                    (let inner-loop ()
                        (define position (GetPosition))
                        (if (Music?) 
                            (let ((step (list-ref pattern index)))
                                (if (and (equal? (if (car step) #t #f) (if (d-GetNote) #t #f)) (= (cdr step) (if (d-IsTied) (- 0 (d-GetDurationInTicks))(d-GetDurationInTicks))))
                                    (if (< index (1- (length pattern)))
                                        (begin
                                            (if (d-NextObject)
                                                (loop (1+ index))))
                                        (begin 
                                            (set! SearchPattern::MatchEnd (GetPosition))
                                            (apply d-GoToPosition outer-position)
                                            (if (> (length pattern) 1)
                                                (d-MoveCursorLeft))
                                            (set! continuing #f)))
                                    (begin 
                                        (apply d-GoToPosition outer-position)
                                        (if  (d-NextObject)   
                                            (loop 0)))))
                            (if (and continuing (d-NextObject))
                                (inner-loop))))))
            (if (and continuing (d-NextObject))
                (outermost-loop))))
    (if SearchPattern::MatchEnd
        (let ((choice (RadioBoxMenu 
            (cons (_ "Continue") 'continue)
            (cons (_ "Execute Scheme") 'execute)
            (cons (_ "Stop") 'stop))))
            (if (> (length pattern) 1)
                (d-MoveCursorRight))     
            (case choice
                    ((continue)
                        (apply d-GoToPosition SearchPattern::MatchEnd)
                        (if (= 1 (length SearchPattern::pattern))
                            (d-NextObject))
                        (d-SearchPattern))
                     ((execute) (d-ExecuteScheme))))
        (let ((choice (RadioBoxMenu 
            (cons (_ "Wrap to start of staff") 'staff)
            (cons (_ "Wrap to next staff") 'below)
            (cons (_ "Stop") 'stop))))
            (case choice
                ((staff)
                    (d-MoveToBeginning)
                    (d-SearchPattern 'continue))
                ((below)
                    (if (d-WrapToNextStaff)
                        (begin
                            (d-MoveToBeginning)
                             (d-SearchPattern 'continue))))))))
