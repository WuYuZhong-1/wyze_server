#!/bin/bash

MY_LIB_PATH=../depend/lib
# echo  $MY_LIB_PATH
export LD_LIBRARY_PATH=$MY_LIB_PATH:$LD_LIBRARY_PATH
echo "======depend lib======>>>>>"
ldd test
echo "======running======>>>>>"
./test


