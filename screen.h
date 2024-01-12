// Copyright (c) 2024 Hubert Gruniaux
// This file is part of SparseMemory which is released under the MIT license.
// See file LICENSE.txt for full license details.

#ifndef CPULM_SCREEN_H
#define CPULM_SCREEN_H

#include "memory.h"

// Try to keep the two following values as power of twos. If it is the case,
// all divisions and modulos can be simplified.
/** The width of the screen in characters. */
#define SCREEN_WIDTH 64 // count of columns
/** The width of the screen in characters. */
#define SCREEN_HEIGHT 16 // count of lines
/** The total size of the screen, in characters. */
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

/** Initializes the screen. */
void screen_init();
/** Puts the given @a styled_char in the screen at the given @a x, @a y
 * coordinates.
 *
 * The format of @a styled_char is specified in the CPUlm assembler
 * documentation. */
void screen_put_character(addr_t x, addr_t y, word_t styled_char);
/** If @a addr is inside the screen memory bounds (starting at @a base_addr)
 * and @a new_word is the new memory's cell value written at @a addr, then
 * update the screen with the given character.
 *
 * In practice, the screen access is done via a memory mapping. Some
 * region of memory is "attached" to screen, each write to it update
 * the screen. This function implements the link between this mapped
 * memory region and the screen. */
void screen_ram_write(addr_t base_addr, addr_t addr, word_t new_word);

#endif // !CPULM_SCREEN_H
