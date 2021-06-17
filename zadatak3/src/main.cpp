#include <iostream>
#include <fstream>
#include <algorithm>
#include <string.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "context.h"
#include "constants_assembler.h"
#include "myexceptions.h"

#define MEMORY_SIZE 0X10000
#define term_in 0xff02
#define term_out 0xff00

#define interrupt_terminal 3
#define interrupt_tajmer 2
#define timer_config 0xff10

char memory[MEMORY_SIZE];
bool prekid = false;
unsigned char ulazUIvtp = 0;

struct termios orig_termios;

void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);
  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void generateInterrupt(unsigned char ulaz) {
    prekid = true;
    ulazUIvtp = ulaz;
}


int endsWith(const char* string ,const char* end) { // returns 1 if ends otherwise 0
    int l1 = strlen(string) , l2 = strlen(end);
    if (l1 < l2) return 0;
    return strcmp(string + l1 - l2 , end) == 0;
}

unsigned short readData(unsigned short adress) {
    unsigned short data = 0;
    data |= (memory[adress] << 8);
    data |= memory[adress+1] & 0x00ff;

    return data;
}

void storeDate(unsigned short adress , unsigned short data) {
    memory[adress] = (data & shortMaskHigherBits) >> 8;
    memory[adress+1] = data & shortMaskLowerBits;
}

unsigned char extractHeigherBits(char data) {
    return (data&0xf0) >> 4;
}

unsigned char extractLowerBits(char data) {
    return data&0x0f;
}

bool doINeedToReadOpperand(unsigned char addr) {
    if (addr == directS || addr == reg_indir_displacmentS || addr == mem_dirS || addr == pc_relS) return true;
    return false;
}


void writeDataIntoReg(unsigned char registry , unsigned short data, Context*  context) {
    switch (registry) {
        case 0x00:
            context->r0 = data;
            break;
        
        case 0x01:
            context->r1 = data;
            break;
        
        case 0x02:
            context->r2 = data;
            break;

        case 0x03:
            context->r3 = data;
            break;

        case 0x04:
            context->r4 = data;
            break;

        case 0x05:
            context->r5 = data;
            break;

        case 0x06:
            context->sp = data;
            break;

        case 0x07:
            context->pc = data;
            break;

        case 0x08:
            context->psw = data;
            break;
    }
}

unsigned short readDataFromReg(unsigned char registry, Context*  context) {
    switch (registry) {
        case 0x00:
            return context->r0;
            break;
        
        case 0x01:
            return context->r1;
            break;
        
        case 0x02:
            return context->r2;
            break;

        case 0x03:
            return context->r3;
            break;

        case 0x04:
            return context->r4;
            break;

        case 0x05:
            return context->r5;
            break;

        case 0x06:
            return context->sp;
            break;

        case 0x07:
            return context->pc;
            break;

        case 0x08:
            return context->psw;
            break;

    }

    throw NoSuchRegisterException("Ne postoji registraj koji ima u binarnom vrednost: " + std::to_string(registry));
}

unsigned short readDataEveryAdressType(unsigned char addrtype , Context* context, unsigned char reg = 0, unsigned short pomeraj = 0) {
    short data;
    switch (addrtype) {
        case 0x00:
            return pomeraj;

        
        case 0x01:
            return readDataFromReg(reg , context );
        
        case 0x02:
            data = readDataFromReg(reg , context );
            return readData(data);

        case 0x03:
            data = readDataFromReg(reg , context ) + pomeraj;
            return readData(data);

        case 0x04:
            return readData(pomeraj);

        case 0x05:
            data = readDataFromReg(reg , context );
            return readData(data + pomeraj); 


    }

    throw NoSuchAddresTypeException("Ne postoji nacin adresiranja koji ima u binarnom vrednost: " + std::to_string(addrtype));
}

void writeDataEveryAdressType(unsigned char addrtype , Context* context, unsigned short data , unsigned char reg = 0, unsigned short pomeraj = 0) {
    short adress;
    switch (addrtype) {
        
        case 0x01:
            writeDataIntoReg(addrtype , data , context);
            return;
        
        case 0x02:
            adress = readDataFromReg(reg , context );
            storeDate(adress, data);
            return;

        case 0x03:
            adress = readDataFromReg(reg , context ) + pomeraj;
            storeDate(adress, data);
            return;

        case 0x04:
            storeDate(pomeraj, data);
            return;
        case 0x05:
            data = readDataFromReg(reg , context ) + pomeraj;
            storeDate(adress, data);
            return;


    }

    throw NoSuchAddresTypeException("Ne postoji nacin adresiranja koji ima u binarnom vrednost: " + std::to_string(addrtype));
}

void setFlags(unsigned short first , unsigned short second , unsigned short result , Context* context , unsigned char instruction ) {

    switch (instruction) {
        case Itest:
            context->psw &= 0xfff6;
            break;

        case Ishr:

            context->psw &= 0xfff2;
            if ((1 << (second-1))& first) {
                context->psw |= 0x0004;
            }
            break;

        case Ishl:

            context->psw &= 0xfff2;
            if ((0x8000 >> (second-1))& first) {
                context->psw |= 0x0004;
            }
            break;

        case Icmp:
            context->psw &= 0xfff0;
            if (!(first&0x8000) && (second&0x8000)) {
                context->psw |= 0x0004;
            }
            if ((!(first&0x8000) && !(second&0x8000) && (result&0x8000))
             || ((first&0x8000) && (second&0x8000) && !(result&0x8000))) {
                context->psw |= 0x0002;
             }

            break;

        default:
            throw NoFlagsForInstructionException("Ne stavljaju se flegovi za instrukciju: " + instruction);
            break;
        
    }
    if (result == 0) {
        context->psw |= 0x0001;
    }
    if (result & 0x8000) {
        context->psw |= 0x0008;
    }
  //  printf("Psw: %x\n" , context->psw);     
    
}

unsigned short getZeroFlag(Context* context) {
    return context->psw & 0x0001;
}

unsigned short getCarryFlag(Context* context) {
    return context->psw & 0x0004;
}

unsigned short getOverflowFlag(Context* context) {
    return context->psw & 0x0002;
}

unsigned short getNegativeFlag(Context* context) {
    return context->psw & 0x0008;
}

void* waitForKeys(void* data) {
    char c;

    while (read(STDIN_FILENO, &c, 1) == 1){ // && c != 'q') {
     /*   if (iscntrl(c)) {
            printf("%d\n", c);
        } else {
            printf("%d ('%c')\n", c, c);
        }    */
    storeDate(term_out , c);
    generateInterrupt(interrupt_terminal);
  
  }

  return 0;
}

void destroyThreads() {
    disableRawMode();
}

void* printDataToOutput(void* data) {
    unsigned short val;
    while (true) {
        val = readData(term_out);
        if (val != 0 ){ //&& !iscntrl(val)) {
            storeDate(term_out , 0);   
            putchar(val);       
        }
    }
    return 0;
}

void* timerFunction(void* data) {
    long delay = 500;
    while (true) {
        delay = readData(timer_config);
        switch(delay) {
            case 0: 
            
                delay = 500;
                break;

            case 1:

                delay = 1000;
                break;

            case 2:

                delay = 1500;
                break;

            case 3:

                delay = 2000;
                break;

            case 4:

                delay = 5000;
                break;

            case 5:

                delay = 10000;
                break;

            case 6:

                delay = 30000;
                break;

            case 7:

                delay = 60000;
                break;

            default:

                delay = 500;
                break;
        }

        usleep(delay* 1000);
        generateInterrupt(interrupt_tajmer);
    }
}

void createdThreads() {
    enableRawMode();
    setvbuf(stdout, NULL, _IONBF, 0);
    pthread_t waitKey;
    int rc = pthread_create(&waitKey, NULL, waitForKeys, NULL);

    pthread_t printChars;
    int rc2 = pthread_create(&printChars, NULL, printDataToOutput, NULL); 

    pthread_t timerFunc;
    int rc3 = pthread_create(&timerFunc, NULL, timerFunction, NULL); 

   if (rc != 0 || rc2 != 0 || rc3 != 0) {
        destroyThreads();
        printf("Nije mogao da napravi niti. \n");
        exit(3);
    } 

}



bool checkCanGoIntoInterrupt(Context* context) {
    if (context->psw & 0x8000) return false;
    if (!prekid) return false;
    return true;
}

bool isMaskedTimer(Context* context) {
    return (context->psw & 0x2000) != 0;
}

bool isMaskedTerminal(Context* context) {
    return (context->psw & 0x4000) != 0;
}

int main(int argc , char** argv) {
    if (argc < 2)  {
        std::cout << "Morate uneti fajl koji hocete da emulirate" << std::endl;
        return 1;
    }

    if (!endsWith(argv[1] , ".hex")) {
         std::cout << "Morate uneti fajl koji se zavrsava sa .hex" << std::endl;
        return 2;
    }


    std::fstream input_file;
    input_file.open(argv[1] , std::ios::in | std::ios::binary);
    if (!input_file) {
        std::cout << "Nije mogao da se otvori ulazni fajl!";
        return 3;
    }

    unsigned short size;
    unsigned short adress;
    unsigned short sp = 0xFEFF;
    while (!input_file.eof()) {
        input_file.read((char*)&adress, sizeof(unsigned short));
        input_file.read((char*)&size, sizeof(unsigned short));
        input_file.read(memory + adress, size);
        if (adress > 0) sp = std::min(sp , adress);
    //    std::cout << std::hex << adress << " " <<  size << std::endl;
    } 


    bool finished = false;
    bool needPomeraj = false;
    unsigned char instruction;
    unsigned char regs;
    unsigned char addremode;
    unsigned char addrMode;
    unsigned char pomoc;
    unsigned short pomeraj;

    Context* context = new Context(readData(0) , sp);


 //   printf("Pocinje\n");
    createdThreads();


 //   Context* context = new Context(0xfe8f , sp);

    try {
        while (!finished) {
            instruction = memory[context->pc++];
            switch(instruction) {
                case Ihalt:
                    finished = true;
                    break;

                case Iint:

                    regs = memory[context->pc++];
                    context->sp -=2;
                    writeDataEveryAdressType(reg_indirS, context , readDataFromReg(PCS , context) , SPS);
                    context->sp -=2;
                    writeDataEveryAdressType(reg_indirS, context , readDataFromReg(PSWS , context) , SPS);
                    pomeraj = (readDataFromReg(extractHeigherBits(regs) , context) % 8) * 2;
                    context->psw |= 0x8000;

                    writeDataIntoReg(PCS , readDataEveryAdressType(mem_dirS , context , 0 , pomeraj) , context);


                    break;

                case Iiret:


                    pomeraj = readDataEveryAdressType(reg_indirS , context , SPS);
                    context->sp += 2;
                    writeDataIntoReg(PSWS, pomeraj ,context);

                    pomeraj = readDataEveryAdressType(reg_indirS , context , SPS);
                    context->sp += 2;
                    writeDataIntoReg(PCS, pomeraj ,context);

                    break;

                case Icall:

                    regs = memory[context->pc++];
                    addremode = memory[context->pc++];

                    addrMode = extractLowerBits(addremode);
                    needPomeraj = doINeedToReadOpperand(addrMode);
                    if (needPomeraj) {
                        pomeraj = readData(context->pc);
                        context->pc += 2;
                    }

                    context->sp -=2;
                    writeDataEveryAdressType(reg_indirS, context , readDataFromReg(PCS , context) , SPS);

                    pomeraj = readDataEveryAdressType(addrMode, context ,extractLowerBits(regs) , pomeraj);
                    context->pc = pomeraj;

                    break;

                case Iret:

                    pomeraj = readDataEveryAdressType(reg_indirS , context , SPS);
                    context->sp += 2;
                    writeDataIntoReg(PCS, pomeraj ,context);

                    break;

                case Ijmp:

                    regs = memory[context->pc++];
                    addremode = memory[context->pc++];

                    addrMode = extractLowerBits(addremode);
                    needPomeraj = doINeedToReadOpperand(addrMode);
                    if (needPomeraj) {
                        pomeraj = readData(context->pc);
                        context->pc += 2;
                    }

                    pomeraj = readDataEveryAdressType(addrMode, context ,extractLowerBits(regs) , pomeraj);
                    context->pc = pomeraj;

                    break;

                case Ijeq:

                    regs = memory[context->pc++];
                    addremode = memory[context->pc++];

                    if (getZeroFlag(context)) {
                        addrMode = extractLowerBits(addremode);
                        needPomeraj = doINeedToReadOpperand(addrMode);
                        if (needPomeraj) {
                            pomeraj = readData(context->pc);
                            context->pc += 2;
                        }



                        pomeraj = readDataEveryAdressType(addrMode, context ,extractLowerBits(regs) , pomeraj);
                        context->pc = pomeraj;
                    }

                    break;

                case Ijne:

                    regs = memory[context->pc++];
                    addremode = memory[context->pc++];

                    if (!getZeroFlag(context)) {
                        addrMode = extractLowerBits(addremode);
                        needPomeraj = doINeedToReadOpperand(addrMode);
                        if (needPomeraj) {
                            pomeraj = readData(context->pc);
                            context->pc += 2;
                        }



                        pomeraj = readDataEveryAdressType(addrMode, context ,extractLowerBits(regs) , pomeraj);
                        context->pc = pomeraj;
                    }
                    

                    break;

                case Ijgt:

                    regs = memory[context->pc++];
                    addremode = memory[context->pc++];

                    if (!getZeroFlag(context) && !(getCarryFlag(context) && getOverflowFlag(context))) {
                        addrMode = extractLowerBits(addremode);
                        needPomeraj = doINeedToReadOpperand(addrMode);
                        if (needPomeraj) {
                            pomeraj = readData(context->pc);
                            context->pc += 2;
                        }



                        pomeraj = readDataEveryAdressType(addrMode, context ,extractLowerBits(regs) , pomeraj);
                        context->pc = pomeraj;
                    }

                    break;

                case Ixchg:

                    regs = memory[context->pc++];
                    pomeraj = readDataFromReg(extractHeigherBits(regs), context);
                    writeDataIntoReg(extractHeigherBits(regs) , readDataFromReg(extractLowerBits(regs), context), context);
                    writeDataIntoReg(extractLowerBits(regs) , pomeraj, context);

                    break;

                case Iadd:
                    regs = memory[context->pc++];
                    pomeraj = readDataFromReg(extractHeigherBits(regs) , context) + readDataFromReg(extractLowerBits(regs) , context);
                    writeDataIntoReg(extractHeigherBits(regs) , pomeraj, context);

                    break;

                case Isub:

                    regs = memory[context->pc++];
                    pomeraj = readDataFromReg(extractHeigherBits(regs) , context) - readDataFromReg(extractLowerBits(regs) , context);
                    writeDataIntoReg(extractHeigherBits(regs) , pomeraj, context);

                    break;

                case Imul:

                    regs = memory[context->pc++];
                    pomeraj = readDataFromReg(extractHeigherBits(regs) , context) * readDataFromReg(extractLowerBits(regs) , context);
                    writeDataIntoReg(extractHeigherBits(regs) , pomeraj, context);

                    break;

                case Idiv:

                    regs = memory[context->pc++];
                    pomeraj = readDataFromReg(extractHeigherBits(regs) , context) / readDataFromReg(extractLowerBits(regs) , context);
                    writeDataIntoReg(extractHeigherBits(regs) , pomeraj, context);

                    break;

                case Icmp:

                    regs = memory[context->pc++];
                    pomeraj = readDataFromReg(extractHeigherBits(regs) , context) - readDataFromReg(extractLowerBits(regs) , context);
                    setFlags(readDataFromReg(extractHeigherBits(regs) , context) , readDataFromReg(extractLowerBits(regs) , context) , pomeraj, context , instruction);
                    

                    break;

                case Inot:

                    regs = memory[context->pc++];
                    pomeraj = ~readDataFromReg(extractHeigherBits(regs) , context);
                    writeDataIntoReg(extractHeigherBits(regs) , pomeraj, context); 

                    break;

                case Iand:

                    regs = memory[context->pc++];
                    pomeraj = readDataFromReg(extractHeigherBits(regs) , context) & readDataFromReg(extractLowerBits(regs) , context);
                    writeDataIntoReg(extractHeigherBits(regs) , pomeraj, context);

                    break;

                case Ior:

                    regs = memory[context->pc++];
                    pomeraj = readDataFromReg(extractHeigherBits(regs) , context) | readDataFromReg(extractLowerBits(regs) , context);
                    writeDataIntoReg(extractHeigherBits(regs) , pomeraj, context);

                    break;

                case Ixor:

                    regs = memory[context->pc++];
                    pomeraj = readDataFromReg(extractHeigherBits(regs) , context) ^ readDataFromReg(extractLowerBits(regs) , context);
                    writeDataIntoReg(extractHeigherBits(regs) , pomeraj, context);

                    break;

                case Itest:

                    regs = memory[context->pc++];
                    pomeraj = readDataFromReg(extractHeigherBits(regs) , context) & readDataFromReg(extractLowerBits(regs) , context);
                    setFlags(readDataFromReg(extractHeigherBits(regs) , context) , readDataFromReg(extractLowerBits(regs) , context) , pomeraj, context, instruction);

                    break;

                case Ishl:

                    regs = memory[context->pc++];
                    pomeraj = readDataFromReg(extractHeigherBits(regs) , context) << readDataFromReg(extractLowerBits(regs) , context);
                    setFlags(readDataFromReg(extractHeigherBits(regs) , context) , readDataFromReg(extractLowerBits(regs) , context) , pomeraj, context, instruction);
                    writeDataIntoReg(extractHeigherBits(regs) , pomeraj, context);

                    break;

                case Ishr:

                    regs = memory[context->pc++];
                    pomeraj = readDataFromReg(extractHeigherBits(regs) , context) >> readDataFromReg(extractLowerBits(regs) , context);
                    setFlags(readDataFromReg(extractHeigherBits(regs) , context) , readDataFromReg(extractLowerBits(regs) , context) , pomeraj, context, instruction);
                    writeDataIntoReg(extractHeigherBits(regs) , pomeraj, context);

                    break;

                case Ildr:
                    regs = memory[context->pc++];
                    addremode = memory[context->pc++];
                    addrMode = extractLowerBits(addremode);
                    needPomeraj = doINeedToReadOpperand(addrMode);
                    if (needPomeraj) {
                        pomeraj = readData(context->pc);
                        context->pc += 2;
                    }

                    if (addrMode == reg_indirS || addrMode == reg_indir_displacmentS) {
                        pomoc = extractHeigherBits(addremode);

                        if (pomoc == pre_subS) {
                            pomoc = extractLowerBits(regs);
                            writeDataIntoReg(pomoc , readDataFromReg(pomoc , context) - 2 , context);

                        }
                        else if (pomoc == pre_sumS) {
                            pomoc = extractLowerBits(regs);
                            writeDataIntoReg(pomoc , readDataFromReg(pomoc , context) + 2 , context);
                        }
                    }

                    pomeraj = readDataEveryAdressType(addrMode, context ,extractLowerBits(regs) , pomeraj);
                    writeDataIntoReg(extractHeigherBits(regs), pomeraj , context);

                    

                    if (addrMode == reg_indirS || addrMode == reg_indir_displacmentS) {
                        pomoc = extractHeigherBits(addremode);

                        if (pomoc == post_subS) {
                            pomoc = extractLowerBits(regs);
                            writeDataIntoReg(pomoc , readDataFromReg(pomoc , context) - 2 , context);

                        }
                        else if (pomoc == post_sumS) {
                            pomoc = extractLowerBits(regs);
                            writeDataIntoReg(pomoc , readDataFromReg(pomoc , context) + 2 , context);
                        }
                    }

                    break;

                case Istr:

                    regs = memory[context->pc++];
                    addremode = memory[context->pc++];
                    addrMode = extractLowerBits(addremode);
                    needPomeraj = doINeedToReadOpperand(addrMode);
                    if (needPomeraj) {
                        pomeraj = readData(context->pc);
                        context->pc += 2;
                    }

                    if (addrMode == reg_indirS || addrMode == reg_indir_displacmentS) {
                        pomoc = extractHeigherBits(addremode);

                        if (pomoc == pre_subS) {
                            pomoc = extractLowerBits(regs);
                            writeDataIntoReg(pomoc , readDataFromReg(pomoc , context) - 2 , context);

                        }
                        else if (pomoc == pre_sumS) {
                            pomoc = extractLowerBits(regs);
                            writeDataIntoReg(pomoc , readDataFromReg(pomoc , context) + 2 , context);
                        }
                    }

                    writeDataEveryAdressType(addrMode, context , readDataFromReg(extractHeigherBits(regs) , context) ,extractLowerBits(regs), pomeraj);

                     if (addrMode == reg_indirS || addrMode == reg_indir_displacmentS) {
                        pomoc = extractHeigherBits(addremode);

                        if (pomoc == post_subS) {
                            pomoc = extractLowerBits(regs);
                            writeDataIntoReg(pomoc , readDataFromReg(pomoc , context) - 2 , context);

                        }
                        else if (pomoc == post_sumS) {
                            pomoc = extractLowerBits(regs);
                            writeDataIntoReg(pomoc , readDataFromReg(pomoc , context) + 2 , context);
                        }
                    }


                    break;

            }

            if (checkCanGoIntoInterrupt(context)) {
                
                prekid = false;
                if (ulazUIvtp == interrupt_terminal && !isMaskedTerminal(context)) {
                    context->sp -=2;
                    writeDataEveryAdressType(reg_indirS, context , readDataFromReg(PCS , context) , SPS);
                    context->sp -=2;
                    writeDataEveryAdressType(reg_indirS, context , readDataFromReg(PSWS , context) , SPS);
                    context->psw |= 0x8000;

                    writeDataIntoReg(PCS , readDataEveryAdressType(mem_dirS , context , 0 , ulazUIvtp * 2) , context);

                }
                else if (ulazUIvtp == interrupt_tajmer && !isMaskedTimer(context)) {
                    context->sp -=2;
                    writeDataEveryAdressType(reg_indirS, context , readDataFromReg(PCS , context) , SPS);
                    context->sp -=2;
                    writeDataEveryAdressType(reg_indirS, context , readDataFromReg(PSWS , context) , SPS);
                    
                    context->psw |= 0x8000;

                    writeDataIntoReg(PCS , readDataEveryAdressType(mem_dirS , context , 0 , ulazUIvtp * 2) , context);
                }
            }
  

        } 
    }
    catch (NoSuchRegisterException& nsr) {
        std::cout << nsr.getMessage() << std::endl;
        destroyThreads();
        exit(1);
    }
    catch (NoSuchAddresTypeException& nsat) {
        std::cout << nsat.getMessage() << std::endl;
        destroyThreads();
        exit(2);
    }
    catch (NoFlagsForInstructionException& nff) {
        std::cout << nff.getMessage() << std::endl;
        destroyThreads();
        exit(4);
    }

    

    destroyThreads();

    delete context;
    return 0;

}