#!/bin/bash

if [ "$1" == "qextserialport" ]; then
    cd deps/qextserialport
    qmake
    make
    cd ../..
fi

exit 0
