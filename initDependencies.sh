#!/bin/sh

set -e

git submodule init
git submodule update

( cd Dependencies/VolViz/Dependencies && git submodule update concurrentqueue )
( cd Dependencies/VolViz/Dependencies && git submodule update glfw )
( cd Dependencies/VolViz/Dependencies && git submodule update libigl )
( cd Dependencies/VolViz/Dependencies && git submodule update PhysUnits )
