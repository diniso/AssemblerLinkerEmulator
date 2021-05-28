#include<vector>
#include<string>
#include <iostream>
#include"constants.h"
#include"symbol.h"

int Symbol::IDSymbols=256;
int Symbol::IDSections=1;
std::vector<std::string> Symbol::names = {};
std::vector<Symbol*> Symbol::symbols = {};

Symbol::Symbol(std::string name,int value , char binding, char section, char type, int redBr) {
    int index = Symbol::getIndexInNames(name);
    if (index == -1) {
        this->name = Symbol::names.size();
        Symbol::names.push_back(name);
    }
    else this->name = index;

    this->value = value;
    this->type = type;
    this->binding = binding;
    this->redBr = redBr;
    this->section = section;
    

    if (this->type == symbol_type_data) this->size = 2;
    else this->size = 0;
}

int Symbol::getIndexInNames(std::string name){
    int n = Symbol::names.size();
    for (int i = 0 ; i < n ; i++) {
        if (Symbol::names[i] == name) return i;
    }
    return -1;
}

Symbol* Symbol::createSymbol(std::string name,int value , char binding, char section, char type){
    Symbol* sym = nullptr;

    if (type == symbol_type_section) { // pravljenje nove sekcije
        sym = new Symbol(name , value , binding , IDSections, type , IDSections);
        IDSections++;
    }
    else sym = new Symbol(name , value , binding , section , type, IDSymbols++);
    Symbol::symbols.push_back(sym);
    return sym;
}

void Symbol::printSymbolTable() {
    std::cout << std::endl;
    std::cout << "RedBr    " << "Name    " << "Value   " << "Size    " << "Type    " << "Binding     " << "Section" << std::endl;

    for (Symbol* sym : Symbol::symbols) {
        if (sym == nullptr) {
            std::cout << "simbol je nullptr";
        }
        std::cout << sym->redBr << "   " << Symbol::names[sym->name] << "   " << sym->value << "    "
        << sym->size << "     ";
        if (sym->type == symbol_type_function) std::cout << "function   ";
        else if (sym->type == symbol_type_data) std::cout << "data    ";
        else if (sym->type == symbol_type_section) std::cout << "section   ";
        else if (sym->type == symbol_type_absolute) std::cout << "absolute symbol      ";

        if (sym->binding == symbol_binding_local) std::cout << "local    ";
        else if (sym->binding == symbol_binding_global) std::cout << "global   ";

        if (sym->section == symbol_undef) std::cout << "UNDEF";
        else if (sym->section == symbol_abs) std::cout << "ABS";
        else std::cout << Symbol::names[Symbol::getSymbolById(sym->section)->name];

        std::cout << std::endl;
    }

    std::cout << std::endl;
}

Symbol* Symbol::getSymbolById(int redBr){
    for (Symbol* sym : Symbol::symbols) {
        if (sym->redBr == redBr) return sym;
    }
    return nullptr;
}