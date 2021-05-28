#!/bin/bash

g++ -o assembler -Iinc/ src/*
./assembler -o main.o tests/interrupts.s