#include "constants.h"
struct CHIP8;

void setup_input();
void setup_graphics();
void draw_graphics(struct CHIP8* self);
void initialize(struct CHIP8* self);
void load_game(struct CHIP8* self, char* game);
void emulate_cycle(struct CHIP8* self);
void set_keys(struct CHIP8* self);

typedef struct CHIP8 {
    unsigned short opcode;      // 2 bytes
    unsigned char memory[MEMORY_SIZE];
    unsigned char V[16];        // 15 general purpose registers and carry flag
    unsigned short I;           // index register
    unsigned short pc;          // program counter
    unsigned char gfx[SCREEN_WIDTH * SCREEN_HEIGHT]; // display
    unsigned char delay_timer;  // 60Hz timer
    unsigned char sound_timer;  // 60Hz timer
    unsigned short stack[16];   //
    unsigned short sp;          // stack pointer
    unsigned char key[16];      // 0x0-0xF for hex keypad
    unsigned char draw_flag;    // do graphics?

    // attaching funcs to struct
    void (*draw_graphics)(struct CHIP8*);
    void (*initialize)(struct CHIP8*);
    void (*load_game)(struct CHIP8*, char*);
    void (*emulate_cycle)(struct CHIP8*);
    void (*set_keys)(struct CHIP8*);

} CHIP8;

