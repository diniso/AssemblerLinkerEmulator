#include "headers.h"
#include "constants_assembler.h"
#include <vector>

std::vector<SectionHeader*> SectionHeader::sections = {};

HeaderTable::HeaderTable() {
    this->e_ident[0] = 0x7f;
    this->e_ident[1] = 'E';
    this->e_ident[2] = 'L';
    this->e_ident[3] = 'F';
    this->e_ident[4] = ELFCLASS16;
    this->e_ident[5] = little_endian;
    this->e_ident[7] = ELFVERSION;

    this->e_type = FILE_RELOCATABLE;
    this->e_machine = MY_ARCHITECTURE;
    this->e_version = ELFVERSION;
    this->e_entry = 0; // virtualna adresa programa, tj. pocetna adresa
    this->e_phoff = 0; // offset do zaglavlja programa - ne koristi se ovde
    this->e_shoff = 0; // ovo treba podesiti offset do tabele sekcija !!!!! od pocetka fajla
    this->e_flags = 0; // flegovi za konkretan procesor
    this->e_ehsize = sizeof(HeaderTable); // velicina zaglavlja
    this->e_phentsize = 0; // velicina zaglavlja programa za 1 ulaz - ne koristi se ovde
    this->e_phnum = 0; // broj ulaza u tabelu zaglavlja - ne koristi se
    this->e_shentsize = sizeof(SectionHeader); // velicina jedno ulaza u tabelu sekcija
    this->e_shnum = 0 ; // broj ulaza u tabelu sekcija - treba postaviti !!!!
    this->e_shstrndx = 0; // ovde treba da stoji indeks u tabeli za string tabelu !!!!!!
}

SectionHeader::SectionHeader(int redBr, int type, int sectionSize , int sectionOffset, int numOfEntries) {
    this->sh_name = redBr;
    this->sh_type = type;
    this->sh_flags = SHT_NOFLAGS;
    this->sh_addr = 0;
    this->sh_offset = sectionOffset;
    this->sh_size = sectionSize;
    this->sh_link = this->sh_info = 0;
    this->sh_addralign = 0;
    this->sh_entsize = numOfEntries;
}

std::vector<SectionHeader*> SectionHeader::getSectionHeaders() {
    return SectionHeader::sections;
}

SectionHeader* SectionHeader::createSectionHeader(int redBr, int type, int sectionSize , int sectionOffset, int numOfEntries) {
    SectionHeader* section = new SectionHeader(redBr , type , sectionSize , sectionOffset , numOfEntries);
    SectionHeader::sections.push_back(section);
    return section;
}

void SectionHeader::destroySectionHeaders() {
    for (SectionHeader* sec : SectionHeader::sections) {
        delete sec;
    }
    SectionHeader::sections.clear();
}