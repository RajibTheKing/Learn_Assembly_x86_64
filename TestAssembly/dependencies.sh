#!/bin/bash

# begin
TARGET=$1
FILES=${@:2}
echo $FILES
CC=gcc
CCFLAG="-std=c++20 -g -mrdrnd -Wall -O3"
# #RELEASE_MODE
# CCFLAG="-std=c++20                                     \
#         -fdiagnostics-color=always                     \
#         -g                                             \
#         -mrdrnd                                        \
#         -Wall                                          \
#         -Werror                                        \
#         -Werror=switch                                 \
#         -Wextra                                        \
#         -Wfatal-errors                                 \
#         -Wpedantic                                     \
#         -Wno-deprecated-declarations                   \
#         -Wno-missing-field-initializers                \
#         -O3
#         "

#DEBUG_MODE
# CCFLAG="-std=c++20 -fconstexpr-depth=2500                         \
#         -fdiagnostics-color=always                     \
#         -fvisibility=internal                          \
#         -g                                             \
#         -mrdrnd                                        \
#         -Wall                                          \
#         -Werror                                        \
#         -Werror=switch                                 \
#         -Wextra                                        \
#         -Wfatal-errors                                 \
#         -Wpedantic                                     \
#         -Wno-deprecated-declarations                   \
#         -Wno-missing-field-initializers                \
#         -ftrapv                                        \
#         -O0"
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
