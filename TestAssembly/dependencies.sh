#!/bin/bash

# begin
TARGET=$1
FILES=${@:2}
echo $FILES
CC=gcc
CCFLAG="-std=c++20 -g -mrdrnd -Wall -o3"
INC=-I/


for file in $FILES ; do
    curobj=${file/%.cpp/.o}
    if [ "${curobj: -2}" == ".s" ]; then curobj=${file/%.s/.o}; fi
    
    cmd="$CC $CCFLAG $INC -c $file -o $curobj"
    echo $cmd
    $cmd
done
OBJFILES=$(echo ./*.o)
cmd="$CC $OBJFILES -lstdc++ -o $TARGET"
echo $cmd
$cmd
#end