#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    // TODO
}

void setup_graphics()
{
    // TODO
}

void draw_graphics()
{
    // TODO
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
    }
}

void emulate_cycle(struct CHIP8* self)
{

    // fetch
    self->opcode = self->memory[self->pc] << 8 | self->memory[self->pc + 1];
    // collect first byte and second byte;
    // set first byte over by 8, filling in new byte with 0
    // merge first byte and second byte with boolean or
    // ex: ((0x2B << 8) | 0xAF) = 0x2BAF

    // decode
    switch (self->opcode & 0xF000) {

    case 0x0000: // two opcodes cannot be told apart by above
        switch (self->opcode & 0x000F) {
        case 0x0000: // display clear
            memset(self->gfx, 0x0, 64 * 32 * sizeof(char));
            self->pc++;
            break;

        case 0x000E:
            // TODO
            self->pc++;
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
            self->pc += 4;
        } else {
            self->pc += 2;
        }
        break;

    case 0x4000: // skip next instruction if Vx != kk
        if ((self->V[(self->opcode & 0x0F00) >> 8])
            != (self->opcode & 0x00FF)) {
            self->pc += 4;
        } else {
            self->pc += 2;
        }
        self->pc += 2;
        break;

    case 0x5000: // skip next instruction if Vx == Vy
        if (self->V[(self->opcode & 0x0F00) >> 8]
            == self->V[(self->opcode & 0x00F0) >> 4]) {
            self->pc += 4;
        } else {
            self->pc += 2;
        }
        self->pc += 2;
        break;

    case 0x6000: // set register 0r00 to be value 00xx
        self->V[(self->opcode & 0x0F00)] = self->opcode & 0x00FF;
        self->pc += 2;
        break;

    case 0x7000: // add value 00xx to value in register 0r00 and assign
        self->V[(self->opcode & 0x0F00) >> 8] += (self->opcode & 0x00FF);
        self->pc += 2;
        break;

    case 0x8000:
        // TODO
        self->pc += 2;
        break;

    case 0x9000:
        if (self->V[(self->opcode & 0x0F00) >> 8]
            != self->V[(self->opcode & 0x00F0) >> 4]) {
            self->pc += 4;
        } else {
            self->pc += 2;
        }
        self->pc += 2;
        break;

    case 0xA000:
        // TODO
        self->pc += 2;
        break;

    case 0xB000:
        // TODO
        self->pc += 2;
        break;

    case 0xC000:
        // TODO
        self->pc += 2;
        break;

    case 0xD000:
        // TODO
        self->pc += 2;
        break;

    case 0xE000:
        switch (self->opcode & 0x00FF) {
        case 0x009E: // skip next instruction if key in VX is pressed
                     // X is 0x0X00 in opcode
            if (self->key[self->V[self->opcode & 0x0F00 >> 8]] != 0) {
                self->pc += 4;
            } else {
                self->pc += 2;
            }
        }
        break;

    case 0xF000:
        // TODO
        self->pc += 2;
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
    // TODO
}

int main(int argc, char** argv)
{
    struct CHIP8 chip;

    setup_graphics();
    setup_input();
    chip.initialize(&chip);
    chip.load_game(&chip, "TODO");

    while (1) {
        chip.emulate_cycle(&chip);

        if (chip.draw_flag) {
            draw_graphics();
        }
        chip.set_keys(&chip);
    };
}
