#include <iostream>
#include <fstream>
#include <string.h>

#define MEMORY_SIZE 0X10000

char memory[MEMORY_SIZE];


int endsWith(const char* string ,const char* end) { // returns 1 if ends otherwise 0
    int l1 = strlen(string) , l2 = strlen(end);
    if (l1 < l2) return 0;
    return strcmp(string + l1 - l2 , end) == 0;
}


int main(int argc , char** argv) {
    if (argc < 2)  {
        std::cout << "Morate uneti fajl koji hocete da emulirate" << std::endl;
        return 1;
    }

    if (!endsWith(argv[1] , ".hex")) {
         std::cout << "Morate uneti fajl koji se zavrsava sa .hex" << std::endl;
        return 2;
    }


    std::fstream input_file;
    input_file.open(argv[1] , std::ios::in | std::ios::binary);
    if (!input_file) {
        std::cout << "Nije mogao da se otvori ulazni fajl!";
        return 3;
    }

    unsigned short size;
    unsigned short adress;
    while (!input_file.eof()) {
        input_file.read((char*)&adress, sizeof(unsigned short));
        input_file.read((char*)&size, sizeof(unsigned short));
        input_file.read(memory + adress, size);
    } 


    std::cout << "Zavrsen program" << std::endl;
    return 0;

}