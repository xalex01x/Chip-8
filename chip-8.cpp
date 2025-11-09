#include<iostream>
#include<fstream>
#include<cstring>
#include<windows.h>
#include"macros.h"

extern void printDisplay();
using namespace std;

struct processor {
    u16 pc;
    u16 index;
    u8 timer;
    u8 sound;
    u8 r[16];   // registri da 0 a F -> v0 ... vF
};

processor cpu;
u8 ram[4096];
u8 stack[256];
u8 sp = 0;
u8 display[WIDTH * HEIGH];
u8 keyboard[16];
u16 opcode;

long long currentTs, expectedTs;; 

void printbyte(u8 byte, char m){
    u16 value = static_cast<u16>(byte);
    if(m == 'd')cout << dec <<  value;
    if(m == 'h')cout << hex <<  value;
}

void printCPU(char m) {
    if(m == 'd') cout << "pc:" <<dec << cpu.pc <<'\t'<< "ind:" << dec << cpu.index<<endl;
    if(m == 'h') cout << "pc:" <<hex << cpu.pc <<'\t'<< "ind:" << hex << cpu.index<<endl;
    for(u8 i = 0; i < 16; i++){
        printbyte(i,m);
        cout <<": ";
        printbyte(cpu.r[i],m);
        cout << "\t";
    }
    cout << endl;
}

void printRAM(u16 addr, u8 len, char m) {
    cout<< "RAM: ";
    for(u8 i = 0; i < len; i++) {
        printbyte(ram[addr + i], m);
        cout << '\t';
    }
    cout << endl;
}

void fontInit() {
    u8 tmp[5*16] = { 0xF0, 0x90, 0x90, 0x90, 0xF0, 
                     0x20, 0x60, 0x20, 0x20, 0x70,
                     0xF0, 0x10, 0xF0, 0x80, 0xF0,
                     0xF0, 0x10, 0xF0, 0x10, 0xF0, 
                     0x90, 0x90, 0xF0, 0x10, 0x10, 
                     0xF0, 0x80, 0xF0, 0x10, 0xF0, 
                     0xF0, 0x80, 0xF0, 0x90, 0xF0, 
                     0xF0, 0x10, 0x20, 0x40, 0x40, 
                     0xF0, 0x90, 0xF0, 0x90, 0xF0, 
                     0xF0, 0x90, 0xF0, 0x10, 0xF0, 
                     0xF0, 0x90, 0xF0, 0x90, 0x90, 
                     0xE0, 0x90, 0xE0, 0x90, 0xE0, 
                     0xF0, 0x80, 0x80, 0x80, 0xF0, 
                     0xE0, 0x90, 0x90, 0x90, 0xE0, 
                     0xF0, 0x80, 0xF0, 0x80, 0xF0, 
                     0xF0, 0x80, 0xF0, 0x80, 0x80 };
    memcpy(&ram[FONT_ADDR], tmp, 5*16);
}

u16 fetch(){
    cpu.pc+=2;
    cpu.pc &= 0x0fff;
    return ram[cpu.pc - 2] << 8 | ram[cpu.pc - 1];
}

void ROMRead(char * path){
    ifstream infile(path, ios::in | ios::binary);
    if (!infile.is_open()) {
        cout << "Errore nell'apertura della ROM" << endl;
        return;
    }
    int i = 0;
    while (!infile.eof()){
        ram[(PC_START_VALUE + i) & 0x0FFF] = infile.get();
        i++;
    }
}

void displayIntruction() {
    u8 x = cpu.r[SECOND_N] & 63;
    u8 y = cpu.r[THIRD_N] & 31;
    u8 N = FOURTH_N;
    u8 coll = 0;
    u16 i = y;
    while( i < y + N && i < HEIGH){
        u8 byte = ram[cpu.index + i - y];
        u8 mask = 0b10000000;
        u16 j = x;
        while( j < x + 8 && j < WIDTH) {
            if(byte & mask){
                if(display[i*WIDTH + j] == white) {
                    display[i*WIDTH + j] = black;
                    coll = 1;
                }
                else display[i*WIDTH + j] = white;
            }
            mask = mask >> 1;
            j++;
        }
        i++;
    }
    cpu.r[0xf] = coll;
    printDisplay();
}

void subtraction(u8 a, u8 b){
    u8 v1 = cpu.r[a];
    u8 v2 = cpu.r[b];
    cpu.r[SECOND_N] = v1 - v2;
    if (v1 >= v2) cpu.r[0xf] = 1;
    else cpu.r[0xf] = 0;
}

void keyboardListener() {
                   //'0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  'a',  'b',  'c',  'd',  'e',  'f'
    int toCheck[] = {0x58, 0x31, 0x32, 0x33, 0x51, 0x57, 0x45, 0x41, 0x53, 0x44, 0x5A, 0x43, 0x34, 0x52, 0x46, 0x56};
    for(u8 i = 0; i < 16; i++){
        if(GetKeyState(toCheck[i]) & PRESSED) keyboard[i] = 1;
            else keyboard[i] = 0;
    }
}

void chipInit(char * rom){
    cout << "provando a caricare " << rom << endl;
    ROMRead(rom);
    fontInit();
    memset(&cpu, 0, sizeof(processor));
    memset(keyboard, 0, 16*sizeof(u8));
    memset(display, black, WIDTH * HEIGH);
    cpu.pc = PC_START_VALUE;
    cout<< "Chip-8 initialized" << endl;
}

void chipCycle() {
        opcode = fetch();
        //cout << hex << opcode << endl;
        u8 op = FIRST_N;
        keyboardListener();
        if(cpu.timer) cpu.timer--;
        if(cpu.sound) cpu.sound--;
        switch(op){
            case 0x0: {
                switch(opcode & 0x0fff) {
                    case 0x00E0: // CLEAR_SCREEN verificata
                        for(int i = 0; i < WIDTH * HEIGH; i++) display[i] = black;
                    break;
                    case 0x00EE: {  // RET verificata
                        sp -= 2;
                        void * ptr = static_cast<void*>(&stack[sp]);
                        memcpy(&cpu.pc, ptr, sizeof(u16));
                    }
                    break;
                    default: cout << "zeros default reached " << hex << opcode << endl;
                }
            }
            break;
            case 0x1:   // JMP verificata
                cpu.pc = 0x0fff & opcode;
            break;
            case 0x2: { // CALL, verificata
                stack[sp++] = cpu.pc & 0x00ff;
                stack[sp++] = cpu.pc >> 8;
                cpu.pc = opcode & 0x0fff;
            }
            break;
            case 0x3:   // JE verificata
                if(cpu.r[SECOND_N] == (opcode & 0x00ff)) cpu.pc += 2;
            break;
            case 0x4:   // JNE verificata
                if(cpu.r[SECOND_N] != (opcode & 0x00ff)) cpu.pc += 2;
            break;
            case 0x5:   // JER verificata
                if(cpu.r[SECOND_N] == cpu.r[THIRD_N]) cpu.pc += 2;
            break;
            case 0x6:   // SET verificata
                cpu.r[SECOND_N] = (opcode & 0x00ff);
            break;
            case 0x7:   // ADD
                cpu.r[SECOND_N] += (opcode & 0x00ff);
            break;
            case 0x8: { // ARITHMETIC
                switch (FOURTH_N) {
                    case 0x0:   // EQ verificata
                        cpu.r[SECOND_N] = cpu.r[THIRD_N];
                    break;
                    case 0x1:   // OR verificata
                        cpu.r[SECOND_N] |= cpu.r[THIRD_N];
                    break;
                    case 0x2:   // AND verificata
                        cpu.r[SECOND_N] &= cpu.r[THIRD_N];
                    break;
                    case 0x3:   // XOR verificata
                        cpu.r[SECOND_N] ^= cpu.r[THIRD_N];
                    break;
                    case 0x4: { // ADD verificata
                        u16 sum = cpu.r[SECOND_N] + cpu.r[THIRD_N];
                        cpu.r[SECOND_N] = sum;
                        if(sum > 255) cpu.r[0xf] = 1;
                        else cpu.r[0xf] = 0;
                    }
                    break;
                    case 0x5:   // SUB X Y verificata
                        subtraction(SECOND_N, THIRD_N);
                    break;
                    case 0x6: { // SHR verificata
                        u16 value;
                        if(BEHAVIOUR) value = cpu.r[THIRD_N];
                        else value = cpu.r[SECOND_N];
                        cpu.r[SECOND_N] = value >> 1;
                        cpu.r[0xf] = value & 0x01;
                    }
                    break;
                    case 0x7:   // SUB Y X verificata
                        subtraction(THIRD_N, SECOND_N);
                    break;
                    case 0xE: { // SHL verificata
                        u16 value;
                        if(BEHAVIOUR) value = cpu.r[THIRD_N];
                        else value = cpu.r[SECOND_N];
                        cpu.r[SECOND_N] = value << 1;
                        cpu.r[0xf] = (value & 0x80) >> 7;
                    }
                    break;
                    default: cout << "eights default reached " << hex << opcode << endl;
                }
            }
            break;
            case 0x9:   // JNER verificata
                if(cpu.r[SECOND_N] != cpu.r[THIRD_N]) cpu.pc += 2;
            break;
            case 0xA:   // SET_INDEX verificata
                cpu.index = (opcode & 0x0fff);
            break;
            case 0xB: { // JMP_OFF verificata
                if(BEHAVIOUR) {
                    cpu.pc = (opcode & 0x0fff) + cpu.r[0x0];
                } else {
                    cpu.pc = (opcode & 0x0fff) + cpu.r[THIRD_N];
                }
            }
            break;
            case 0xC:   // RAND verificata
                cpu.r[SECOND_N] = (rand() & opcode & 0xff);
            break;
            case 0xD: { // PRINT verificata
                displayIntruction();
            }
            break;
            case 0xE: { // SKIP verificate
                switch (FOURTH_N) {
                    case 0x1:
                        if(!keyboard[cpu.r[SECOND_N]]) cpu.pc += 2;
                    break;
                    case 0xE:
                        if(keyboard[cpu.r[SECOND_N]]) cpu.pc += 2;
                    break;
                    default: cout << "e's default reached " << hex << opcode << endl;
                }
            }
            break;
            case 0xF: {
                switch (FOURTH_N) {
                    case 0x3:   // DECIMAL verificata
                        ram[cpu.index] = cpu.r[SECOND_N] / 100;
                        ram[(cpu.index + 1) & 0x0fff] = (cpu.r[SECOND_N] / 10) % 10;
                        ram[(cpu.index + 2) & 0x0fff] = cpu.r[SECOND_N] % 10;
                    break;
                    case 0x5: {
                        switch (THIRD_N) {
                            case 0x1:   
                                cpu.timer = cpu.r[SECOND_N];
                            break;
                            case 0x5: {
                                u16 i;
                                for(i = 0; i <= SECOND_N; i++) ram[cpu.index + i] = cpu.r[i & 0x000f];
                                if(BEHAVIOUR) cpu.index += ((SECOND_N + i + 1) & 0x0fff);
                            }
                            break;
                            case 0x6: {
                                u16 i;
                                for(i = 0; i <= SECOND_N; i++) cpu.r[i] = ram[cpu.index + i];
                                if(BEHAVIOUR) cpu.index += ((SECOND_N + 1 + i) & 0x0fff);
                            }
                            break;
                            default: cout << "f_57's default reached " << hex << opcode << endl;
                        }
                        break;
                    }
                    case 0x7:
                        cpu.r[SECOND_N] = cpu.timer;
                    break;
                    break;
                    case 0x8:
                        cpu.sound = cpu.r[SECOND_N];
                    break;
                    case 0x9:
                        cpu.index = FONT_ADDR + (cpu.r[SECOND_N] & 0x0f)*5;
                    break;
                    case 0xA: {
                        u8 found = 0;
                        for(u8 i = 0; i < 16; i++)
                            if(keyboard[i]){
                                cpu.r[SECOND_N] = i;
                                found = 1;
                                break;
                            }
                        if(!found) cpu.pc -=2;
                    }
                    break;
                    case 0xE: {
                        cpu.index += cpu.r[SECOND_N];
                        cpu.index &= 0xfff;
                        if(BEHAVIOUR){
                            if(cpu.index > 0xfff) cpu.r[0xf] = 1;
                                else cpu.r[0xf] = 0;
                        }
                    }
                    break;
                    default: cout << "f's default reached " << hex << opcode << endl;
                }
            }
            break;
            default: cout << "default reached " << hex << opcode << endl;
        }
}