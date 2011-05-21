#!/bin/sh
set -e
for i in `seq 0 16`
do
	OORT_NUM_THREADS=$i ./dedicated/oort_dedicated -s $$ scenarios/basic.json examples/reference.lua examples/reference.lua | grep victorious
done
