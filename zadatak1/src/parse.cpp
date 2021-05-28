#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include "constants.h"
#include "symbol.h"

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

    return -1;
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
    

    Symbol::printSymbolTable();
    lines.clear();

    try {
        for (std::vector<std::string>& vec : newLines) {
            if (vec[0] == ".global") {
                Symbol* sym = Symbol::getSymbolByName(vec[1]);
                sym->binding = symbol_binding_global;
            }
            for (std::string str : vec) {
                std::cout << str << " ";
            }
            std::cout << std::endl;
        }
    }
    catch (DuplicateSymbolException& dup) {
     //   std::string symbol = dup.getMessage();
     //   std::cout << "Duplicate symbol: " << symbol << " on line:" << numOfLine << std::endl;
        exit(1);
    }

    Symbol::printSymbolTable();
    Symbol::destroySymbolTable();

 /*   std::fstream output_file;
    output_file.open(outputFileName , std::ios::out);
    if (!output_file) {
        std::cout << "Nije mogao da kreira izlazni fajl!";
        input_file.close();
        return 1;
    }
 

    output_file.close(); */
    
    return 0;
}