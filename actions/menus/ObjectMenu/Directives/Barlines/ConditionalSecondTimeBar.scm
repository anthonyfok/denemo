;ConditionalSecondTimeBar
;Insert 2nd time bar for other print-outs only
(d-SetMark)
(d-GoToEnd)
(d-Copy)
(d-RepeatEndStart)
(d-OnlyForLayout (cons 'noninteractive (cons (d-StaffProperties "query=lily_name") (d-GetCurrentStaffLayoutId))))
(d-AddMeasure)
(d-IgnoreOn 'noninteractive)
(d-OnlyForLayout (cons 'noninteractive (cons (d-StaffProperties "query=lily_name") (d-GetCurrentStaffLayoutId))))
(d-MoveCursorRight)
(d-Paste)
(d-IgnoreOff 'noninteractive)
(d-OnlyForLayout (cons 'noninteractive (cons (d-StaffProperties "query=lily_name") (d-GetCurrentStaffLayoutId))))