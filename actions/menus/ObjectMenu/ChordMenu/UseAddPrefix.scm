(let ((tag "UseAddPrefix")(params UseAddPrefix::params))
    (if (equal? params "edit")
        (set! params #f))
    (d-Directive-standalone tag)
     (if params
        (d-DirectivePut-standalone-graphic tag "\nAx\nDenemo\n24")
        (d-DirectivePut-standalone-graphic tag "\nA\nDenemo\n24"))
    (if (not params)
        (set! params "add"))
    (d-DirectivePut-standalone-postfix tag (string-append " \\set additionalPitchPrefix = #\"" params "\" "))
    (d-DirectivePut-standalone-display tag "add prefix")
   
    (d-DirectivePut-standalone-minpixels tag 30)
    (d-SetSaved #f)
    (d-RefreshDisplay))
