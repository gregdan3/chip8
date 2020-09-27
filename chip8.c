#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

/* BIG thanks to Laurence Muller with his article
 * http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
 * That's where I got interested and started
 */

/* 0x000-0x1FF is chip8 interpreter
 * 0x050-0x0A0 is 4x5 font set, a builtin
 * 0x200-0xFFF is p-ROM and work RAM
 */

void setup_input()
{
    printf("Set input!");
    // TODO
}

void setup_graphics()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_Window* win = SDL_CreateWindow("chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE, 0);
    printf("Set graphics!");
}

void draw_graphics(struct CHIP8* self)
{
    int x = 0;
    int y = 0;

    for (int i = 0; i < (64 * 32); i++) {
        if (self->gfx[i] == 1) { // GFX is from a region of RAM...
        }

    }

}

void initialize(struct CHIP8* self)
{
    // Initial state
    self->pc = 0x200;
    self->opcode = 0;
    self->I = 0;
    self->sp = 0;

    // Clear display
    memset(self->gfx, 0x0, 64 * 32 * sizeof(char));

    // Clear stack
    memset(self->stack, 0x0, 16 * sizeof(short));

    // Clear registers
    memset(self->V, 0x0, 16 * sizeof(char));

    // Clear memory
    memset(self->memory, 0x0, 4096 * sizeof(char));

    // Prepare font
    // TODO: what if hires font?
    for (int i = 0; i < 80; i++) {
        self->memory[i] = chip8_fontset[i];
    }
}

void load_game(struct CHIP8* self, char* game)
{
    int i = 0;
    char c;

    FILE* file = fopen(game, "rb");
    while (!feof(file)) {
        c = fgetc(file);
        self->memory[i + 0x200] = c;
        i += 1;
        // TODO: if file exceeds 4k - 0x200, tell user and fail
    }
}

void emulate_cycle(struct CHIP8* self)
{

    // fetch
    self->opcode = self->memory[self->pc] << 8 | self->memory[self->pc + 1];
    printf("0x%X\n", self->opcode);
    printf("PC%hu\n", self->pc);
    // collect first byte and second byte;
    // set first byte over by 8, filling in new byte with 0
    // merge first byte and second byte with boolean or
    // ex: ((0x2B << 8) | 0xAF) = 0x2BAF


    // pre-emptively increment program counter
    self->pc += 2;
    // opcodes that assign it will overwrite this
    // opcodes that skip an instruction have their own += 2


    // decode
    switch (self->opcode & 0xF000) {

    case 0x0000: // two opcodes cannot be told apart by above
        switch (self->opcode & 0x000F) {
        case 0x0000: // display clear
            memset(self->gfx, 0x0, 64 * 32 * sizeof(char));
            // TODO: set draw byte?
            break;

        case 0x000E: // return from subroutine
            // assign program counter from the top stack value
            self->pc = self->stack[self->sp];
            // decrement stack pointer
            self->sp--;
            // TODO: does the chip8 also clear that value on the stack?
            // or is it assumed to be junk and ignored?
            break;

        default:
            printf("Unknown opcode [0x0000]: 0x%X\n", self->opcode);
            break;
        }

    case 0x1000: // set the program counter to 0x0FFF of opcode
        self->pc = self->opcode & 0x0FFF;
        break;

    case 0x2000: // call subroutine
        // put program counter on the stack
        self->stack[self->sp] = self->pc;
        // increment stack pointer
        self->sp++;
        // move stackpointer to specified location
        self->pc = self->opcode & 0x0FFF;
        break;

    case 0x3000: // skip next instruction if 0r00 == 00xx
        if ((self->V[(self->opcode & 0x0F00) >> 8])
            == (self->opcode & 0x00FF)) {
            self->pc += 2;
        }
        break;

    case 0x4000: // skip next instruction if 0r00 != 00xx
        if ((self->V[(self->opcode & 0x0F00) >> 8])
            != (self->opcode & 0x00FF)) {
            self->pc += 2;
        }
        break;

    case 0x5000: // skip next instruction if 0r00 == 00r0
        if (self->V[(self->opcode & 0x0F00) >> 8]
            == self->V[(self->opcode & 0x00F0) >> 4]) {
            self->pc += 2;
        }
        break;

    case 0x6000: // assign register 0r00 to 00xx
        self->V[(self->opcode & 0x0F00) >> 8] = self->opcode & 0x00FF;
        break;

    case 0x7000: // sum register 0r00 with 00xx and assign to 0r00
        self->V[(self->opcode & 0x0F00) >> 8] += (self->opcode & 0x00FF);
        break;

    case 0x8000:
        switch (self->opcode & 0x000F) {
        case 0x0000: // assign register 0r00 to value of register 00r0
            self->V[(self->opcode & 0x0F00) >> 8] = self->V[(self->opcode & 0x00F0) >> 4];
            break;
        case 0x0001: // assign register 0r00 to 0r00 bitwise OR 00r0
            self->V[(self->opcode & 0x0F00) >> 8] |= self->V[(self->opcode & 0x00F0) >> 4];
            break;
        case 0x0002: // assign register 0r00 to 0r00 bitwise AND 00r0
            self->V[(self->opcode & 0x0F00) >> 8] &= self->V[(self->opcode & 0x00F0) >> 4];
            break;
        case 0x0003: // assign register 0r00 to 0r00 bitwise XOR 00r0
            self->V[(self->opcode & 0x0F00) >> 8] ^= self->V[(self->opcode & 0x00F0) >> 4];
            break;
        case 0x0004: // sum 0r00 and 00r0, assign to 0r00, set carry if overflow
            printf("Partially implemented opcode 8rr7!\n");
            break;
        case 0x0005: // set VF if 0r00 > 00r0, then 0r00 -= 00r0
            // shortcut: comparisons return 1 for true and 0 for false
            self->V[0xF] = (self->V[self->opcode & 0x0F00 >> 8] > (self->V[self->opcode & 0x00F0 >> 4]));
            self->V[(self->opcode & 0x0F00) >> 8] -= self->V[(self->opcode & 0x00F0) >> 4];
            break;
        case 0x0006: // set VF if least-sig bit of 0r00 is 1, else 0, then 0r00 /= 2
            // shortcut: set VF to least significant bit directly
            self->V[0xF] = (self->V[self->opcode & 0x0F00 >> 8] & 0x01);
            self->V[(self->opcode & 0x0F00) >> 8] /= 2;
            break;
        case 0x0007: // set VF if 0r00 > 00r0, then 0r00 = 00r0 - 0r00
            // same trick as 8rr5
            self->V[0xF] = (self->V[self->opcode & 0x00F0 >> 4] > (self->V[self->opcode & 0x0F00 >> 8]));
            self->V[(self->opcode & 0x0F00) >> 8] = self->V[(self->opcode & 0x00F0) >> 4] - self->V[(self->opcode & 0x0F00) >> 8];
            break;
        case 0x000E: // set VF if most-sig bit of 0r00 is 1, else 0, then 0r00 *= 2
            self->V[0xF] = (self->V[self->opcode & 0x0F00 >> 8] & 0x10);
            self->V[(self->opcode & 0x0F00) >> 8] *= 2;
            break;
        default:
            printf("Unknown opcode [0x8000]: 0x%X\n", self->opcode);
            break;
        }
        break;

    case 0x9000: // skip next instruction if 0r00 != 00r0
        if (self->V[(self->opcode & 0x0F00) >> 8]
            != self->V[(self->opcode & 0x00F0) >> 4]) {
            self->pc += 2;
        }
        break;

    case 0xA000: // assign I register to 0nnn
        self->I = self->opcode & 0x0FFF;
        break;

    case 0xB000: // jump to 0nnn + V0 register's value
        self->pc = (self->opcode & 0x0FFF) + self->V[0];
        break;

    case 0xC000: // put a random value (0-255 & 00xx) into 0r00
        self->V[(self->opcode & 0x0F00)]
            = (rand() % 256) & (self->opcode & 0x00FF);
        break;

    case 0xD000:
        // TODO: display sprite??
        break;

    case 0xE000:
        switch (self->opcode & 0x00FF) {
        case 0x009E: // skip next instruction if key in 0r00 is pressed
            if (self->key[self->V[self->opcode & 0x0F00 >> 8]] != 0) {
                self->pc += 2;
            }
            break;
        case 0x00A1: // skip next instruction if key in V0r00 is not pressed
            if (self->key[self->V[self->opcode & 0x0F00 >> 8]] == 0) {
                self->pc += 2;
            }
            break;
        default:
            printf("Unknown opcode [0xE000]: 0x%X\n", self->opcode);
            break;
        }

    case 0xF000:
        switch (self->opcode & 0x00FF) {
        case 0x0007: // store value of delay timer in 0r00
            self->V[(self->opcode & 0x0F00) >> 8] = self->delay_timer;
            break;
        case 0x000A: // wait for keypress, store in 0r00
            //TODO: implement
            break;
        case 0x0015: // set delay timer to value of 0r00
            self->delay_timer = self->V[(self->opcode & 0x0F00) >> 8];
            break;
        case 0x0018: // set sound timer to value of 0r00
            self->sound_timer = self->V[(self->opcode & 0x0F00) >> 8];
            break;
        case 0x001E: // I register is assigned sum of I and 0r00
            self->I += self->V[(self->opcode & 0x0F00) >> 8];
            break;
        case 0x0029: // I register is assigned address of char in register 0r00
            self->I = self->V[(self->opcode & 0x0F00 >> 8)] * 5; // 0c00 is a char
            break;
        case 0x0033:
            break;
        case 0x0055: // copy registers 0 through 0r00 into memory
                     // starts from address in I
            for (int i = 0; i <= ((self->opcode & 0x0F00) >> 8); i++) {
                self->memory[self->I + i] = self->V[i];
            }
            break;
        case 0x0065: // copy memory starting from address I into registers
                     // up to register 0r00
            for (int i = 0; i <= ((self->opcode & 0x0F00) >> 8); i++) {
                self->V[i] = self->memory[self->I + i];
            }
            break;
        }
        break;
    }

    // update timers
    if (self->delay_timer > 0) {
        self->delay_timer--;
    }

    if (self->sound_timer > 0) {
        if (self->sound_timer == 1) {
            // TODO: beep
            printf("BEEP\n");
        }
        self->sound_timer--;
    }
}

void set_keys(struct CHIP8* self)
{
    getchar(); // TODO: bad step function
    // TODO: listen for keyboard input
}

int main(int argc, char** argv)
{
    // TODO: argp or getopt
    CHIP8 chip = {};
    chip.draw_graphics = draw_graphics;
    chip.initialize = initialize;
    chip.load_game = load_game;
    chip.emulate_cycle = emulate_cycle;
    chip.set_keys = set_keys;

    setup_graphics();
    setup_input();
    chip.initialize(&chip);
    chip.load_game(&chip, "TODO");

    while (1) {
        chip.emulate_cycle(&chip);

        if (chip.draw_flag) {
            draw_graphics(&chip);
        }
        chip.set_keys(&chip);
    };
}
