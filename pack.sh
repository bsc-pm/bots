#!/bin/sh

VERSION=1.0

make clean
mkdir bots
cp -ar common/ config inputs/ omp-tasks/ serial run/ bots
cp -a configure Makefile bots
rm -fr bots/omp-tasks/concom/ bots/omp-tasks/knapsack/ bots/inputs/knapsack/
tar --exclude=*svn* --exclude=*knapsack* -zcf bots-$VERSION.tar.gz bots
rm -fr bots
