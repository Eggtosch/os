#!/bin/bash

olddir=$(pwd)
basedir=$(dirname $(readlink -f "$0"))

cd $basedir

ld -r -b binary -o font_unifont.o unifont.sfn
gcc -O3 -fPIE -o font font.c font_unifont.o
./font > ../../fonts/8x16.pgm
rm font_unifont.o
rm font
