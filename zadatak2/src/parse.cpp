#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>

#include "objectfilewrapper.h"
#include "myexceptions.h"
#include "symbol.h"
#include "relocationrecord.h"
#include "symbolmappings.h"

std::vector<ObjectFileWrapper*> extractedFiles;


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
    std::cout << "Usao u parsiranje" << std::endl;
    std::unordered_map<Symbol*, char*> symbols;
    std::vector<std::string> names;
    std::vector<RelocationRecord*> records;

    try {
        for (std::string file : inputFiles) {
            ObjectFileWrapper* ofw = ObjectFileWrapper::createObjectWrapper(file);
            Symbol::createNewSymbolTable(symbols , names, ofw);
            extractedFiles.push_back(ofw);
            Symbol::printSymbolTable(ofw->symbols, ofw->symbolNames);
            RelocationRecord::printRelocationRecords(ofw->records);
        }

        if (Symbol::undefined.size() != 0) {
            throw UndefinedSymbolException("Error! Symbol: " + *(Symbol::undefined.begin()) + " undefined");
        }
        
        for (ObjectFileWrapper* ofw: extractedFiles) {
            Symbol::createNewRelocationRecords(records , symbols ,names, ofw);
        } 
        Symbol::printSymbolTable(symbols, names);
        SymbolMapping::printSymbolMapping();
        RelocationRecord::printRelocationRecords(records);
        
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