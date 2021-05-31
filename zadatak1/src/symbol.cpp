#include<vector>
#include<string>
#include <iostream>
#include"constants.h"
#include"symbol.h"


int Symbol::IDSymbols=256;
int Symbol::IDSections=1;
std::vector<std::string> Symbol::names = {};
std::vector<Symbol*> Symbol::symbols = {};

std::vector<std::string> Symbol::getStringTable() {
    return Symbol::names;
}

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

std::vector<Symbol*> Symbol::getSymbolTable(){
    return Symbol::symbols;
}


int Symbol::getIndexInNames(std::string name){
    int n = Symbol::names.size();
    for (int i = 0 ; i < n ; i++) {
        if (Symbol::names[i] == name) return i;
    }
    return -1;
}

Symbol* Symbol::getSymbolByName(std::string name) {
    int index = Symbol::getIndexInNames(name);
    if (index == -1) return nullptr;

    for (Symbol* sym : Symbol::symbols) {
        if (sym->name == index) return sym;
    }

    return nullptr;
}

Symbol* Symbol::createSymbol(std::string name,int value , char binding, char section, char type){

    int index = Symbol::getIndexInNames(name);
    if (index != -1) throw DuplicateSymbolException(name);
    index = Symbol::names.size();
    Symbol::names.push_back(name);
    
    Symbol* sym = nullptr;

    if (type == symbol_type_section) { // pravljenje nove sekcije
        sym = new Symbol(index , value , binding , IDSections, type , IDSections);
        IDSections++;
    }
    else sym = new Symbol(index , value , binding , section , type, IDSymbols++);
    Symbol::symbols.push_back(sym);
    return sym;
}

void Symbol::printSymbolTable() {
    std::cout << std::endl;
    std::cout << "RedBr    " << "Name    " << "Value   " << "Size    " << "Type    " << "Binding     " << "Section" << std::endl;

    for (Symbol* sym : Symbol::symbols) {
        std::cout << sym->redBr << "   " << Symbol::names[sym->name] << "   " << ((short)sym->value) << "    "
        << sym->size << "     ";
        if (sym->type == symbol_type_function) std::cout << "function   ";
        else if (sym->type == symbol_type_data) std::cout << "data    ";
        else if (sym->type == symbol_type_section) std::cout << "section   ";
        else if (sym->type == symbol_type_absolute) std::cout << "absolute symbol      ";
        else if (sym->type == symbol_type_undef) std::cout << "undef    ";
        else if (sym->type == symbol_type_label) std::cout << "labela   ";

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

void Symbol::destroySymbolTable() {
    Symbol::IDSymbols=256;
    Symbol::IDSections=1;
    Symbol::names.clear();
    for (Symbol* sym : Symbol::symbols) {
        delete sym;
    }
    Symbol::symbols.clear();
}