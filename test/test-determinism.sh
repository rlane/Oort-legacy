#!/bin/sh
export RISC_SEED=$$
set -e
for i in `seq 0 16`
do
	RISC_NUM_THREADS=$i ./dedicated/risc_dedicated scenarios/basic.lua examples/switch.lua examples/switch.lua | grep victorious
done
