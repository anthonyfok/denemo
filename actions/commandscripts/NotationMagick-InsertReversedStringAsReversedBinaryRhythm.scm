;;; Warning!!! This file is derived from those in actions/menus/... do not edit here
(let ((return (NotationMagick::AskNewAsciistring)))
	(if return
		 (NotationMagick::PutBinaryStringList  (reverse (NotationMagick::ReverseStringsInList (NotationMagick::String->CharsAsBinary return))))
		 #f))