#include <iostream>
#include <string>
#include <string.h>

extern int parseFile(std::string inputFileName , std::string outputFileName);

int endsWith(const char* string ,const char* end) { // returns 1 if ends otherwise 0
    int l1 = strlen(string) , l2 = strlen(end);
    if (l1 < l2) return 0;
    return strcmp(string + l1 - l2 , end) == 0;
}

int main(int argc , char** argv) {

    if (argc != 4 || strcmp("-o" , argv[1]) || !endsWith(argv[2] , ".o") || !endsWith(argv[3], ".s")) {
        std::cout <<"Greska! Format asemblera je asembler -o izlazni_fajl.s ulazni_fajl.o" << std::endl;
        return 1;
    }

    return parseFile(argv[3] , argv[2]);

}