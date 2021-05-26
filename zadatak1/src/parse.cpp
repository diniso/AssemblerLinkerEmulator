#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

extern int endsWith(const char* string ,const char* end);

std::vector<std::regex> allRegex;

void initRegexs() {
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:)\\s{0,}")); // nalazi samu labelu
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(halt|iret|ret|\\.end)\\s{0,}")); // nalazi fje halt iret ret i direktivu .end
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(\\.global|\\.extern)\\s{0,}([a-zA-z_]{1,})(,[a-zA-z_]{1,}){0,}\\s{0,}")); // nalazi .global .extern
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(\\.word)\\s{0,}([a-zA-z_]{1,}|\\d{1,}|0x[\\d|a-f|A-F]{1,}|0X[\\d|a-f|A-F]{1,})(,[a-zA-z_]{1,}|,\\d{1,}|,0x[\\d|a-f|A-F]{1,}|,0X[\\d|a-f|A-F]{1,}){0,}\\s{0,}")); // nalazi .word
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(\\.section)\\s{0,}([a-zA-z_]{1,})\\s{0,}")); // nalazi .section
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(\\.skip)\\s{0,}(\\d{1,}|0x[\\d|a-f|A-F]{1,}|0X[\\d|a-f|A-F]{1,})\\s{0,}")); // nalazi .skip
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(\\.equ)\\s{0,}([a-zA-z_]{1,})\\s{0,},\\s{0,}(\\d{1,}|0x[\\d|a-f|A-F]{1,}|0X[\\d|a-f|A-F]{1,})\\s{0,}")); // nalazi .equ
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(not|pop|push|int)\\s{0,}(r0|r1|r2|r3|r4|r5|r6|r7|sp|pc|pws)\\s{0,}")); // nalazi sa 1 argumentom istrukcije racunanja
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(xchg|add|sub|mul|div|cmp|and|or|xor|test|shl|shr)\\s{0,}(r0|r1|r2|r3|r4|r5|r6|r7|sp|pc|psw)\\s{0,},\\s{0,}(r0|r1|r2|r3|r4|r5|r6|r7|sp|pc|psw)\\s{0,}")); // nalazi sa 2 argumenta istrukcije racunanja oba registri
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(ldr|str)\\s{0,}(r0|r1|r2|r3|r4|r5|r6|r7|sp|pc|psw)\\s{0,},\\s{0,}(r0|r1|r2|r3|r4|r5|r6|r7|sp|pc|psw|[\\$|\\s]\\d{1,}|[\\$|\\s]0x[\\d|a-f|A-F]{1,}|[\\$|\\s]0X[\\d|a-f|A-F]{1,}|[\\s|\\$|%][a-zA-z_]{1,})\\s{0,}")); // nalazi sa 2 argumenta istrukcije racunanja drugi parametar sve sem reg indirektno sa pomerajem
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(ldr|str)\\s{0,}(r0|r1|r2|r3|r4|r5|r6|r7|sp|pc|psw)\\s{0,},\\s{0,}\\[\\s{0,}(r0|r1|r2|r3|r4|r5|r6|r7|sp|pc|psw)\\s{0,}(\\+\\s{0,}([a-zA-z_]{1,}|\\d{1,}|0x[\\d|a-f|A-F]{1,}|0X[\\d|a-f|A-F]{1,})\\s{0,}){0,1}\\]\\s{0,}")); // nalazi sa 2 argumenta istrukcije racunanja drugi parametar reg indirektno sa pomerajem
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(call|jmp|jeq|jne|jgt)\\s{0,}\\*\\[\\s{0,}(r0|r1|r2|r3|r4|r5|r6|r7|sp|pc|psw)\\s{0,}(\\+\\s{0,}([a-zA-z_]{1,}|\\d{1,}|0x[\\d|a-f|A-F]{1,}|0X[\\d|a-f|A-F]{1,})\\s{0,}){0,1}\\]\\s{0,}")); // nalazi sa skok instukcije sa registarskim indireknim i pomeraj
    allRegex.push_back(std::regex("\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(call|jmp|jeq|jne|jgt)\\s{0,}(\\*r0|\\*r1|\\*r2|\\*r3|\\*r4|\\*r5|\\*r6|\\*r7|\\*sp|\\*pc|\\*psw|[\\*|\\s]\\d{1,}|[\\*|\\s]0x[\\d|a-f|A-F]{1,}|[\\*|\\s]0X[\\d|a-f|A-F]{1,}|[\\s|\\*|%][a-zA-z_]{1,})\\s{0,}")); // nalazi sa skok instukcije sa svim ostalim adresiranjima
}

bool checkEmptyLine(std::string line) {
    for (char c : line) {
        if (c != ' ' && c != '\t') return false;
    }
    return true;
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

    int numOfLine = 1;

    for (std::string str : lines) {
        int index = -1 , n = str.size();

        if (n == 0 || checkEmptyLine(str)) {
            numOfLine++;
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
            numOfLine++;
            continue;
        }

        bool found = false;
        for (std::regex rx : allRegex) {
            if (std::regex_match(line ,rx)) {
                // ovde treba da se obradi linija
                found = true;
                break;
            }
        }

        if (!found) {
            std::cout << "Error on line: "<< numOfLine << " . Couldnt match string: "<< line << std::endl; 
             // exit(1);
        }
        
        numOfLine++;
    } 

  /*  std::string regex1 = "\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(call|jmp|jeq|jne|jgt)\\s{0,}(\\*r0|\\*r1|\\*r2|\\*r3|\\*r4|\\*r5|\\*r6|\\*r7|\\*sp|\\*pc|\\*psw|[\\*|\\s]\\d{1,}|[\\*|\\s]0x[\\d|a-f|A-F]{1,}|[\\*|\\s]0X[\\d|a-f|A-F]{1,}|[\\s|\\*|%][a-zA-z_]{1,})\\s{0,}";
    for (std::string str : lines) {
        if (std::regex_match(str , std::regex(regex1))) {
            std::cout << "Match string: " << str << std::endl; 
        }
        else {
            std::cout << "Didnt match string: "<< str << std::endl; 
        }
    } */

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