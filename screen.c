// Copyright (c) 2024 Hubert Gruniaux
// This file is part of SparseMemory which is released under the MIT license.
// See file LICENSE.txt for full license details.

#include "screen.h"

#include <assert.h>
#include <stdio.h>

#define SCI "\x1b["

#ifndef DISABLE_SCREEN_STYLING
enum style_flag_t {
    STYLE_BOLD = 1 << 18,
    STYLE_FAINT = 1 << 19,
    STYLE_ITALIC = 1 << 20,
    STYLE_UNDERLINE = 1 << 21,
    STYLE_BLINKING = 1 << 22,
    STYLE_HIDE = 1 << 23,
    STYLE_CROSSED = 1 << 24,
    STYLE_OVERLINE = 1 << 25,
};
#endif // !DISABLE_SCREEN_STYLING

void screen_init() {
    printf(SCI "?25l"); // hide cursor
    printf(SCI "2J"); // clear screen
    printf(SCI "17;1H"); // initial cursor position
    fflush(stdout);
}

void screen_init_with_ram_mapping(ram_t* ram) {
    assert(ram != NULL);
    screen_init();
    // End points are included so
    // The interval is [SCREEN_BASE_ADDR; SCREEN_BASE_ADDR + SCREEN_SIZE - 1]
    ram_install_write_listener(ram, SCREEN_BASE_ADDR,
        SCREEN_BASE_ADDR + SCREEN_SIZE - 1, &screen_ram_write);
}

void screen_terminate() {
    printf(SCI "?25h"); // show cursor
    fflush(stdout);
}

void screen_put_character(addr_t x, addr_t y, word_t styled_char) {
    assert(x < SCREEN_WIDTH);
    assert(y < SCREEN_HEIGHT);
    // Save current cursor position
    printf(SCI "s");
    // Cursor indices are 1-based, thus the +1.
    // The first argument is the row number, the second is the column number.
    printf(SCI "%u;%uH", y + 1, x + 1);

    const char ch = (char)(styled_char & 0x7f); // ASCII character, 7-bits
                                                // 1-bit which is always set to 0

#ifndef DISABLE_SCREEN_STYLING
    // Check if there is any styling, and if yes, then handle it.
    // Because styling handling is heavy, we try to skip it if there is no style
    // specified.
    if (styled_char & (~0x7f)) {
        word_t fg_color = ((styled_char >> 8) & 0x1f); // 5-bits
        word_t bg_color = ((styled_char >> 13) & 0x1f); // 5-bits

        if (fg_color == 0) // default foreground color
            fg_color = 39;
        else if (fg_color <= 9) // normal colors
            fg_color = (fg_color - 1) + 30;
        else if (fg_color <= 16) // bright colors
            fg_color = 90 + (fg_color - 9);
        else
            assert(0 && "invalid fg color");

        if (bg_color == 0) // default background color
            bg_color = 49;
        else if (bg_color <= 9) // normal colors
            bg_color = (bg_color - 1) + 40;
        else if (bg_color <= 16) // bright colors
            bg_color = 100 + (bg_color - 9);
        else
            assert(0 && "invalid bg color");

        char style_buffer[32]; // should be large enough
        char* it = style_buffer;
        *it++ = ';';

        // Handle all style attributes
        if ((styled_char & STYLE_BOLD) != 0) {
            *it++ = '1';
            *it++ = ';';
        }
        if ((styled_char & STYLE_FAINT) != 0) {
            *it++ = '2';
            *it++ = ';';
        }
        if ((styled_char & STYLE_ITALIC) != 0) {
            *it++ = '3';
            *it++ = ';';
        }
        if ((styled_char & STYLE_UNDERLINE) != 0) {
            *it++ = '4';
            *it++ = ';';
        }
        if ((styled_char & STYLE_BLINKING) != 0) {
            *it++ = '5'; // slow blinking, fast is not widely supported
            *it++ = ';';
        }
        if ((styled_char & STYLE_HIDE) != 0) {
            // No widely supported
            *it++ = '8';
            *it++ = ';';
        }
        if ((styled_char & STYLE_CROSSED) != 0) {
            // No widely supported
            *it++ = '9';
            *it++ = ';';
        }
        if ((styled_char & STYLE_OVERLINE) != 0) {
            // No widely supported
            *it++ = '5';
            *it++ = '3';
            *it++ = ';';
        }

        *(--it) = '\0'; // printf expect a NUL-terminated string

        // Emit the Select Graphic Rendition command to style the character.
        printf(SCI "0;%u;%u%sm", fg_color, bg_color, style_buffer);
    }
#endif // !DISABLE_SCREEN_STYLING

    putchar(ch);
#ifndef DISABLE_SCREEN_STYLING
    printf(SCI "0m");
#endif // !DISABLE_SCREEN_STYLING

    // Restore cursor position
    printf(SCI "u");
    fflush(stdout);
}

void screen_ram_write(addr_t addr, word_t new_word) {
    // Check if addr is in the bounds.
    assert(addr >= SCREEN_BASE_ADDR && addr < (SCREEN_BASE_ADDR + SCREEN_SIZE));

    const addr_t offset = addr - SCREEN_BASE_ADDR;
    const addr_t x = offset % SCREEN_WIDTH;
    const addr_t y = offset / SCREEN_WIDTH;
    screen_put_character(x, y, new_word);
}
