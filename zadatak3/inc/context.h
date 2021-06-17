#include <iostream>


struct Context {
    unsigned short r0;
    unsigned short r1;
    unsigned short r2;
    unsigned short r3;
    unsigned short r4;
    unsigned short r5;
    unsigned short sp;
    unsigned short pc;
    unsigned short psw;

    Context(unsigned short pc ,unsigned short sp);


    friend std::ostream &operator<<(std::ostream &os, const Context& context);
};