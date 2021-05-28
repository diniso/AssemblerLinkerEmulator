#include<vector>
#include<string>

struct Symbol {
    int name; // offset u nizu imena
    int value; // offset u sekciji
    int size;
    int redBr;
    char type;    // function,data,section
    char binding; // local global
    char section; // UNDEF ABS SectionId

private:

    static int IDSymbols;
    static int IDSections;
    static std::vector<std::string> names;
    static std::vector<Symbol*> symbols;
    static int getIndexInNames(std::string name); // implementirano

    Symbol(std::string name,int value , char binding, char section, char type, int redBr); // implementirano


public:

    static Symbol* getSymbolById(int redBr); // implementirano
    static Symbol* createSymbol(std::string name,int value , char binding, char section, char type); // implementirano
    static char* getBynaryDataForSymbolTable(int& size);
    static char* getBynaryDataForNameTable(int& size);
    static void printSymbolTable(); // implementirano

};

