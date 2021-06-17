#include "context.h"


Context::Context(unsigned short pc ,unsigned short sp) {
        this->pc = pc;
        this->sp = sp;
        this->r0 = this->r1 = this->r2 = this->r3 = this->r4 = this->r5 = this->psw = 0;
}

std::ostream &operator<<(std::ostream &os, const Context& context){ 
    os << std::endl;
    os << "R0: " << context.r0 << std::endl;
    os << "R1: " << context.r1 << std::endl;
    os << "R2: " << context.r2 << std::endl;
    os << "R3: " << context.r3 << std::endl;
    os << "R4: " << context.r4 << std::endl;
    os << "R5: " << context.r5 << std::endl;
    os << "SP: " << context.sp << std::endl;
    os << "PC: " << context.pc << std::endl;
    os << "PSW: " << context.psw << std::endl;
    os << std::endl;
    
    return os;
}