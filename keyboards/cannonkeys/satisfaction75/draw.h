#pragma once

#include "satisfaction75.h"
#include "helpers.h"
#include "font.h"
#include <stdio.h>

#define min(x, y) (((x) >= (y)) ? (y) : (x))
#define draw_pixel(x, y, on) oled_write_pixel(x, y, on)

/**
 * Generic helpers
 */

void draw_line_h(uint8_t x, uint8_t y, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        oled_write_pixel(i + x, y, true);
    }
}

void draw_line_v(uint8_t x, uint8_t y, uint8_t len, bool on) {
    for (uint8_t i = 0; i < len; i++) {
        oled_write_pixel(x, i + y, on);
    }
}

void draw_rectangle(uint8_t x, uint8_t y, uint8_t x_len, uint8_t y_len, bool on) {
    for (uint8_t i = 0; i < x_len; i++) {
        for (uint8_t j = 0; j < y_len; j++) {
            oled_write_pixel(i + x, j + y, on);
        }
    }
}

// Draws from bottom y to top y. Only cursor x controllable
void draw_wpm_bar(uint8_t cursor_x, uint8_t wpm, char* date) {
    uint8_t x     = (cursor_x * 5) + cursor_x - 1;
    uint8_t y     = 5;
    uint8_t y_len = min(wpm / 6, 24);
    for (uint8_t i = 0; i < 2; i++) {
        for (uint8_t j = 0; j < y_len; j++) {
            oled_write_pixel(i + x + 14, 23 - j + y, true);
            oled_write_pixel(i + x - 3, 23 - j + y, true);
        }
    }

    // Embed date in wpm bar
    char date_first_section[3]  = {date[0], date[1], '\0'};
    char date_second_section[3] = {date[3], date[4], '\0'};
    if (wpm > 145) {
        draw_rectangle(x - 1, y, 15, 28, true);
    }
    oled_set_cursor(cursor_x, 1);
    oled_write(date_first_section, wpm > 145);
    oled_set_cursor(cursor_x, 2);
    oled_write(date_second_section, wpm > 145);

    // draw_line_h(x - 2, y - 2, 17);
    // draw_line_h(x - 3, y - 1, 19);
    // draw_line_h(x - 3, y + 24, 19);
    // draw_line_h(x - 2, y + 25, 17);
}

/**
 * Feature helpers
 */

// Mods square (S, C, A, G)
void draw_mods_square(uint8_t mod_state, int8_t enc_turn_state, bool show_enc_turn, uint8_t cursor_x, uint8_t cursor_y) {
    uint8_t x = (cursor_x * 5) + cursor_x - 1;
    uint8_t y = cursor_y * 8;
    oled_set_cursor(cursor_x, cursor_y);
    draw_line_h(x, y - 1, 7);
    draw_line_h(x, y + 7, 7);
    draw_line_v(x - 1, y, 7, true);
    draw_line_v(x + 7, y, 7, true);
    if ((mod_state & MOD_MASK_SHIFT) || (mod_state & MOD_MASK_CTRL) || (mod_state & MOD_MASK_ALT) || (mod_state & MOD_MASK_GUI) || (enc_turn_state != 0 && show_enc_turn)) {
        draw_rectangle(x, y - 1, 7, 9, true);
        if (mod_state & MOD_MASK_SHIFT) {
            oled_write_P(PSTR("S"), true);
        } else if (mod_state & MOD_MASK_CTRL) {
            oled_write_P(PSTR("C"), true);
        } else if (mod_state & MOD_MASK_ALT) {
            oled_write_P(PSTR("A"), true);
        } else if (mod_state & MOD_MASK_GUI) {
            oled_write_P(PSTR("G"), true);
        } else if (enc_turn_state >= 1) {
            oled_write_P(PSTR("+"), true);
        } else if (enc_turn_state <= 1) {
            oled_write_P(PSTR("-"), true);
        }
    }
}

// Info panel (vol, caps, layer) - horizontal line
void draw_info_panel(led_t led_state, uint8_t layer_state, char* enc_mode, uint8_t cursor_x, uint8_t cursor_y, bool is_long_caps) {
    uint8_t x               = (cursor_x * 5) + cursor_x - 1;
    uint8_t y               = cursor_y * 8;
    uint8_t long_caps_x_add = is_long_caps ? 6 : 0;

    oled_set_cursor(cursor_x, cursor_y);
    if (enc_press_state > 0) {
        draw_rectangle(x, y - 1, 19, 10, true);
        draw_line_v(x - 1, y, 8, true);
        draw_line_v(x + 19, y, 8, true);
        oled_write(enc_mode, true);
    } else {
        oled_write(enc_mode, false);
    }
    oled_advance_char();
    if (led_state.caps_lock) {
        draw_rectangle(x + 24, y - 1, 19 + long_caps_x_add, 10, true);
        draw_line_v(x + 24 - 1, y, 8, true);
        draw_line_v(x + 24 + 19 + long_caps_x_add, y, 8, true);
        oled_write_P(PSTR(is_long_caps ? "CAPS" : "CAP"), true);
    } else {
        oled_write_P(PSTR(is_long_caps ? "CAPS" : "CAP"), false);
    }
    oled_advance_char();
    if (biton32(layer_state) > 0) {
        draw_rectangle(x + 48 + long_caps_x_add, y - 1, 12, 10, true);
        draw_line_v(x + 48 - 1 + long_caps_x_add, y, 8, true);
        draw_line_v(x + 48 + 12 + long_caps_x_add, y, 8, true);
        oled_write_P(PSTR("L"), true);
        oled_write_char(get_highest_layer(layer_state) + 0x30, true);
    } else {
        oled_write_P(PSTR("L"), false);
        oled_write_char(get_highest_layer(layer_state) + 0x30, false);
    }
}

// Used in pets and gifs mode - generic details
void draw_details_panel(bool as_overlay) {
    if (!as_overlay || (as_overlay && (get_mods() & MOD_MASK_CTRL))) {
        led_t led_state = host_keyboard_led_state();

        // BG box
        if (as_overlay) {
            draw_rectangle(71, 0, 58, 32, false);
            draw_line_v(70, 1, 31, false);
            draw_line_v(69, 2, 30, false);
            draw_line_v(68, 3, 29, false);
            draw_line_v(67, 5, 27, false);
            draw_line_v(66, 8, 24, false);
            draw_line_v(65, 13, 19, false);
        }

        switch (date_time_mode) {
            default:
            case 0:
                oled_set_cursor(is_24hr_time() ? 16 : 14, 1);
                oled_write(get_time(), false);
                break;
            case 1:
                oled_set_cursor(13, 1);
                oled_write(get_date(false), false);
                break;
            case 2:
                break;
        }

        if (led_state.caps_lock) {
            oled_set_cursor(12, 3);
            oled_write_P(PSTR("CAPS"), false);
        } else {
            oled_set_cursor(13, 3);
            oled_write(get_enc_mode(), false);
        }
        oled_write_P(PSTR("   L"), false);
        draw_line_v(104, 24, 8, true);
        oled_write_char(get_highest_layer(layer_state) + 0x30, false);
    }
}


