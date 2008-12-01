#!/bin/bash


BASE_DIR=$(dirname $0)/..
RUN_DIR=$BASE_DIR/run

for script in $RUN_DIR/run-*.sh; do
	script=$(basename $script)

	app=${script%*.sh}
	app=${app#run-}

	[ "$app" = "all" ] && continue;

	
	$RUN_DIR/$script $*
done


