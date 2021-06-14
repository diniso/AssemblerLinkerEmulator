#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string.h>

#include "myexceptions.h"
#include "constants_assembler.h"
#include "symbol.h"
#include "objectfilewrapper.h"
#include "symbolmappings.h"
#include "relocationrecord.h"

int Symbol::IDSymbols=256;
int Symbol::IDSections=1;
std::unordered_set<std::string> Symbol::undefined = {};
std::unordered_set<std::string> Symbol::defined = {};

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

Symbol* Symbol::getSymbolByName(std::unordered_map<Symbol*, char*>& symbols, std::vector<std::string>& names , std::string name){
    int index = -1;
    for (int i = 0 ; i < names.size() ; i++) {
        if (names[i] == name) {
            index = i;
            break;
        }
    }

    if (index == -1) return nullptr;

    for (auto par : symbols) {
        if (par.first->name == index) return par.first;
    }
    return nullptr;
}

void Symbol::createNewSymbolTable(std::unordered_map<Symbol*, char*>& symbols , std::vector<std::string>& names ,ObjectFileWrapper* ofw ){


    for (auto& par: ofw->symbols) {
        Symbol* s = par.first;
        int type = s->type;
        if (type != symbol_type_section) continue;
        std::string symbolName = ofw->symbolNames[s->name];
        
        bool found = false;
        for (int i = 0 ; i < names.size() ; i++) {
            if (names[i] == symbolName)  { // ovde se spajaju sekcije
                for (auto& par2: symbols) {
                    if (par2.first->name == i) {
                        
                        Symbol* novaSekcija = par2.first;
                        SymbolMapping::mappings.push_back(new SymbolMapping(ofw->filename , s->redBr , novaSekcija->redBr , novaSekcija->size ));        
                        par2.second = (char*)realloc(par2.second , novaSekcija->size + s->size);
                        for (int i = 0 ; i < s->size ; i++)
                            par2.second [novaSekcija->size + i] = par.second[i];
                        novaSekcija->size += s->size;
                    }
                }

                found = true;
                break;  
            }
        } 
        // ovde je kad ubacujem sekciju prvi put
        if (found) continue;
        
        int index = names.size();
        names.push_back(symbolName);
        Symbol* sym = new Symbol(index , 0 , symbol_binding_local , Symbol::IDSections, symbol_type_section , Symbol::IDSections);
        SymbolMapping::mappings.push_back(new SymbolMapping(ofw->filename , s->redBr , sym->redBr , 0 ));
        Symbol::IDSections++;
        sym->size = s->size;
        char* data = par.second;
        par.second = nullptr;

        symbols.insert(std::pair<Symbol*, char*>(sym, data));

    }

    for (auto par: ofw->symbols) {
        Symbol* s = par.first;
        int type = s->type;
        if (type == symbol_type_section) continue;

        std::string symbolName = ofw->symbolNames[s->name];

        if (s->section == symbol_undef) {
            if (Symbol::defined.find(symbolName) == Symbol::defined.end()) Symbol::undefined.insert(symbolName);
            continue;
        }

         for (int i = 0 ; i < names.size() ; i++) {
            if (names[i] == symbolName)  { // ako se nadji isti definisani symbol
                throw DuplicateSymbolException("Symbol: " + symbolName + " is defined more then once");
            }
        } 

        if (Symbol::undefined.find(symbolName) != Symbol::undefined.end()) Symbol::undefined.erase(symbolName);
        
        int index = names.size();
        names.push_back(symbolName);
        SymbolMapping* sm = SymbolMapping::findMappingByPrevId(s->section, ofw->filename);
        Symbol* sym = new Symbol(index , s->value + sm->adress , s->binding , sm->id, s->type , Symbol::IDSymbols++);
        SymbolMapping::mappings.push_back(new SymbolMapping(ofw->filename , s->redBr , sym->redBr , sym->value ));
        Symbol::defined.insert(symbolName);
        symbols.insert(std::pair<Symbol*, char*>(sym, nullptr));

    }
 
} 

void Symbol::createNewRelocationRecords(std::vector<RelocationRecord*>& records, std::unordered_map<Symbol*, char*>& symbols , std::vector<std::string>& names, ObjectFileWrapper* ofw){
       for (RelocationRecord* rec : ofw->records) {

           Symbol* sym = nullptr;

           for (auto& par : ofw->symbols) {
               if (par.first->redBr == rec->symId) {
                   sym = par.first;
                   break;
               }
           }

        SymbolMapping* sm = nullptr;
        if (sym->section == symbol_undef) {
            std::string symbolName = ofw->symbolNames[sym->name];
            for (int i = 0 ; i < names.size(); i++) {
                if (names[i] == symbolName) {
                    for (auto& par : symbols) {
                        if (par.first->name == i) {
                            sm = SymbolMapping::findMappingById(par.first->redBr);
                            break;
                        }
                    }

                    break;
                }
            }
        }
        else sm = SymbolMapping::findMappingByPrevId(rec->symId, ofw->filename);
        SymbolMapping* sm2 = SymbolMapping::findMappingByPrevId(rec->section, ofw->filename);

        records.push_back(new RelocationRecord(sm->id , sm2->adress + rec->offset , sm2->id , rec->relocationType ));
    }  
}

Symbol* Symbol::getSymbolById(std::unordered_map<Symbol*, char*>& symbols, int redBr) {
    for (auto& par : symbols) {
        if (par.first->redBr == redBr) return par.first;
    }
    return nullptr;
}

void Symbol::printSymbolTable(std::unordered_map<Symbol*, char*>& symbols, std::vector<std::string>& names) {

    std::cout << std::endl;
    std::cout << "RedBr    " << "Name    " << "Value   " << "Size    " << "Type    " << "Binding     " << "Section" << std::endl;

    for (auto par : symbols) {
        Symbol* sym = par.first;
        std::cout <<  std::hex <<sym->redBr << "   " << names[sym->name] << "   " << ((short)sym->value) << "    "
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