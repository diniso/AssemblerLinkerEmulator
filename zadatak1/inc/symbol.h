#include<vector>
#include<string>
#include "myexceptions.h"

struct Symbol {
    int name; // offset u nizu imena ili u string sekciji
    int size;
    int redBr;
    int value; // offset u sekciji
    char type;    // function,data,section
    char binding; // local global
    char section; // UNDEF ABS SectionId

private:

    static int IDSymbols;
    static int IDSections;
    static std::vector<std::string> names;
    static std::vector<Symbol*> symbols;
    static int getIndexInNames(std::string name); // implementirano

    Symbol(int name,int value , char binding, char section, char type, int redBr); // implementirano


public:

    static Symbol* getSymbolByName(std::string name); // implementirano
    static Symbol* getSymbolById(int redBr); // implementirano
    static Symbol* createSymbol(std::string name,int value , char binding, char section, char type); // implementirano
    static void printSymbolTable(); // implementirano
    static std::vector<Symbol*> getSymbolTable(); // implementirano
    static std::vector<std::string> getStringTable(); // implementirano
    static void destroySymbolTable(); // implementirano

};

