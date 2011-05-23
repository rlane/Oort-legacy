#!/bin/sh
set -e
DB=local-tournament.yaml
ITERS=100
export RUBYLIB=tournament/lib

rm -f $DB
for X in $@
do
	./tournament/bin/oort-tournament-register -D $DB $X $X
done

./tournament/bin/oort-tournament -D $DB --iters $ITERS

./tournament/bin/oort-tournament-report -D $DB
