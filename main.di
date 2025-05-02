var x 5
label start
out x
out "\n"
if a x > 0
    var x x - 1
    jump start
else a
    out "Done"
endif a