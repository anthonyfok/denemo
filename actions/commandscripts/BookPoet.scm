;;; Warning!!! This file is derived from those in actions/menus/... do not edit here
;;BookPoet
(if BookPoet::params
	(BookTitles::Do "Poet" "poet"  BookPoet::params #f)
	(BookTitles::Do "Poet" "poet" "My Poet"  (_ "Give name for poet/lyricist/librettist etc or blank out to delete: ")))
