

#define relocation_absolute 0
#define relocation_pc_rel 1

#define direct 0
#define reg_dir 1
#define reg_indir 2
#define reg_indir_displacment 3
#define mem_dir 4
#define pc_rel 5

#define addend -2

#define symbol_type_function 0
#define symbol_type_data 1
#define symbol_type_section 2
#define symbol_type_absolute 3
#define symbol_type_undef 4
#define symbol_type_label 5

#define symbol_binding_local 0
#define symbol_binding_global 1

#define symbol_undef 0
#define symbol_abs -127

#define shortMaskHigherBits 0xff00
#define shortMaskLowerBits 0x00ff
// konstante za zaglavlje
#define little_endian 0x01
#define ELFCLASS16 0x03
#define ELFVERSION 0x01
#define FILE_RELOCATABLE 1
#define FILE_EXECUTABLE 2
#define MY_ARCHITECTURE 0x0100

#define SHT_NULL 0x00
#define SHT_PROGBITS 0x01
#define SHT_SYMTAB 0x02
#define SHT_STRTAB 0x03
#define SHT_RELA 0x04

#define SHT_NOFLAGS 0

#define SECTION_SYMTAB ".symtab"
#define SECTION_REL ".rel"
#define SECTION_STRTAB ".strtab"


// instrukcije sve su 1 bajt
#define Ihalt 0x00
#define Iint 0x10
#define Iiret 0x20
#define Icall 0x30
#define Iret 0x40
#define Ijmp 0x50
#define Ijeq 0x51
#define Ijne 0x52
#define Ijgt 0x53
#define Ixchg 0x60
#define Iadd 0x70
#define Isub 0x71
#define Imul 0x72
#define Idiv 0x73
#define Icmp 0x74
#define Inot 0x80
#define Iand 0x81
#define Ior  0x82
#define Ixor 0x83
#define Itest 0x84
#define Ishl 0x90
#define Ishr 0x91
#define Ildr 0xA0
#define Istr 0xB0
#define Ipop 0xA0 // pop i push koriste ldr i str
#define Ipush 0xB0

//registri za source i dest za 2 bajt
#define R0D 0x00
#define R0S 0x00
#define R1D 0x10
#define R1S 0x01
#define R2D 0x20
#define R2S 0x02
#define R3D 0x30
#define R3S 0x03
#define R4D 0x40
#define R4S 0x04
#define R5D 0x50
#define R5S 0x05
#define R6D 0x60
#define R6S 0x06
#define R7D 0x70
#define R7S 0x07
#define PSWD 0x80
#define PSWS 0x08
#define PCD 0x70
#define PCS 0x07
#define SPD 0x60
#define SPS 0x06

// ovako se prevode adresiranja za 3 bajt ako je potrebno
#define directS 0x00
#define reg_dirS 0x01
#define reg_indirS 0x02
#define reg_indir_displacmentS 0x03
#define mem_dirS 0x04
#define pc_relS 0x05

#define no_update 0x00
#define pre_sub 0x10
#define pre_sum 0x20
#define post_sub 0x30
#define post_sum 0x40

#define pre_subS 0x01
#define pre_sumS 0x02
#define post_subS 0x03
#define post_sumS 0x04

