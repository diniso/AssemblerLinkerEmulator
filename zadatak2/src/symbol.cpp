#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include "myexceptions.h"
#include "constants_assembler.h"
#include "symbol.h"

int Symbol::IDSymbols=256;
int Symbol::IDSections=1;

Symbol::Symbol(int name,int value , char binding, char section, char type, int redBr) {
    
    this->name = name;
    this->value = value;
    this->type = type;
    this->binding = binding;
    this->redBr = redBr;
    this->section = section;
    

    if (this->type == symbol_type_data) this->size = 2;
    else this->size = 0;
}

Symbol* Symbol::createSymbol(std::unordered_map<Symbol*, char*>& symbols , std::vector<std::string>& names, std::string name,int value , char binding, char section, char type){


    for (int i = 0 ; i < names.size() ; i++) {
        if (names[i] == name) throw DuplicateSymbolException(name);
    }
    int index = names.size();
    names.push_back(name);
    
    Symbol* sym = nullptr;

    if (type == symbol_type_section) { // pravljenje nove sekcije
        sym = new Symbol(index , value , binding , IDSections, type , IDSections);
        IDSections++;
    }
    else sym = new Symbol(index , value , binding , section , type, IDSymbols++);
    symbols.insert(std::pair<Symbol*, char*>(sym, nullptr));
    return sym;
}

void Symbol::printSymbolTable(std::unordered_map<Symbol*, char*>& symbols, std::vector<std::string>& names) {

    std::cout << std::endl;
    std::cout << "RedBr    " << "Name    " << "Value   " << "Size    " << "Type    " << "Binding     " << "Section" << std::endl;

    for (auto par : symbols) {
        Symbol* sym = par.first;
        std::cout << sym->redBr << "   " << names[sym->name] << "   " << ((short)sym->value) << "    "
        << sym->size << "     ";
        if (sym->type == symbol_type_function) std::cout << "function   ";
        else if (sym->type == symbol_type_data) std::cout << "data    ";
        else if (sym->type == symbol_type_section) std::cout << "section   ";
        else if (sym->type == symbol_type_absolute) std::cout << "absolute symbol      ";
        else if (sym->type == symbol_type_undef) std::cout << "undef    ";
        else if (sym->type == symbol_type_label) std::cout << "labela   ";

        if (sym->binding == symbol_binding_local) std::cout << "local    ";
        else if (sym->binding == symbol_binding_global) std::cout << "global   ";

        std::string sectionName = "";
        for (auto par2: symbols) {
            Symbol* sym2 = par2.first;
            if (sym2->redBr == sym->section) {
                sectionName = names[sym2->name];
                break;
            }
        }

        if (sym->section == symbol_undef) std::cout << "UNDEF";
        else if (sym->section == symbol_abs) std::cout << "ABS";
        else std::cout << sectionName;

        std::cout << std::endl;
    }

    std::cout << std::endl;
}