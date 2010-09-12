;; Lilypond to Denemo
;; A Lexer / Parser to import ly files into Denemo or other formats
;; By Richard Shann and Nils Gey, July / August 2010
;; Usage of SILex and LALR-scm 
; This file is part of Denemo, http://www.denemo.org
;
;  Its based on Lilyponds parser.yy: 
;		Copyright (C) 1997--2010 Han-Wen Nienhuys <hanwen@xs4all.nl>
;								 Jan Nieuwenhuizen <janneke@gnu.org>
;								 
; Denemo is free software: you can redistribute it and/or modify
;  it under the terms of the GNU General Public License as published by
;  the Free Software Foundation, either version 3 of the License, or
;  (at your option) any later version.
;
;  Denemo is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License
;  along with Denemo.  If not, see <http://www.gnu.org/licenses/>.
;;

(define lyimport::filename #f)
(define lyimport::state #f) ;;;;; stack of lexer states
(define lyimport::AssignmentTable #f)
(define lyimport::create_lexer_each_time #f) 
(define lyimport::halt_on_error #f)
(define lexer-get-line #f)
(define lexer-get-column #f)
(define lexer-get-offset #f)
(define lexer-getc #f)
(define lexer-ungetc #f)
(define lexer-port #f)

(define lyimport::noteslexer #f)
(define lyimport::quotelexer #f)
(define lyimport::blocklexer #f)

;; Libs
(load "lalr.scm")
(load "silex.scm")
(load "multilex.scm")
(load "lyimport-lexer.scm") ; Helper functions for the lexer
(load "lyimport-parser.scm") ; Helper functions and parser rules
(load "lyimport-todenemo.scm") ; conversion of parse tree to Denemo script

(define (lyimport::multilexer)
;;; the lexing procedure itself
					;(format #t "lexer state ~a ~a~%" lyimport::state (car lyimport::state))
  (cond
   ((eqv? (car lyimport::state) 'notes)
    (lyimport::noteslexer))
   ((eqv? (car lyimport::state) 'quote)
    (lyimport::quotelexer))
   ((eqv? (car lyimport::state) 'block)
    (lyimport::blocklexer))
   (else
    (display "no lexer"))))

(if (defined? 'Denemo)
    (begin
      ;; Options
      (set! lyimport::create_lexer_each_time #t) ; Switch to decide if the lexer gets rebuild everytime or the existing file gets used. Default #t if interactive
      (set! lyimport::halt_on_error #t) ; Switch to decide if a caught error stops the program and gives a reminder or silently goes on, creating a wrong output. Default #t if interactive
      )
    (begin
      (set! lyimport::create_lexer_each_time #f) ;
      (set! lyimport::halt_on_error #t) ; on  error do not stop
      )
    )


(define (lyimport::sysdirectory name)
  (if (defined? 'Denemo)
      (string-append DENEMO_ACTIONS_DIR name)
      name))
(define (lyimport::localdirectory name)
  (if (defined? 'Denemo)
      (string-append DENEMO_LOCAL_ACTIONS_DIR name)
      name))

					; Assignment functions. Wants a string as type and something of your own choice (for example a music list)
					; Three more functions to get the values are defined and used in lyimport-lexer.scm FIXME move this function there too????
(define (lyimport::as-create key pair-type-and-value)
  (hashq-set! lyimport::AssignmentTable (string->symbol key) pair-type-and-value)
  )

;;;;;;;;;;;;;;;;;;;SILEX LEXER;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;If needed: Generate a loadable, standalone lexer file from the .l syntax file 
(if (and (file-exists? (lyimport::localdirectory "notes.l.scm")) (not lyimport::create_lexer_each_time))
	(display "Using existing lexer file\n")
	(begin ; The user wants a new generation or the file does not exist yet.
	  (lex-tables  (lyimport::sysdirectory "notes.l") "notes-table"   (lyimport::localdirectory "notes.l.scm")  'counters 'all)))
;;;FIXME further lexers here e.g. for quoted strings, lilypond blocks we don't need to examine etc.
(if (and (file-exists? (lyimport::localdirectory "quote.l.scm")) (not lyimport::create_lexer_each_time))
    (display "Using existing lexer file\n")
    (begin ; The user wants a new generation or the file does not exist yet.
      (lex-tables (lyimport::sysdirectory "quote.l") "quote-table"  (lyimport::localdirectory "quote.l.scm")  'counters 'all)))

(if (and (file-exists? (lyimport::localdirectory "block.l.scm")) (not lyimport::create_lexer_each_time))
    (display "Using existing lexer file\n")
    (begin ; The user wants a new generation or the file does not exist yet.
      (lex-tables (lyimport::sysdirectory "block.l") "block-table"  (lyimport::localdirectory "block.l.scm")  'counters 'all)))


(define (lyimport::import)
(format #t "Starting lyimport now ~%~%~%")
  (if (defined? 'Denemo)
      (set! lexer-port (open-input-file lyimport::filename))
      (set! lexer-port (open-input-file "mytest.ly")))

  (let ((IS (lexer-make-IS 'port lexer-port  'all)))
    (set! lexer-get-line (lexer-get-func-line IS))
    (set! lexer-get-column (lexer-get-func-column IS))
    (set! lexer-get-offset (lexer-get-func-offset IS))
    (set! lexer-getc (lexer-get-func-getc IS))
    (set! lexer-ungetc (lexer-get-func-ungetc IS))
    
    
    
    (load (lyimport::localdirectory "notes.l.scm"))
    (set! lyimport::noteslexer (lexer-make-lexer notes-table IS))
    
    
    (load (lyimport::localdirectory "quote.l.scm"))
    (set! lyimport::quotelexer (lexer-make-lexer quote-table IS))
    
    (load (lyimport::localdirectory "block.l.scm"))
    (set! lyimport::blocklexer (lexer-make-lexer block-table IS))
    )



;Blank the table of Assignments. Every assignment (block of notes and other events) is stored as one entry in the hash table.
(set! lyimport::AssignmentTable (make-hash-table))
					;(hashq-set! lyimport::AssignmentTable 'name_of_assignment_var_as_symbol value_of_assignment) ; Template

(set! lyimport::state (list 'notes)) 

;;;;;;;;;;;;;;;;;;;LALR-SCM PARSER;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

					; Run the parser. It wants the lexer and a function to display the uncatched errors. Automatically runs on the current input port.
(newline)
(display ":::::::: Parser Start ::::::::::")(newline)
(let ((final_list (mxml2ly2denemo-parser lyimport::multilexer displayerror)))

  (newline)
  (display ":::::::: Parser Finished ::::::::::")(newline)
  
  (newline)
  (display "============= Here is the final list =============")(newline)
  (display "============= ====================== =============")(newline)
  (pretty-print final_list)(newline)
  (display "============= ====================== =============")(newline)
  (newline)
  
  ;; Close input port
  (close (current-input-port))
  
  (lyimport::convert_to_denemo final_list)))

(if (not (defined? 'Denemo))
    (lyimport::import))
