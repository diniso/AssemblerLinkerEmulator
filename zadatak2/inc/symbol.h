#include <vector>
#include <string>
#include <unordered_map>

struct Symbol {
    int name; // offset u nizu imena ili u string sekciji
    int size;
    int redBr;
    int value; // offset u sekciji
    char type;    // function,data,section
    char binding; // local global
    char section; // UNDEF ABS SectionId

    static void printSymbolTable(std::unordered_map<Symbol*, char*>& symbols, std::vector<std::string>& names);
    static Symbol* createSymbol(std::unordered_map<Symbol*, char*>& symbols , std::vector<std::string>& names , std::string name,int value , char binding, char section, char type);

    Symbol() {}

private:
    static int IDSymbols;
    static int IDSections;

    Symbol(int name,int value , char binding, char section, char type, int redBr);

};