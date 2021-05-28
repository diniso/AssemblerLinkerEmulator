#include<vector>
#include<string>
#include"constants.h"
#include"symbol.h"

Symbol::Symbol(std::string name,int value , char binding, char section, char type) { // treba dodati size
    int index = Symbol::getIndexInNames(name);
    if (index == -1) this->name = Symbol::names.size();
    else this->name = index;

    this->value = value;
    this->type = type;
    this->binding = binding;
    this->section = section;
    this->redBr = Symbol::ID++;
}

int Symbol::getIndexInNames(std::string name){
    int n = Symbol::names.size();
    for (int i = 0 ; i < n ; i++) {
        if (Symbol::names[i] == name) return i;
    }
    return -1;
}

Symbol* Symbol::createSymbol(std::string name,int value , char binding, char section, char type){
    Symbol* sym = new Symbol(name , value , binding , section , type);
    Symbol::symbols.push_back(sym);
    return sym;
}