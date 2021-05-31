
struct RelocationRecord {
    int symId; // redniBrojSimbola
    int ofsset; // ofset u sekciji
    char section; // broj sekcije ili 0 ako nije definisan ili 255 za apsolutne simbole
    char relocationType;  // 0 ako je aps 1 ako je pc

    static void printRelocationRecords(std::vector<RelocationRecord*>& records);

};