#!/bin/bash

if [ -h "$HOME/tauon" ];
then
	unlink $HOME/tauon
	echo "Remove symlink $HOME/tauon"
fi

ln -s $(pwd) $HOME/tauon
