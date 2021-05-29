#include <vector>

struct RelocationRecord {
    int symId; // redniBrojSimbola
    int ofsset; // ofset u sekciji
    char section; // broj sekcije ili 0 ako nije definisan ili 255 za apsolutne simbole
    char relocationType;  // 0 ako je aps 1 ako je pc


    static RelocationRecord* createRelocationRecord(int symId , int ofsset , char section , char recolationType); // implementirano
    static void deleteRelocationRecords(); // implementirano
    static void printRelocationRecords();
private:

    RelocationRecord(int symId , int ofsset , char section , char recolationType);
    static std::vector<RelocationRecord*> records;
};