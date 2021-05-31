#!/bin/bash

g++ -o assembler -Iinc/ src/*
./assembler -o interrupts.o tests/interrupts.s
./assembler -o main.o tests/main.s