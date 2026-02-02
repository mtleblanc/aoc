#!/bin/bash
file="$1"

if [[ "$file" == *"/src/"* ]]; then
    # src/2015/day25.cc -> inputs/2015/input25
    newfile=$(echo "$file" | sed 's|/src/|/inputs/|; s|/day\([0-9]*\)\.cc$|/input\1|')
else
    # inputs/2015/input25 -> src/2015/day25.cc
    newfile=$(echo "$file" | sed 's|/inputs/|/src/|; s|/input\([0-9]*\)$|/day\1.cc|')
fi

code -r "$newfile"
