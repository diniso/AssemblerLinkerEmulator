#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include "constants.h"
#include "symbol.h"
#include "relocationrecord.h"
#include "headers.h"

extern int endsWith(const char* string ,const char* end);

std::vector<std::regex> allRegex;

void initRegexs() {
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:)\\s{0,}")); // nalazi samu labelu
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(halt|iret|ret|\\.end)\\s{0,}")); // nalazi fje halt iret ret i direktivu .end
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(\\.global|\\.extern)\\s{0,}(([a-zA-z_]{1,},\\s{0,}){0,})([a-zA-z_]{1,})\\s{0,}")); // nalazi .global .extern
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(\\.word)\\s{0,}(([a-zA-z_]{1,},\\s{0,}|\\d{1,5},\\s{0,}|0x[\\d|a-f|A-F]{1,4},\\s{0,}|0X[\\d|a-f|A-F]{1,4},\\s{0,}){0,})([a-zA-z_]{1,}|\\d{1,5}|0x[\\d|a-f|A-F]{1,4}|0X[\\d|a-f|A-F]{1,4})\\s{0,}")); // nalazi .word
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(\\.section)\\s{0,}([a-zA-z_]{1,})\\s{0,}")); // nalazi .section
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(\\.skip)\\s{0,}(\\d{1,5}|0x[\\d|a-f|A-F]{1,4}|0X[\\d|a-f|A-F]{1,4})\\s{0,}")); // nalazi .skip
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(\\.equ)\\s{0,}([a-zA-z_]{1,})\\s{0,},\\s{0,}(\\d{1,5}|0x[\\d|a-f|A-F]{1,4}|0X[\\d|a-f|A-F]{1,4})\\s{0,}")); // nalazi .equ
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(not|pop|push|int)\\s{0,}(r0|r1|r2|r3|r4|r5|r6|r7|sp|pc|pws)\\s{0,}")); // nalazi sa 1 argumentom istrukcije racunanja
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(xchg|add|sub|mul|div|cmp|and|or|xor|test|shl|shr)\\s{0,}(r0|r1|r2|r3|r4|r5|r6|r7|sp|pc|psw)\\s{0,},\\s{0,}(r0|r1|r2|r3|r4|r5|r6|r7|sp|pc|psw)\\s{0,}")); // nalazi sa 2 argumenta istrukcije racunanja oba registri
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(ldr|str)\\s{0,}(r0|r1|r2|r3|r4|r5|r6|r7|sp|pc|psw)\\s{0,},\\s{0,}(r0|r1|r2|r3|r4|r5|r6|r7|sp|pc|psw|[\\$|\\s]\\d{1,}|[\\$|\\s]0x[\\d|a-f|A-F]{1,}|[\\$|\\s]0X[\\d|a-f|A-F]{1,}|[\\s|\\$|%][a-zA-z_]{1,})\\s{0,}")); // nalazi sa 2 argumenta istrukcije racunanja drugi parametar sve sem reg indirektno sa pomerajem
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(ldr|str)\\s{0,}(r0|r1|r2|r3|r4|r5|r6|r7|sp|pc|psw)\\s{0,},\\s{0,}\\[\\s{0,}(r0|r1|r2|r3|r4|r5|r6|r7|sp|pc|psw)\\s{0,}(\\+\\s{0,}([a-zA-z_]{1,}|\\d{1,5}|0x[\\d|a-f|A-F]{1,4}|0X[\\d|a-f|A-F]{1,4})\\s{0,}){0,1}\\]\\s{0,}")); // nalazi sa 2 argumenta istrukcije racunanja drugi parametar reg indirektno sa pomerajem
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(call|jmp|jeq|jne|jgt)\\s{0,}\\*\\[\\s{0,}(r0|r1|r2|r3|r4|r5|r6|r7|sp|pc|psw)\\s{0,}(\\+\\s{0,}([a-zA-z_]{1,}|\\d{1,5}|0x[\\d|a-f|A-F]{1,4}|0X[\\d|a-f|A-F]{1,4})\\s{0,}){0,1}\\]\\s{0,}")); // nalazi sa skok instukcije sa registarskim indireknim i pomeraj
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(call|jmp|jeq|jne|jgt)\\s{0,}(\\*r0|\\*r1|\\*r2|\\*r3|\\*r4|\\*r5|\\*r6|\\*r7|\\*sp|\\*pc|\\*psw|[\\*|\\s]\\d{1,5}|[\\*|\\s]0x[\\d|a-f|A-F]{1,4}|[\\*|\\s]0X[\\d|a-f|A-F]{1,4}|[\\s|\\*|%][a-zA-z_]{1,})\\s{0,}")); // nalazi sa skok instukcije sa svim ostalim adresiranjima
}

bool checkEmptyLine(std::string line) {
    for (char c : line) {
        if (c != ' ' && c != '\t') return false;
    }
    return true;
}

int getInstructionsLenght(std::string instruction, int adressType1  , int adressType2) {
    if (instruction == "halt") return 1;
    if (instruction == "int" && adressType1 == reg_dir) return 2;
    if (instruction == "iret" || instruction == "ret") return 1;

    if ((instruction == "call" || instruction == "jmp" || instruction == "jeq" || instruction == "jne" || instruction == "jgt")
        && (adressType1 == reg_indir_displacment || adressType1 == direct || adressType1 == mem_dir || adressType1 == pc_rel)) return 5;
    if ((instruction == "call" || instruction == "jmp" || instruction == "jeq" || instruction == "jne" || instruction == "jgt") 
        && (adressType1 == reg_dir || adressType1 == reg_indir ) ) return 3;

    if (instruction == "xchg" && adressType1 == reg_dir && adressType2 == reg_dir) return 2;
    if ((instruction == "add" || instruction == "sub" || instruction == "mul" || instruction == "div" || instruction == "cmp") 
    &&  (adressType1 == reg_dir && adressType2 == reg_dir)) return 2;
    if ((instruction == "and" || instruction == "or" || instruction == "xor" || instruction == "test" )
    && (adressType1 == reg_dir && adressType2 == reg_dir)) return 2;
    if (instruction == "not" && adressType1 == reg_dir) return 2;
    if ((instruction == "shl" || instruction == "shr")
    && (adressType1 == reg_dir && adressType2 == reg_dir)) return 2;

    if (instruction == "ldr" && adressType1 == reg_dir && 
    (adressType2 == reg_indir_displacment || adressType2 == direct || adressType2 == mem_dir || adressType2 == pc_rel)) return 5;
    if (instruction == "str" && adressType1 == reg_dir && 
    (adressType2 == reg_indir_displacment || adressType2 == mem_dir || adressType2 == pc_rel) ) return 5;
    if ((instruction == "ldr" || instruction == "str") && adressType1 == reg_dir && 
    (adressType2 == reg_dir || adressType2 == reg_indir )) return 3;
    if ((instruction == "push" || instruction == "pop") && adressType1 == reg_dir) return 3;

    throw CantCalculateInstrunctionLenghtException("Cant calculate instrunction lenght: " +  instruction + " " + std::to_string(adressType1) + " " + std::to_string(adressType2));
}

bool isInstructionJump(std::string instruction) {
    if ("call" == instruction || "jmp" == instruction || "jeq" == instruction || "jne" == instruction || "jgt" == instruction ) return true;
    return false;
}

unsigned short stringToShort(std::string value) {
            std::stringstream ss;
            unsigned short ret;
            if (value.size() > 2 && value[0] == '0' && (value[1] == 'x' || value[1] == 'X'))
                ss << std::hex << value.substr(2 , value.size() -2);
            else ss << value;

            ss >> ret;
            return ret;
}

char getAdressTypeCode(int adressType) {
    if (adressType == direct) return directS;
    if (adressType == reg_dir) return reg_dirS;
    if (adressType == reg_indir) return reg_indirS;
    if (adressType == reg_indir_displacment) return reg_indir_displacmentS;
    if (adressType == mem_dir) return mem_dirS;
    if (adressType == pc_rel) return pc_relS;

    throw NoSuchAdressTypeException("Cant regognize adress type: " + std::to_string(adressType));
}
// ovo bi se valjalo malo popraviti
int getAdressType(std::string value, bool isJumpInstruction) {

    if (value[0] == '%') return pc_rel;
    if (value[0] != '*' && isJumpInstruction) return direct;
    if (isJumpInstruction) {
        return getAdressType(value.substr(1 , value.size()), false);
    }

    if ("r0" == value || "r1" == value || "r2" == value || "r3" == value || "r4" == value || "r5" == value || "r6" == value || "r7" == value ) return reg_dir;
    if ("psw" == value || "pc" == value || "sp" == value) return reg_dir;

    if (value[0] == '$') return direct;
    if (value[0] != '[') return mem_dir;
    
    for (char c : value) {
        if (c == '+') return reg_indir_displacment;
    }
    return reg_indir;
}

bool AreBothOperandsReg(std::string instruction) {
    if (instruction == "add" || instruction == "sub" || instruction == "mul" || instruction == "div" || instruction == "xcgh") return true;
    if (instruction == "cmp" || instruction == "and" || instruction == "or" || instruction == "xor" || instruction == "test") return true;
    if (instruction == "shl" || instruction == "shr" ) return true;

    return false;
}

bool DoInstructionHaveOperands(std::string instruction) {
    if (instruction == "halt" || instruction == "ret" || instruction == "iret") return false;

    return true;
}

char getInstructionCode(std::string instruction) {
    if (instruction == "halt") return Ihalt;
    if (instruction == "int") return Iint;
    if (instruction == "iret") return Iiret;
    if (instruction == "call") return Icall;
    if (instruction == "ret") return Iret;
    if (instruction == "jmp") return Ijmp;
    if (instruction == "jeq") return Ijeq;
    if (instruction == "jne") return Ijne;
    if (instruction == "jgt") return Ijgt;
    if (instruction == "push") return Ipush;
    if (instruction == "pop") return Ipop;
    if (instruction == "xchg") return Ixchg;
    if (instruction == "add") return Iadd;
    if (instruction == "sub") return Isub;
    if (instruction == "mul") return Imul;
    if (instruction == "div") return Idiv;
    if (instruction == "cmp") return Icmp;
    if (instruction == "not") return Inot;
    if (instruction == "and") return Iand;
    if (instruction == "or") return Ior;
    if (instruction == "xor") return Ixor;
    if (instruction == "test") return Itest;
    if (instruction == "shl") return Ishl;
    if (instruction == "shr") return Ishr;
    if (instruction == "ldr") return Ildr;
    if (instruction == "str") return Istr;

    throw NoSuchInstructionException("Instruction: " + instruction + " dont exists");
}

char getCodeFor2RegInstruction(std::string regD, std::string regS ) {
    char ans = 0;

    if (regD == "r0") ans |= R0D;
    else if (regD == "r1") ans |= R1D;
    else if (regD == "r2") ans |= R2D;
    else if (regD == "r3") ans |= R3D;
    else if (regD == "r4") ans |= R4D;
    else if (regD == "r5") ans |= R5D;
    else if (regD == "r6") ans |= R6D;
    else if (regD == "r7") ans |= R7D;
    else if (regD == "pc") ans |= PCD;
    else if (regD == "psw") ans |= PSWD;
    else if (regD == "sp") ans |= SPD;

    if (regS == "r0") ans |= R0S;
    else if (regS == "r1") ans |= R1S;
    else if (regS == "r2") ans |= R2S;
    else if (regS == "r3") ans |= R3S;
    else if (regS == "r4") ans |= R4S;
    else if (regS == "r5") ans |= R5S;
    else if (regS == "r6") ans |= R6S;
    else if (regS == "r7") ans |= R7S;
    else if (regS == "pc") ans |= PCS;
    else if (regS == "psw") ans |= PSWS;
    else if (regS == "sp") ans |= SPS;

    return ans;
}

void printArrayAsBytes(char* arr , int size , int sectionId) {
    std::cout << "Velicina sekcije: " << sectionId << " iznosi: " << size << std::endl;
    for (int i = 0 ; i < size ; i++) {
        printf("%.2x " , (unsigned char)arr[i]);
    }
    std::cout << std::endl;
        

}

std::vector<std::string> getSymbolRegLiteralFromInput(std::string sym , bool isJumpInstruction , int adressType) {
    std::vector<std::string> ans;
    if (adressType == reg_dir || adressType == mem_dir) {
        if (isJumpInstruction) ans.push_back(sym.substr(1 , sym.size() -1));
        else ans.push_back(sym);
    }
    else if (adressType == reg_indir) {
        if (isJumpInstruction) ans.push_back(sym.substr(2 , sym.size() -3));
        else ans.push_back(sym.substr(1 , sym.size() -2));
    }
    else if (adressType == reg_indir_displacment) {
        if (isJumpInstruction) sym = sym.substr(2 , sym.size() -3);
        else sym = sym.substr(1 , sym.size() -2);

        for (int i = 0 ; i < sym.size() ; i++) {
            if (sym[i] == '+') {
                ans.push_back(sym.substr(0 , i));
                ans.push_back(sym.substr(i + 1 , sym.size() - i - 1));
                break;
            }
        }
    }
    else if (adressType == pc_rel) {
        ans.push_back("pc");
        ans.push_back(sym.substr(1 , sym.size() -1));
    }
    else if (adressType == direct) {
        if (isJumpInstruction) ans.push_back(sym);
        else ans.push_back(sym.substr(1 , sym.size() - 1));
    }

    return ans;
}

unsigned short getShortValueForSymbolLiteral(std::string literalSymbol) {
    if (literalSymbol[0] < '0' || literalSymbol[0] > '9') { // symbol
        Symbol* sym = Symbol::getSymbolByName(literalSymbol);
        if (sym == nullptr) throw UndefinedSymbolException("Symbol undefined");

        return sym->value;
    }

    return stringToShort(literalSymbol);
}

int parseFile(std::string inputFileName , std::string outputFileName) {
    std::fstream input_file;
    input_file.open(inputFileName , std::ios::in);
    if (!input_file) {
        std::cout << "Ulazni fajl ne postoji!";
        return 1;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(input_file, line)) {
        lines.push_back(line);
    }

    input_file.close();

    initRegexs();

    std::vector<std::vector<std::string>> newLines;

    int numOfLine = 0;
    std::smatch matches;
    std::string labela = "";
    int lc = 0;
    int sectionId = -1;
    bool end = false;
    try {
        for (std::string str : lines) {
            numOfLine++;
            int index = -1 , n = str.size();

            if (n == 0 || checkEmptyLine(str)) {
                continue;
            }

            for (int i = 0; i < n ; i++) {
                if (str[i] == '#') {
                    index = i;
                    break;
                }
            }
            
            std::string line = str;
            if (index != -1) line = str.substr(0 , index);

            if (checkEmptyLine(line)) {
                continue;
            }

            bool found = false;
            for (std::regex rx : allRegex) {
                if (std::regex_match(line, matches ,rx)) {
                    found = true;

                    std::string prviParam = matches[1].str();
                    if (prviParam != "") {
                        labela = prviParam.substr(0 , prviParam.size() - 1);
                    }
                    unsigned n = matches.size();
                    if (n == 2 ) break;

                    std::string directiveOrInstruction = matches[2].str();

                    std::vector<std::string> linija;
                    linija.push_back(directiveOrInstruction);

                    if (labela != "") {
                            Symbol::createSymbol(labela , lc , symbol_binding_local , sectionId , symbol_type_label);
                            labela = "";
                    }

                    if (directiveOrInstruction[0] != '.') {
                        std::vector<std::string> params;
                        for (unsigned i = 3 ; i < matches.size(); i++) {
                            params.push_back(matches[i].str());
                        } 

                        if (params.size() > 2) {
                            bool isJump = isInstructionJump(directiveOrInstruction);
                            int pomeraj = (isJump ? 0 : 1);
                            std::string paramNovi = (isJump ? "*[" : "[");
                            paramNovi += params[0+pomeraj];
                            if (params[2+pomeraj] != "") {
                                    paramNovi += "+";
                                    paramNovi += params[2+pomeraj];
                            }
                            paramNovi += "]";

                            std::string prviParam = params[0];
                            params.clear();
                            if (!isJump) params.push_back(prviParam);
                            params.push_back(paramNovi);
        
                        }
                        if (params.size() > 0) {
                            std::string poslednji = params[params.size() -1];
                            if (poslednji[0] == ' ') params[params.size() -1] = poslednji.substr(1 , poslednji.size()-1);
                        }

                        
                        for (std::string str : params)
                            linija.push_back(str);
                        
                        int paramsSize = params.size();
                        int adressType1 = (paramsSize == 0 ? -1 : getAdressType(params[0], isInstructionJump(directiveOrInstruction)));
                        int adressType2 = (paramsSize <= 1 ? -1 : getAdressType(params[1], isInstructionJump(directiveOrInstruction)));
                        int instructionSize = getInstructionsLenght(directiveOrInstruction,adressType1, adressType2);


                        
                        lc += instructionSize;
                        

                    }
                    else {
    
                            if (directiveOrInstruction == ".skip") {
                                std::string literal = matches[3].str();
                                linija.push_back(literal); 
                                lc += stringToShort(literal);

                            }else if (directiveOrInstruction == ".end") {
                                if (sectionId != -1) Symbol::getSymbolById(sectionId)->size = lc;
                                end = true;


                            }else if (directiveOrInstruction == ".equ") {
                                std::string symbol = matches[3].str();
                                std::string literal = matches[4].str();
                                linija.push_back(symbol);
                                linija.push_back(literal); // napraviti novi simbol
                                
                                Symbol::createSymbol(symbol , stringToShort(literal), symbol_binding_local , symbol_abs , symbol_type_absolute);

                            }else if (directiveOrInstruction == ".section") {
                                std::string sekcija = matches[3].str(); // treba dodati u tabelu simbola
                                linija.push_back(sekcija);
                                Symbol* sym = Symbol::createSymbol(sekcija , 0, symbol_binding_local , sectionId , symbol_type_section);
                                if (sectionId != -1) Symbol::getSymbolById(sectionId)->size = lc;
                                lc = 0;
                                sectionId = sym->section;

                            }else if (directiveOrInstruction == ".word") {
                                
                                std::string prvi = matches[3].str();
                                std::string poslednji = matches[matches.size() -1].str();
                                if (prvi != "") {
                                    int start = 0;
                                    for (int i = 0 ; i < prvi.size(); i++) {
                                        if (prvi[i] == ',') {
                                            std::string symbol = prvi.substr(start , i-start); // kreirati simbol
                                            linija.push_back(symbol);
                                            start = i + 1;
                                            lc +=2;
                                        }
                                        else if (prvi[i] == ' ') start = i + 1;
                                    }
                                }
                                linija.push_back(poslednji);
                                lc += 2;

                            }else if (directiveOrInstruction == ".extern") {

                                std::string prvi = matches[3].str();
                                std::string poslednji = matches[matches.size() -1].str();
                                if (prvi != "") {
                                    int start = 0;
                                    for (int i = 0 ; i < prvi.size(); i++) {
                                        if (prvi[i] == ',') {
                                            std::string symbol = prvi.substr(start , i-start); // kreirati simbol
                                            linija.push_back(symbol);
                                            Symbol::createSymbol(symbol , 0, symbol_binding_global , symbol_undef , symbol_type_undef);
                                            start = i + 1;
                                        }
                                        else if (prvi[i] == ' ') start = i + 1;
                                    }
                                }
                                linija.push_back(poslednji);
                                Symbol::createSymbol(poslednji , 0, symbol_binding_global , symbol_undef , symbol_type_undef);

                            }else if (directiveOrInstruction == ".global") {

                                if (sectionId != -1) throw GlobalSymbolException("On line " + std::to_string(numOfLine) + ": .global should be before every section");

                                std::string prvi = matches[3].str();
                                if (prvi != "") {
                                    int start = 0;
                                    for (int i = 0 ; i < prvi.size(); i++) {
                                        if (prvi[i] == ',') {
                                            std::string symbol = prvi.substr(start , i-start); // kreirati simbol
                                            linija.push_back(symbol);
                                            start = i + 1;
                                        }
                                        else if (prvi[i] == ' ') start = i + 1;
                                    }
                                }
                                linija.push_back(matches[matches.size() -1].str());
                            }
                
                    }
                    
                    if (directiveOrInstruction != ".end" && directiveOrInstruction != ".equ" && directiveOrInstruction != ".extern") 
                        newLines.push_back(linija);
                    break;
                }
            }

            if (end) break;

            if (!found) {
               std::cout << "Error on line: "<< numOfLine << " . Couldnt match string: "<< line << std::endl; 
               exit(3);
            }
            
            
        } 
    }
    catch (DuplicateSymbolException& dse) {
        std::string symbol = dse.getMessage();
        std::cout << "Duplicate symbol: " << symbol << " on line:" << numOfLine-1 << std::endl;
        exit(1);
    }
    catch (GlobalSymbolException& gse) {
        std::cout << gse.getMessage() << std::endl;
        exit(2);
    }
    catch (CantCalculateInstrunctionLenghtException& ex) {
        std::cout << ex.getMessage() << std::endl;
        exit(7);
    }
    
    lines.clear();
    allRegex.clear();

    std::fstream output_file;
    output_file.open(outputFileName , std::ios::out | std::ios::binary);
    if (!output_file) {
        std::cout << "Nije mogao da kreira izlazni fajl! " << outputFileName << std::endl;
        return 1;
    }

    HeaderTable* headertable = new HeaderTable(); 
    output_file.write((char*)headertable , sizeof(HeaderTable));
    

    lc = 0;
    sectionId = -1;
    end = false;


    int numOfSections = 0;
    int sizeOfSections = 0;
    char* data = nullptr;
    int size = 0;

    try {
        for (std::vector<std::string>& vec : newLines) {
            if (vec[0] == ".global") {
                Symbol* sym = Symbol::getSymbolByName(vec[1]);
                if (sym == nullptr) throw UndefinedSymbolException("Error on directive .global " + vec[1] + " : No Such symbol.");
                sym->binding = symbol_binding_global;
                continue;
            }
            else if (vec[0] == ".section") {
                if (sectionId != -1) { // snimi u fajl
                    output_file.write(data, size);
              //      printArrayAsBytes(data , size , sectionId);
                    SectionHeader::createSectionHeader(sectionId , SHT_PROGBITS , size ,sizeOfSections + sizeof(HeaderTable) );
                    numOfSections++;
                    sizeOfSections += size;        
                    delete data;
                }
                Symbol* sym = Symbol::getSymbolByName(vec[1]);
                if (sym == nullptr) throw UndefinedSymbolException("Error on directive .section " + vec[1] + " : No Such symbol.");
                sectionId = sym->section;
                data = new char[sym->size];
                size = sym->size;
                lc = 0;
                
                continue;
            }
            else if (vec[0] == ".word") {
                int i = 1;
                try {
                    for (i = 1 ; i < vec.size() ; i++) {
                        unsigned short value = getShortValueForSymbolLiteral(vec[i]);

                        if (vec[i][0] < '0' || vec[i][0] > '9') { // symbol
                            Symbol* sym = Symbol::getSymbolByName(vec[i]);
                            if (sym->section != symbol_abs) {
                                if (sym->binding == symbol_binding_local) {
                                    RelocationRecord::createRelocationRecord(sym->section , lc,  sectionId , relocation_absolute);
                                }                         
                                else {
                                    RelocationRecord::createRelocationRecord(sym->redBr , lc ,  sectionId , relocation_absolute);
                                    value = 0;
                                }
                            }
                            
                        }
                        data[lc++] = (shortMaskHigherBits & value) >> 8;;
                        data[lc++] = value & shortMaskLowerBits;

                    }
                }
                catch (UndefinedSymbolException& use) {
                    throw UndefinedSymbolException("Error on directive .word " + vec[i] + " : No Such symbol.");
                }
                

                continue;
            }
            else if (vec[0] == ".skip") {
                unsigned short value = stringToShort(vec[1]);
                for (int i = 0 ; i < value ; i++) {
                    data[lc++] = 0;
                }
                continue;
            }
            else { // instrukcije

                std::string instruction = vec[0];
                data[lc++] = getInstructionCode(instruction);
                if (!DoInstructionHaveOperands(instruction)) continue;

                if (AreBothOperandsReg(instruction)) {
                    data[lc++] = getCodeFor2RegInstruction(vec[1] , vec[2]);
                    continue;
                }

                if (instruction == "int" || instruction == "not"  || instruction == "pop" || instruction == "push") {
                    if (instruction == "pop" || instruction == "push")  data[lc++] = (getCodeFor2RegInstruction(vec[1] , "sp"));
                    else data[lc++] = (getCodeFor2RegInstruction(vec[1] , ""));
                    if (instruction == "pop") {
                        data[lc++] = reg_indirS | post_sum;
                    }
                    else if (instruction == "push") {
                        data[lc++] = reg_indirS | pre_sub;
                    }
                    continue;
                }

                int adressType;
                std::vector<std::string> params;
                // ovde jos ostalo instrukcije skoka i ldt i str moze doci do rekolacionih zapisa
                if (isInstructionJump(instruction)) {
                    adressType = getAdressType(vec[1] , true);
                    
                    params = getSymbolRegLiteralFromInput(vec[1] , true , adressType);
                    if (adressType == reg_dir || adressType == reg_indir || adressType == reg_indir_displacment
                        ||  adressType == pc_rel) {
                        data[lc++] = (getCodeFor2RegInstruction("" , params[0]) | 0xf0);
                    }     
                    else data[lc++] = 0xff;

                    data[lc++] = (getAdressTypeCode(adressType) | no_update);
                    
                }else {
                    adressType = getAdressType(vec[2] , false);
                    params = getSymbolRegLiteralFromInput(vec[2] , false , adressType);
                    if (adressType == reg_dir || adressType == reg_indir || adressType == reg_indir_displacment
                        ||  adressType == pc_rel) {
                        data[lc++] = (getCodeFor2RegInstruction(vec[1] , params[0]));
                    }     
                    else data[lc++] = (getCodeFor2RegInstruction(vec[1] , "") | 0x0f);

                    data[lc++] = (getAdressTypeCode(adressType) | no_update);
                }

                // treba da se doda pomeraj ako nije reg_dir i reg_indir
                if (adressType == direct || adressType == mem_dir || adressType == reg_indir_displacment
                        ||  adressType == pc_rel) {

                            try {
                                std::string literalSymbol = ((adressType == direct || adressType == mem_dir) ? params[0] : params[1]);
                                unsigned short value = getShortValueForSymbolLiteral(literalSymbol);
                                 if (literalSymbol[0] < '0' || literalSymbol[0] > '9') { // symbol
                                    Symbol* sym = Symbol::getSymbolByName(literalSymbol);
                                    if (sym->section != symbol_abs) {
                                        if (adressType != pc_rel) {
                                            if (sym->binding == symbol_binding_local) {
                                                RelocationRecord::createRelocationRecord(sym->section , lc,  sectionId , relocation_absolute);         
                                            }                         
                                            else {
                                                RelocationRecord::createRelocationRecord(sym->redBr , lc ,  sectionId , relocation_absolute);
                                                value = 0;
                                            }
                                        }
                                        else { // ovo treba da se popravi
                                            if (sym->binding == symbol_binding_local) {
                                                
                                                if (sym->section != sectionId) RelocationRecord::createRelocationRecord(sym->section , lc,  sectionId , relocation_pc_rel);
                                                value += addend;
                                            }                         
                                            else {
                                                if (sym->section != sectionId) RelocationRecord::createRelocationRecord(sym->redBr , lc ,  sectionId , relocation_pc_rel);
                                                value = addend;
                                            }
                                        } 
                                    }
                                    
                                    
                                } 
                                data[lc++] = (shortMaskHigherBits & value) >> 8;
                                data[lc++] = shortMaskLowerBits & value;

                            }
                            catch (UndefinedSymbolException& use) {
                                throw UndefinedSymbolException("Error on instruction :" + vec[0] + " " + vec[1] + ".Symbol undefined");
                            }    
                        
                        
                    } 
                
            }
            
        }
    }
    catch (UndefinedSymbolException& dup) {
        std::cout << dup.getMessage() << std::endl;
        exit(4);
    }
    catch (NoSuchInstructionException& nse) {
        std::cout << nse.getMessage() << std::endl;
        exit(5);
    }
    catch (NoSuchAdressTypeException& nate) {
        std::cout << nate.getMessage() << std::endl;
        exit(6);
    }

    output_file.write(data, size);
  //  printArrayAsBytes(data , size , sectionId);
    SectionHeader::createSectionHeader(sectionId , SHT_PROGBITS , size ,sizeOfSections + sizeof(HeaderTable) );
    numOfSections++;
    sizeOfSections += size;
    delete data;    // ovde sam zavrsio sa korisnicki sekcijama - treba dodati tabelu simbola relokacije i tabelu stringova 
    newLines.clear();

    

    Symbol* tab = Symbol::createSymbol(SECTION_SYMTAB , 0 , symbol_binding_local , 0 , symbol_type_section);
    Symbol* rel = Symbol::createSymbol(SECTION_REL , 0 , symbol_binding_local , 0 , symbol_type_section);
    Symbol* str = Symbol::createSymbol(SECTION_STRTAB , 0 , symbol_binding_local , 0 , symbol_type_section);

    std::vector<Symbol*> symbols =  Symbol::getSymbolTable();
    std::vector<std::string> strings = Symbol::getStringTable();
    std::vector<RelocationRecord*> records = RelocationRecord::getAllRecords();
    tab->size = symbols.size() * sizeof(Symbol);
    rel->size = records.size() * sizeof(RelocationRecord);


    int velicinaStr = 0;
    for (std::string strin : strings) {
        const char* bytes = strin.c_str();
        velicinaStr += strlen(bytes) + 1;
        output_file.write(bytes , strlen(bytes) + 1);
    }
    str->size = velicinaStr;

    for (RelocationRecord* rec : records) output_file.write((char*)rec , sizeof(RelocationRecord));
    for (Symbol* sym : symbols) output_file.write((char*)sym , sizeof(Symbol));


    SectionHeader::createSectionHeader(str->redBr , SHT_STRTAB , str->size ,sizeOfSections + sizeof(HeaderTable), strings.size() );
    sizeOfSections += str->size; headertable->e_shstrndx = numOfSections; numOfSections++;
    SectionHeader::createSectionHeader(rel->redBr , SHT_RELA , rel->size ,sizeOfSections + sizeof(HeaderTable) , records.size());
    sizeOfSections += rel->size; numOfSections++;
    SectionHeader::createSectionHeader(tab->size , SHT_SYMTAB , tab->size ,sizeOfSections + sizeof(HeaderTable),  symbols.size());
    sizeOfSections += tab->size; numOfSections++;
    

    std::vector<SectionHeader*> sectionHeaders = SectionHeader::getSectionHeaders();
    headertable->e_shnum = sectionHeaders.size();
    headertable->e_shoff = sizeOfSections + sizeof(HeaderTable);

    for (SectionHeader* sh : sectionHeaders) output_file.write((char*) sh , sizeof(SectionHeader));


    output_file.seekg(32, std::ios::beg);
    output_file.write((char*) &headertable->e_shoff, 4);
    output_file.seekg(sizeof(HeaderTable) - 4, std::ios::beg);
    output_file.write((char*)&headertable->e_shnum , 2);
    output_file.write((char*)&headertable->e_shstrndx , 2);

  //  std::cout << headertable->e_shoff  << " " << headertable->e_shnum << " " << headertable->e_shstrndx << std::endl;
    
  //  Symbol::printSymbolTable();
  //  RelocationRecord::printRelocationRecords();

    delete headertable;

    RelocationRecord::deleteRelocationRecords();
    Symbol::destroySymbolTable();
    SectionHeader::destroySectionHeaders();
    
 

    output_file.close(); 
    
    return 0;
}