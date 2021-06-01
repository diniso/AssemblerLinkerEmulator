#include <string>
#include <vector>


struct SymbolMapping {
    std::string filename;
    int prevId , id;
    int adress;

    SymbolMapping(std::string filename , int prevId , int id , int adress);

    static SymbolMapping* findMappingByPrevId(int prevId, std::string filename = "");
    static SymbolMapping* findMappingById(int id);
    static void printSymbolMapping();
    static std::vector<SymbolMapping*> mappings;
};