#include <vector>
#include <iostream>
#include "relocationrecord.h"
#include "constants_assembler.h"

void RelocationRecord::printRelocationRecords(std::vector<RelocationRecord*>& records){
    std::cout << std::endl << "SymbolId    Ofsset    Section    RelocationType" << std::endl;

    for (RelocationRecord* rec : records) {
        std::cout << rec->symId << "    " << rec->ofsset << "    " << (int)rec->section << "    ";
        if (rec->relocationType == relocation_absolute) std::cout << "Rel_Absolute" << std::endl;
            else std::cout << "Rel_PC" << std::endl;
    }

    std::cout << std::endl;
}