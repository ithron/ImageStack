#!/bin/sh

set -e

echo Configuring ImageStack

PROJECT_ROOT=`pwd`
mkdir -p $PROJECT_ROOT/Dependencies/build

# Init VolViz
if [ -d "$PROJECT_ROOT/Dependencies/VolViz/include" ]; then
  cd $PROJECT_ROOT/Dependencies/VolViz

  ln -fs $PROJECT_ROOT/Dependencies/GSL/gsl $PROJECT_ROOT/Dependencies/VolViz/Dependencies/GSL/
  ln -fs $PROJECT_ROOT/Dependencies/eigen/* $PROJECT_ROOT/Dependencies/VolViz/Dependencies/eigen/
  ./configDependencies.sh
else
  echo "Skip VolViz"
fi


# Build googletest
if [ ! -d "$PROJECT_ROOT/Dependencies/build/googletest" ]; then
  cd $PROJECT_ROOT/Dependencies/build
  mkdir -p googletest
  cd googletest
  cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_GTEST=On -DBUILD_GMOCK=Off -GXcode ../../googletest
else
  echo Skip googletest
fi

cd $PROJECT_ROOT

