#include <string>
#include <vector>
#include <iostream>

#include "symbolmappings.h"
#include "constants_assembler.h"

std::vector<SymbolMapping*> SymbolMapping::mappings = {};

SymbolMapping::SymbolMapping(std::string filename , int prevId , int id , int adress) {
    this->filename = filename;
    this->prevId = prevId;
    this->id = id;
    this->adress = adress;
}

SymbolMapping* SymbolMapping::findMappingByPrevId(int prevId, std::string filename) {
    for (SymbolMapping* sm : SymbolMapping::mappings) {
        if (filename != "" && sm->filename != filename) continue;
        if (sm->prevId == prevId) return sm;
    }
    return nullptr;
}

SymbolMapping* SymbolMapping::findMappingById(int id){
    for (SymbolMapping* sm : SymbolMapping::mappings) {
        if (sm->id == id) return sm;
    }
    return nullptr;
}

void SymbolMapping::printSymbolMapping(){
    std::cout << "PrevId   " << "Id    " << "newAdress   " << "file" << std::endl;

    for (SymbolMapping* sm : SymbolMapping::mappings) {
        std::cout << sm->prevId << "   " << sm->id << "   " << sm->adress << "   " << sm->filename << std::endl;
    }
    std::cout << std::endl;
}