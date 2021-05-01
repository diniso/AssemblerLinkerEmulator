#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_set>

extern int endsWith(const char* string ,const char* end);

std::unordered_set<std::string> instructionsSet , directiveSet;

void inicilalizujKolekcije() {
    directiveSet.insert(".global");
    directiveSet.insert(".extern");
    directiveSet.insert(".section");
    directiveSet.insert(".word");
    directiveSet.insert(".skip");
    directiveSet.insert(".equ");
    directiveSet.insert(".end");

    instructionsSet.insert("halt");
    instructionsSet.insert("int");
    instructionsSet.insert("iret");
    instructionsSet.insert("call");
    instructionsSet.insert("ret");
    instructionsSet.insert("jmp");
    instructionsSet.insert("jeq");
    instructionsSet.insert("jne");
    instructionsSet.insert("jgt");
    instructionsSet.insert("push");
    instructionsSet.insert("pop");
    instructionsSet.insert("xchg");
    instructionsSet.insert("add");
    instructionsSet.insert("sub");
    instructionsSet.insert("mul");
    instructionsSet.insert("div");
    instructionsSet.insert("cmp");
    instructionsSet.insert("not");
    instructionsSet.insert("and");
    instructionsSet.insert("or");
    instructionsSet.insert("xor");
    instructionsSet.insert("test");
    instructionsSet.insert("shl");
    instructionsSet.insert("shr");
    instructionsSet.insert("ldr");
    instructionsSet.insert("str");


}

std::vector<std::string> parseLine(std::string line) { // vraca labelu, direktivu ili
    std::vector<std::string> ret(2 , "");

    std::stringstream stream(line , std::ios::in);
    std::string word;
    bool firstWord = true;
    while (!stream.eof()) {
        stream >> word;
        if (word.size() > 0 && word[0] == '#') break; // ako nesto pocinje sa # obustavi obradu
        if (endsWith(word.c_str() , ":")) { // proevi dal je labela
            if (firstWord) {
                ret[0] = word.substr(0 , word.size() -1);
                std::cout << ret[0] << std::endl;
            }
            else {
                // baciti neki izutetak jer labela nije na pocetku
            }
        }
        if (word.size() > 0 && word[0]== '.') { // pocetak direktive
            if (ret[1] != "") {
                if (directiveSet.find(word) != directiveSet.end()){
                    ret[1] = word;
                }
                else {
                    // baciti izuzetak jer directiva ne postoji
                }
            }
            else {
                // baciti izuzetak jer postoji vec direktiva ili linija
            }
            

        }

        firstWord = false;
    }

    return ret;
}

int parseFile(std::string inputFileName , std::string outputFileName) {
    std::fstream input_file;
    input_file.open(inputFileName , std::ios::in);
    if (!input_file) {
        std::cout << "Ulazni fajl ne postoji!";
        return 1;
    }

    std::fstream output_file;
    output_file.open(outputFileName , std::ios::out);
    if (!output_file) {
        std::cout << "Nije mogao da kreira izlazni fajl!";
        input_file.close();
        return 1;
    }

    inicilalizujKolekcije();
    std::string line;
    while (!input_file.eof()) {
        getline(input_file, line);
        parseLine(line);

    }


    output_file.close();
    input_file.close();
    return 0;
}