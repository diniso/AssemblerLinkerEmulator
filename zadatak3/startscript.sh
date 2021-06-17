#!/bin/bash                                                                     

g++ -o emulator -Iinc/ src/* -lpthread
# xxd tests/program.hex
./emulator tests/program.hex

