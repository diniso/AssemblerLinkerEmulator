#include <vector>
#include <iostream>
#include "relocationrecord.h"
#include "constants.h"


std::vector<RelocationRecord*> RelocationRecord::records = {};

std::vector<RelocationRecord*> RelocationRecord::getAllRecords() {
    return RelocationRecord::records;
}

void RelocationRecord::deleteRelocationRecords(){
    for (RelocationRecord* rec : RelocationRecord::records) {
        delete rec;
    }
    RelocationRecord::records.clear();
}

RelocationRecord::RelocationRecord(int symId , int ofsset , char section , char recolationType){
    this->symId = symId;
    this->ofsset = ofsset;
    this->section = section;
    this->relocationType = recolationType;
}

RelocationRecord* RelocationRecord::createRelocationRecord(int symId , int ofsset , char section , char relocationType){
    RelocationRecord* rec = new RelocationRecord(symId , ofsset , section , relocationType);
    RelocationRecord::records.push_back(rec);
    return rec;
}

void RelocationRecord::printRelocationRecords(){
    std::cout << std::endl << "SymbolId    Ofsset    Section    RelocationType" << std::endl;

    for (RelocationRecord* rec : RelocationRecord::records) {
        std::cout << rec->symId << "    " << rec->ofsset << "    " << (int)rec->section << "    ";
        if (rec->relocationType == relocation_absolute) std::cout << "Rel_Absolute" << std::endl;
            else std::cout << "Rel_PC" << std::endl;
    }

    std::cout << std::endl;
}