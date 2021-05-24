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
        int index = -1 , n = line.size();
        for (int i = 0; i < n ; i++) {
            if (line[i] == '#') {
                index = i;
                break;
            }
        }
        if (n == 0 || index == 0) continue;

        if (index == -1) lines.push_back(line);
        else lines.push_back(line.substr(0 , index));
    }

    input_file.close();

    initRegexs();

    std::cout << allRegex.size() << std::endl;

    for (std::string str : lines) {
        bool found = false;
        for (std::regex rx : allRegex) {
            if (std::regex_match(str ,rx)) {
                std::cout << "Match string: " << str << std::endl; 
                found = true;
                break;
            }
        }

        if (!found) std::cout << "Didnt match string: "<< str << std::endl; 
        
    }

 /*   std::string regex1 = "\\s{0,}([a-zA-z_]{1,}:){0,1}\\s{0,}(\\.equ)\\s{0,}([a-zA-z_]{1,})\\s{0,},\\s{0,}(\\d{1,}|0x[\\d|a-f|A-F]{1,}|0X[\\d|a-f|A-F]{1,})\\s{0,}";
    for (std::string str : lines) {
        if (std::regex_match(str , std::regex(regex1))) {
            std::cout << "Match string: " << str << std::endl; 
        }
        else {
            std::cout << "Didnt match string: "<< str << std::endl; 
        }
    } */

  /*  std::fstream output_file;
    output_file.open(outputFileName , std::ios::out);
    if (!output_file) {
        std::cout << "Nije mogao da kreira izlazni fajl!";
        input_file.close();
        return 1;
    }
 */

  //  output_file.close();
    
    return 0;
}