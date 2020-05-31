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
    unsigned char memory[4096]; // 4K
    unsigned char V[16];        // 15 general purpose registers and carry flag
    unsigned short I;           // index register
    unsigned short pc;          // program counter
    unsigned char gfx[64 * 32]; // display
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

unsigned char chip8_fontset[80] = {
    /* this is a clever trick
     * the bits are used to print the characters
     * 4 bits wide (notice all end in 0) and 5 high
     */
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
