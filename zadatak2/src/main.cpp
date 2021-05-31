#include <string.h>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <vector>

extern int parseFiles(std::vector<std::string>& inputFiles , std::string outputFile , bool isExeFile, std::unordered_map<std::string ,unsigned short>& mapaAdresa);

unsigned short stringToShort(std::string value) {
    std::stringstream ss;
    unsigned short ret;
    if (value.size() > 2 && value[0] == '0' && (value[1] == 'x' || value[1] == 'X'))
        ss << std::hex << value.substr(2 , value.size() -2);
    else ss << value;

    ss >> ret;
    return ret;
}

int getPositionOfChar(char* arr , char c) {
    int index = -1;
    int position = 0;
    while (*arr != 0) {
        if (*arr == c) {
            index = position;
            break;
        }
        arr++;
        position++;
    }
    return index;
}

int startsWith(const char* string ,const char* start) {
    int l1 = strlen(string) , l2 = strlen(start);
    if (l1 < l2) return 0;

    for (int i = 0 ; i < l2 ; i++) {
        if (string[i] != start[i]) return 0;
    }
    return 1;
}

int endsWith(const char* string ,const char* end) { // returns 1 if ends otherwise 0
    int l1 = strlen(string) , l2 = strlen(end);
    if (l1 < l2) return 0;
    return strcmp(string + l1 - l2 , end) == 0;
}

int main(int argc , char** argv) {

    

    bool assemblyFile = false , exeFile = false;
    std::unordered_map<std::string ,unsigned short> mapaAdresa;
    std::vector<std::string> inputFiles;
    std::string outputFile = "";
    for (int i =  1 ; i < argc ; i++) {
        if (!strcmp(argv[i], "-o")) {
            if (i + 1>= argc) continue;

            outputFile = argv[++i];
            continue;
        }

        if (!strcmp(argv[i], "-hex")) {
            exeFile = true;
            continue;
        }

        if (!strcmp(argv[i], "-linkable")) {
            assemblyFile = true;
            continue;
        }

        if (startsWith(argv[i] , "-place=")) {
            int startIndex = getPositionOfChar(argv[i] , '=');
            int endIndex = getPositionOfChar(argv[i] , '@');
            if (startIndex == -1 || endIndex == -1 ) {
                std::cout << "Parametar -place nije u dobrom formatu" << std::endl;
                return 1;
            }
            std::string sectionName = argv[i];
            unsigned short adress = stringToShort(sectionName.substr(endIndex + 1 , sectionName.size()));
            sectionName = sectionName.substr(startIndex + 1 , endIndex - startIndex - 1);

            mapaAdresa.insert(std::pair<std::string, unsigned short>(sectionName , adress));
            continue;
        }

        if (!endsWith(argv[i] , ".o")) {
            std::cout << "Svi ulazni fajlovi moraju da imaju ekstenziju .o" << std::endl;
            return 2;
        }
        inputFiles.push_back(argv[i]);
    }
    if (assemblyFile && exeFile) {
        std::cout << "Ne mozete pokrenuti linker sa opcijama -hex i -linkable u isto vreme" << std::endl;
        return 3;
    }
    if (outputFile == "") {
        std::cout << "Morate uneti naziv izlaznog fajla" << std::endl;
        return 4;
    }
    if (assemblyFile) {
       if (!endsWith(outputFile.c_str() , ".o"))  {
           std::cout << "Fajl sa -linkable mora da ima ekstenziju .o" << std::endl;
           return 5;
       }
    }
    else {
        if (!endsWith(outputFile.c_str() , ".hex"))  {
           std::cout << "Fajl sa -hex mora da ima ekstenziju .hex" << std::endl;
           return 6;
        }
    }

    return parseFiles(inputFiles , outputFile , exeFile , mapaAdresa);
}