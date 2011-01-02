#!/bin/sh
set -e
for i in `seq 0 16`
do
	RISC_NUM_THREADS=$i ./dedicated/risc_dedicated -s $$ scenarios/basic.json examples/switch.lua examples/switch.lua | grep victorious
done
