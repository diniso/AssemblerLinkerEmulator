#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>

#include "objectfilewrapper.h"
#include "symbol.h"
#include "relocationrecord.h"
#include "headers.h"
#include "myexceptions.h"
#include "constants_assembler.h"

ObjectFileWrapper::~ObjectFileWrapper() {
    for (auto par: symbols) {    
        delete par.first;
        if (par.second != nullptr) delete par.second;
    }
    symbols.clear();
    for (RelocationRecord* rec : records) delete rec; 
    records.clear();
    symbolNames.clear();
}

ObjectFileWrapper::ObjectFileWrapper(std::string filename) {
    this->filename = filename;
}


ObjectFileWrapper* ObjectFileWrapper::createObjectWrapper(std::string inputFileName) {
    ObjectFileWrapper* ofw = new ObjectFileWrapper(inputFileName);

    std::ifstream file;
    file.open(inputFileName , std::ios::in | std::ios::binary);
    if (!file) throw CantOpenFileException("Cant open file: " + inputFileName);

    HeaderTable* ht = new HeaderTable();
    file.read((char*)ht, sizeof(HeaderTable));

    if (ht->e_ident[0] != 0x7f || ht->e_ident[1] != 'E' || ht->e_ident[2] != 'L' || ht->e_ident[3] != 'F') throw NotElfFileException("Program moze da radi samo sa elf formatom");

    if (ht->e_type != FILE_RELOCATABLE) throw NotRelocatableObjectFileException("Linker radi samo sa relokacionim objektnim fajlovima");

    unsigned int velicinaTabeleSekcije = ht->e_shentsize;
    unsigned int offsetTabele = ht->e_shoff;
    unsigned int brojSekcija = ht->e_shnum;
    unsigned int strSekcija = ht->e_shstrndx;

    std::vector<SectionHeader*> sections;

    file.seekg(offsetTabele , std::ios::beg);

    for (int i = 0 ; i < brojSekcija; i++) {
        SectionHeader* sekcija = (SectionHeader*)malloc(sizeof(SectionHeader));
        file.read((char*) sekcija , sizeof(SectionHeader));
        sections.push_back(sekcija);
    }

    file.seekg(sections[strSekcija]->sh_offset , std::ios::beg);
    for (int i = 0 ; i < sections[strSekcija]->sh_entsize; i++) {
        char c;
        std::string element = "";
        while (1) {
            file.read(&c , 1);
            if (c == 0) break;
            element.push_back(c);
        }
        ofw->symbolNames.push_back(element);
    } // ucitani nazive stringova

    file.seekg(sections[strSekcija+1]->sh_offset , std::ios::beg);
    for (int i = 0 ; i < sections[strSekcija+1]->sh_entsize; i++) {
        RelocationRecord* r = (RelocationRecord*) malloc(sizeof(RelocationRecord));
        file.read((char*)r , sizeof(RelocationRecord));
        ofw->records.push_back(r);
    } // ucitani relokacioni zapisi 

    file.seekg(sections[strSekcija+2]->sh_offset , std::ios::beg);
    for (int i = 0 ; i < sections[strSekcija+2]->sh_entsize; i++) {
        Symbol* s = (Symbol*) malloc(sizeof(Symbol));
        file.read((char*)s , sizeof(Symbol));
        if (ofw->symbolNames[s->name] == SECTION_SYMTAB || ofw->symbolNames[s->name] == SECTION_REL || ofw->symbolNames[s->name] == SECTION_STRTAB
            || (s->type != symbol_type_section && s->binding != symbol_binding_global)) {
                free(s);
                continue;
            }
        ofw->symbols.insert(std::pair<Symbol* , char*> (s, nullptr));
    } // ucitani symboli

    for (auto& par : ofw->symbols) {
        if (par.first->type != symbol_type_section) continue;

        file.seekg(sections[par.first->redBr-1]->sh_offset , std::ios::beg);
        par.second = (char*)malloc(par.first->size);

        for (int i = 0 ; i < par.first->size ; i++) {
            file.read(par.second + i , 1);
        //    printf("%.2x " , (unsigned char)par.second[i]);
        }
        //std::cout << std::endl;

    }
    

    delete ht;
    file.close();
    return ofw;
}
