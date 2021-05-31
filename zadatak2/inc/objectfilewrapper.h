#include <vector>
#include <string>
#include <unordered_map>


struct Symbol;
struct RelocationRecord;

struct ObjectFileWrapper {
    std::string filename;
    std::vector<RelocationRecord*> records;
    std::unordered_map<Symbol*, char*> symbols;
    std::vector<std::string> symbolNames;

    ObjectFileWrapper(std::string filename);
    ~ObjectFileWrapper();

    static ObjectFileWrapper* createObjectWrapper(std::string inputFileName);

    
};