#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

struct ObjectFileWrapper;
struct RelocationRecord;

struct Symbol {
    int name; // offset u nizu imena ili u string sekciji
    int size;
    int redBr;
    int value; // offset u sekciji
    char type;    // function,data,section
    char binding; // local global
    char section; // UNDEF ABS SectionId

    static void printSymbolTable(std::unordered_map<Symbol*, char*>& symbols, std::vector<std::string>& names);
    static void createNewSymbolTable(std::unordered_map<Symbol*, char*>& symbols , std::vector<std::string>& names ,ObjectFileWrapper* ofw );
    static void createNewRelocationRecords(std::vector<RelocationRecord*>& records, std::unordered_map<Symbol*, char*>& symbols, std::vector<std::string>& names, ObjectFileWrapper* ofw);
    static Symbol* getSymbolByName(std::unordered_map<Symbol*, char*>& symbols,  std::vector<std::string>& names , std::string name);
    Symbol() {}

    static std::unordered_set<std::string> undefined;

private:
    static int IDSymbols;
    static int IDSections;
    static std::unordered_set<std::string> defined;

    

    Symbol(int name,int value , char binding, char section, char type, int redBr);

};