#!/bin/bash

rm -rf install
ecosconfig check

echo "create build tree"
ecosconfig tree

echo "generate build tree"
make

echo "End ..."
cp  -v target.ld install/lib

echo "Delete tmp files .."
rm -rf hal kernel makefile services error infra isoinfra language
