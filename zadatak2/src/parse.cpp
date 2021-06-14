#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

#include "objectfilewrapper.h"
#include "myexceptions.h"
#include "symbol.h"
#include "relocationrecord.h"
#include "symbolmappings.h"
#include "constants_assembler.h"

std::vector<ObjectFileWrapper*> extractedFiles;

bool isoverllap(std::vector<std::pair<int,int>> zauzeteAdrese , int y1 , int y2) {

    for (auto& par : zauzeteAdrese) {
        if (std::max(par.first , y1) <= std::min(par.second , y2)) return true;
    }
    return false;
}


void setAdresses(std::unordered_map<std::string ,unsigned short>& mapaAdresa, std::unordered_map<Symbol*, char*>& symbols, std::vector<std::string>& names) {
    std::vector<std::pair<int,int>> zauzeteAdrese;

        for (auto& par : mapaAdresa) {
            Symbol* sym = Symbol::getSymbolByName(symbols , names , par.first);
            if (sym == nullptr) {
                throw UndefinedSymbolException("Postavili ste adresu simbola koji ne postoji. " + par.first);
            }
            if (isoverllap(zauzeteAdrese, par.second , par.second + sym->size)) {
                throw AdressOverlapesException("Adrese sekcija koje ste postavili se preklapaju");
            }

            zauzeteAdrese.push_back(std::pair<int,int>(par.second , par.second + sym->size));
            sym->value = par.second;

        }

        int maxAdress = 0xFF00;

        for (auto& par : symbols ) {
            Symbol* sym = par.first;
            if (sym->type != symbol_type_section) continue;

            if (mapaAdresa.find(names[sym->name]) != mapaAdresa.end()) {
              //  std::cout << "Ne treba adresa za: " << names[sym->name] << std::endl;
                continue;
            }

            bool pronasao = false;
            for (int i = maxAdress - 1; i > 0 ; i-= sym->size) {
                if (!isoverllap(zauzeteAdrese, i , i + sym->size) && i + sym->size < maxAdress) {
                    pronasao = true;
                //    std::cout << "Adresa za: " << names[sym->name] << " je: " << i <<std::endl;
                    sym->value = i;
                    zauzeteAdrese.push_back(std::pair<int,int>(i , i + sym->size));
                    break;
                }
            }

            if (!pronasao) { // baciti izuzetak
                throw CantFindAdressException("Nije uspeo da pronadje adresu za simbol: " + names[sym->name]);
             //   std::cout << "" << std::endl;
            }
        }

}

void printSection(char* arr , int size) {
    std::cout << "Velicina sekcije: " << size << std::endl;
    for (int i = 0 ; i < size ; i++) {
        printf("%.2x " , (unsigned char)arr[i]);
    }
    std::cout << std::endl;
}

void deleteResources() {
    for (ObjectFileWrapper* ofw : extractedFiles) {
        delete ofw;
    }
    extractedFiles.clear();
}

//    std::cout << "Naziv fajla: " << ofw->filename << std::endl;
         //   Symbol::printSymbolTable(ofw->symbols, ofw->symbolNames);
         //   RelocationRecord::printRelocationRecords(ofw->records);
         //   std::cout << std::endl;


int parseFiles(std::vector<std::string>& inputFiles , std::string outputFile , bool isExeFile, std::unordered_map<std::string ,unsigned short>& mapaAdresa) {
    std::unordered_map<Symbol*, char*> symbols;
    std::vector<std::string> names;
    std::vector<RelocationRecord*> records;

    try {
        for (std::string file : inputFiles) {
            ObjectFileWrapper* ofw = ObjectFileWrapper::createObjectWrapper(file);
            Symbol::createNewSymbolTable(symbols , names, ofw);
            extractedFiles.push_back(ofw);
         /*   Symbol::printSymbolTable(ofw->symbols, ofw->symbolNames);
            RelocationRecord::printRelocationRecords(ofw->records); */
        }

        if (Symbol::undefined.size() != 0) {
            throw UndefinedSymbolException("Error! Symbol: " + *(Symbol::undefined.begin()) + " undefined");
        }
        
        for (ObjectFileWrapper* ofw: extractedFiles) {
            Symbol::createNewRelocationRecords(records , symbols ,names, ofw);
        } 

        if (isExeFile) {
            setAdresses (mapaAdresa, symbols, names);

            for (auto& par : symbols) {
                Symbol* sym = par.first;
                if (sym->type == symbol_type_section) continue;

                Symbol* sec = Symbol::getSymbolById(symbols , sym->section);
                sym->value += sec->value;
            }

            for (RelocationRecord* rec : records) {
                char* data = nullptr;
                Symbol* sym = nullptr;
                Symbol* sec = nullptr;
                for (auto& par : symbols) {
                    if (par.first->redBr == rec->section) {
                        data = par.second;
                        sec = par.first;
                    }

                    if (par.first->redBr == rec->symId) {
                        sym = par.first;
                    }
                }
                sym = Symbol::getSymbolById(symbols , sym->section);


                short value = (data[rec->offset + 1] | (data[rec->offset] << 8));
                value += ((short)sym->value);
                data[rec->offset] = (shortMaskHigherBits & value) >> 8;
                data[rec->offset+1] = value & shortMaskLowerBits; 
            }

       /*     for (auto& par : symbols) {
                if (par.second == nullptr) continue;

                std::cout << "Sekcija: " << names[par.first->name] << std::endl;
                printSection(par.second , par.first->size);

                //std::endl;
            } */

            std::fstream output_file;
            output_file.open(outputFile , std::ios::out | std::ios::binary);
            if (!output_file) {
                throw CantOpenFileException("Nije mogao da kreira izlazni fajl! " + outputFile);
            }

            short val ;

            for (auto& par : symbols) {
                if (par.second == nullptr) continue;

                val = par.first->value;
                output_file.write((char*) &val , sizeof(val));

                val = par.first->size;
                output_file.write((char*) &val , sizeof(val));

                output_file.write(par.second , par.first->size);
            }

            output_file.close();

        }else {

        }


        deleteResources();
     

     /*   Symbol::printSymbolTable(symbols, names);
        SymbolMapping::printSymbolMapping();
        RelocationRecord::printRelocationRecords(records); */
        
    }
    catch (CantOpenFileException& cofe) {
        std::cout << cofe.getMessage() << std::endl ;
        deleteResources();
        return 1;
    }
    catch (NotRelocatableObjectFileException& nre) {
        std::cout << nre.getMessage() << std::endl ;
        deleteResources();
        return 2;
    }
    catch (NotElfFileException& nee) {
        std::cout << nee.getMessage() << std::endl ;
        deleteResources();
        return 3;
    }
    catch (UndefinedSymbolException& use) {
        std::cout << use.getMessage() << std::endl ;
        deleteResources();
        return 4;
    }
    catch (DuplicateSymbolException& dse) {
        std::cout << dse.getMessage() << std::endl ;
        deleteResources();
        return 5;
    }
   
    return 0;
}