#pragma once

#include "satisfaction75.h"
#include "draw.h"
#include "helpers.h"
#include "atyu_config.h"
#include <stdio.h>

#ifdef OLED_CLOCK_ENABLED

// Draw a set of pixels within defined bounds; pixels are defined as a 2D array of 0s and 1s
void draw_clock_digit(uint8_t x_start, uint8_t y_start, uint8_t pixels[CLOCK_FONT_ROWS][CLOCK_FONT_COLS]) {
    for (uint8_t i = 0; i < CLOCK_FONT_COLS; i++) {
        for (uint8_t j = 0; j < CLOCK_FONT_ROWS; j++) {
            uint8_t x = i + x_start;
            uint8_t y = j + y_start;
            draw_pixel(x, y, pixels[j][i] == 1);
        }
    }
}

void draw_big_clock(bool show_enc_turn) {
    led_t   led_state = host_keyboard_led_state();
    uint8_t mod_state = get_mods();
    uint8_t x = 4;
    uint8_t y = 3;
    uint8_t  hour         = last_minute / 60;
    uint16_t minute       = last_minute % 60;
    uint8_t  digit_w      = 10;
    bool     is_pm        = (hour / 12) > 0;
    bool is_24hr = is_24hr_time();

    if (!is_24hr) {
        hour = hour % 12;
        if (hour == 0) {
            hour = 12;
        }
    }

    // hh
    draw_clock_digit(x, y, clock_font[(int)(hour / 10)]);
    draw_clock_digit(x + digit_w, y, hour == 24 ? clock_font[0] : clock_font[hour % 10]);
    // colon
    draw_clock_digit(x + digit_w * 2, y, clock_font[13]);
    // // mm
    draw_clock_digit(x + digit_w * 3, y, minute == 60 ? clock_font[0] : clock_font[(int)(minute / 10)]);
    draw_clock_digit(x + digit_w * 4, y, clock_font[minute % 10]);

    if (!is_24hr) {
        draw_clock_digit(x + digit_w * 5, y, clock_font[is_pm ? 11 : 10]);
        draw_clock_digit(x + digit_w * 6, y, clock_font[12]);
    }

    draw_mods_square(mod_state, enc_turn_state, show_enc_turn, 1, 3);
    draw_info_panel(led_state, layer_state, get_enc_mode(), 3, 3, true);
    draw_wpm_bar(18, wpm(), get_date(false));
};

#endif
