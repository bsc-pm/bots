#!/bin/bash

#parse arguments
done=
while [ -z $done ]; do
case $1 in
	"-cb")
		shift;;
	*) done=true;;
esac
done


#run benchmarks


for b in $benchmarks; do
	cd benchmark
done

