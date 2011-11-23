#!/bin/sh

VERSION=1.1.2

make clean
mkdir bots
cp -ar common/ config inputs/ omp-tasks/ serial run/ bots
cp -a configure Makefile bots
rm -fr bots/omp-tasks/concom/ bots/omp-tasks/knapsack/ bots/inputs/knapsack/ bots/config/make.config
rm -fr bots/run/run-knapsack.sh
find bots -type f -print0 | xargs -0 chmod a-w
tar --exclude=*svn* -zcf bots-$VERSION.tar.gz bots
rm -fr bots
