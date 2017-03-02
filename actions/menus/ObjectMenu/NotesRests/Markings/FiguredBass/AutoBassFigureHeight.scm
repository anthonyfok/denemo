(let ()
(define (AdjustFigures) (disp "Adjust Figures\n\n")
    (d-MoveToBeginning)
    (while (d-StaffDown))
    (while (d-NextNote)
        (let ((fig (d-GetBassFigure)) (height  (d-GetNoteStaffPosition)))
            (define (multiple fig)
                (string-index fig #\|))
            
           (define (adjust height thelist)
                (define count 0)
                (if  (< height 4)
                    (set! height 4)) 
                 (while (pair? thelist)
                    (set! count (1+ count)) ;(disp "the list " thelist "\n")
                    (d-AdjustBassFigureHeight (cons (string-append "FBHeight" (number->string count)) (number->string (+  (* 1.75 (car thelist)) -1.250 (/ (+ 5 height) 2)))))
                    (set! thelist (cdr thelist))))
                    
             (define (skip-over-spaces fig count len)
                (let loop ()
                    (define this (string-ref fig count))
                    (if (and (equal? this #\space) (< count (string-length fig)))
                        (begin
                            (set! count (1+ count))
                            (if (< count len)
                                (loop)))))
                (1- count))
                
            (define (skip-over-figures fig count len)
                (let loop ()
                    (define this (string-ref fig count))
                    (if (not (or (equal? this #\space)(equal? this #\|)))
                        (begin
                            (set! count (1+ count))
                            (if (< count len)
                                (loop)))))
                (1- count))
                
            (define (figure-heights fig) ;; returns a list of the number of figures stacked vertically for each group in the string fig
                (define thelist '())
                (define num 1) ;the number of figures stacked vertically in the current group
                (define len (1- (string-length fig)))
                (let loop ((count  (1+ (skip-over-spaces fig 0 len))))
                    (define this (string-ref fig count)) ;(disp "looping with <" fig "> char " this " count" count " num =" num "\n")
                    (if (equal? this #\|)
                        (begin
                            (set! thelist (cons (1- num) thelist))
                            (set! count (skip-over-spaces fig (1+ count) len))
                            (set! num 1))
                        (if (equal? this #\space)
                            (begin
                                (set! num (1+ num))
                                (set! count (skip-over-spaces fig count len)))
                            (begin
                                (set! count (skip-over-figures fig count len)))))
                    (if (< count len)
                        (loop (1+ count))
                        (set! thelist (cons num thelist))))
                (reverse thelist)) ;;;end create figure heights list
;;;procedure
           (if (and fig height (not (equal-string? fig "_")))
            (let ((thelist (figure-heights fig))(before 0)(after 0))
                        (if (d-MoveCursorLeft)
                            (begin
                                (set! before (d-GetNoteStaffPosition))
                                (if (not before)
                                    (set! before 0))
                                (d-MoveCursorRight)
                                (if (d-MoveCursorRight)
                                    (begin
                                        (set! after (d-GetNoteStaffPosition))
                                        (if (not after)
                                            (set! after 0))
                                        (d-MoveCursorLeft)))))
                        (if (and (> before height)(> after height)) ;this note is in a trough
                            (begin (disp "the list " thelist "\n")
                                (set! before (if (> before after) after before))
                                 (adjust before thelist))
                             (begin
                                (if (multiple fig)
                                    (adjust (if (d-IsTied) (1+ height) height) thelist))))))))) ;;;end of AdjustFigures for whole movement       
                                 
(d-PushPosition)
(while (d-PreviousMovement))
(AdjustFigures)
(while (d-NextMovement)
    (AdjustFigures))
(d-PopPosition))