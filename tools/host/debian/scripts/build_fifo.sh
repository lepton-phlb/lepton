#!/bin/bash -

echo "Make fifos four synthetic target"

mkfifo -m 0666 /tmp/app2synth
mkfifo -m 0666 /tmp/synth2app

echo "Create tmp file contain tauon virtual_cpu path"
#PATH for virtual_cpu
echo "$HOME/tauon/tools/bin/virtual_cpu" > /tmp/tauontools
