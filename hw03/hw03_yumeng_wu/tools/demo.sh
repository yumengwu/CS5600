#!/bin/bash

if [[ ! -e $1 ]]; then
    echo "Usage: ./demo.sh executable"
    exit 1
fi

tools/gen-random.pl 5 /tmp/input-$$.dat
echo "== input: " /tmp/input-$$.dat
tools/print-file.pl /tmp/input-$$.dat
gdb --args ./$1 /tmp/input-$$.dat /tmp/output-$$.dat
#./$1 /tmp/input-$$.dat /tmp/output-$$.dat
echo "== output: " /tmp/input-$$.dat
tools/print-file.pl /tmp/output-$$.dat
echo "== correct: " /tmp/correct-$$.dat
tools/sort-file.pl /tmp/input-$$.dat /tmp/correct-$$.dat
tools/print-file.pl /tmp/correct-$$.dat
echo "== compare says:"
tools/cmp.pl /tmp/output-$$.dat /tmp/correct-$$.dat
rm -f /tmp/input-$$.dat /tmp/output-$$.dat /tmp/correct-$$.dat
