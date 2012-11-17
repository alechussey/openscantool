#!/bin/bash

if [ "$1" == "libobd" ]; then
	cd deps/libobd
    qmake
    make
    cd ../..
fi

exit 0
