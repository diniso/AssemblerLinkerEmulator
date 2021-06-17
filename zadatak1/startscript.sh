#!/bin/bash

g++ -o assembler -Iinc/ src/*
./assembler -o interrupts.o tests/interrupts.s
./assembler -o main.o tests/main.s
mv interrupts.o ../zadatak2/tests/interrupts.o
mv main.o ../zadatak2/tests/main.o
#xxd main.o
#xxd interrupts.o