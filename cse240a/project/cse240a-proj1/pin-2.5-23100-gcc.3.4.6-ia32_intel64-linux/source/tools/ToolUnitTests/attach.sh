#! /bin/sh
rm -f pin_attached fini.out
touch fini.out
./$1attach &
$2 -pid $! -mt 0 -t $1fini.so
touch pin_attached
wait

