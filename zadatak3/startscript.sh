#!/bin/bash                                                                     

g++ -o emulator -Iinc/ src/*
./emulator tests/program.hex
# xxd program.hex
