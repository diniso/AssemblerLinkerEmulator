#!/bin/bash

g++ -o linker -Iinc/ src/*
./linker -hex -place=ivt@0x0000 -o program.hex tests/main.o tests/interrupts.o
mv program.hex ../zadatak3/tests/program.hex
# xxd program.hex