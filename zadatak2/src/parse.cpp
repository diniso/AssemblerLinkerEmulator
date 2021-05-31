#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>

#include "objectfilewrapper.h"
#include "myexceptions.h"
#include "symbol.h"
#include "relocationrecord.h"

std::vector<ObjectFileWrapper*> extractedFiles;


void deleteResources() {
    for (ObjectFileWrapper* ofw : extractedFiles) {
        delete ofw;
    }
    extractedFiles.clear();
}


int parseFiles(std::vector<std::string>& inputFiles , std::string outputFile , bool isExeFile, std::unordered_map<std::string ,unsigned short>& mapaAdresa) {
    std::cout << "Usao u parsiranje" << std::endl;

    try {
        for (std::string file : inputFiles) {
            ObjectFileWrapper* ofw = ObjectFileWrapper::createObjectWrapper(file);
            std::cout << "Naziv fajla: " << ofw->filename << std::endl;
            Symbol::printSymbolTable(ofw->symbols, ofw->symbolNames);
            RelocationRecord::printRelocationRecords(ofw->records);
            std::cout << std::endl;
        }
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
   
    return 0;
}