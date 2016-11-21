#!/bin/sh

set -e

PROJECT_ROOT=`pwd`
mkdir -p $PROJECT_ROOT/Dependencies/build

cd Dependencies/build

# Init VolViz
cd ../VolViz
./configDependencies.sh

cd $PROJECT_ROOT/Dependencies/build

# Build googletest
mkdir -p googletest
cd googletest
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_GTEST=On -DBUILD_GMOCK=Off -GXcode ../../googletest

cd $PROJECT_ROOT

ln -s $PROJECT_ROOT/Dependencies/GSL/gsl $PROJECT_ROOT/Dependencies/VolViz/Dependencies/GSL/
ln -s $PROJECT_ROOT/Dependencies/eigen/* $PROJECT_ROOT/Dependencies/VolViz/Dependencies/eigen/
