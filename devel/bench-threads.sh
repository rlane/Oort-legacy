#!/bin/sh
set -e
for i in `seq 0 16`
do
	export oort_NUM_THREADS=$i
	echo Threads: $i
	time ./dedicated/oort_dedicated -s $$ scenarios/basic.json examples/reference.lua examples/reference.lua
done
