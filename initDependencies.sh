#!/bin/sh

set -e

git submodule init
git submodule update

( cd Dependencies/VolViz/Dependencies && git submodule update --init concurrentqueue )
( cd Dependencies/VolViz/Dependencies && git submodule update --init glfw )
( cd Dependencies/VolViz/Dependencies && git submodule update --init PhysUnits )
