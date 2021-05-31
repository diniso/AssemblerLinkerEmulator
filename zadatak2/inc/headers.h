#include <vector>

#define EI_NIDENT 16

struct HeaderTable {
    unsigned char   e_ident[EI_NIDENT];
    unsigned short      e_type;
    unsigned short      e_machine;
    unsigned int     e_version;
    unsigned int     e_entry;
    unsigned int      e_phoff;
    unsigned int      e_shoff;
    unsigned int     e_flags;
    unsigned short      e_ehsize;
    unsigned short      e_phentsize;
    unsigned short      e_phnum;
    unsigned short      e_shentsize;
    unsigned short      e_shnum;
    unsigned short      e_shstrndx;

    HeaderTable();
};


struct SectionHeader {
    unsigned int sh_name;
    unsigned int sh_type;
    unsigned int sh_flags;
    unsigned int sh_addr;
    unsigned int sh_offset;
    unsigned int sh_size;
    unsigned int sh_link;
    unsigned int sh_info;
    unsigned int sh_addralign;
    unsigned int sh_entsize;

    static SectionHeader* createSectionHeader(int redBr, int type, int sectionSize , int sectionOffset, int numOfEntries = 0);
    static std::vector<SectionHeader*> getSectionHeaders();
    static void destroySectionHeaders();
    SectionHeader();

private:

    SectionHeader(int redBr, int type, int sectionSize , int sectionOffset, int numOfEntries);


    static std::vector<SectionHeader*> sections;
};