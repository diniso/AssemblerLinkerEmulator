#!/bin/bash

g++ -o linker -Iinc/ src/*
./linker -hex -place=iv_table@0x0000 -place=text@0x4000 -o mem_content.hex main.o interrupts.o