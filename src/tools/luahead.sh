#!/bin/bash
#print $1
if [ -n "$1" ]; then
    cd $1
    python ../tools/qtlua.py GameLua.cpp -o GameLua.h
    python ../tools/qtlua.py qtgui/qtlua.cpp -o qtgui/qtlua.h
else
    echo -e "usage:\n$0 [path]\n"
fi

