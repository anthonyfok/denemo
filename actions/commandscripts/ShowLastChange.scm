;;; Warning!!! This file is derived from those in actions/menus/... do not edit here
;;;;ShowLastChange
(let ((last (d-GetLastChange)))
  (if last
    (d-InfoDialog (string-append (_ "The last change was:\n") last))
    (d-InfoDialog (_ "No change recorded in Undo stack"))))
