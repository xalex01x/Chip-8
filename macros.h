#define WIDTH 64
#define HEIGH 32
#define white 177
#define black 178
#define CYCLE_LEN 16
#define PC_START_VALUE 0x200
#define PRESSED 0x8000
#define FONT_ADDR 0x0050
#define CELL_SIDE 16
#define FPS 240
#define RR 1.0f / FPS

// leggono da opcode i nibble da sinistra a destra
#define FIRST_N     static_cast<u8>(opcode >> 12)
#define SECOND_N    static_cast<u8>((opcode & 0x0f00) >> 8)
#define THIRD_N     static_cast<u8>((opcode & 0x00f0) >> 4)
#define FOURTH_N    static_cast<u8>((opcode & 0x000f))

// metti a "0" per avere il funzionamento SUPER-CHIP, un altro numero per il COSMAC VIP
#define BEHAVIOUR 0x0 

typedef unsigned short u16;
typedef unsigned char u8;