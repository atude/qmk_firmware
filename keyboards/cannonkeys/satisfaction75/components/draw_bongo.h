#pragma once

#include "satisfaction75.h"
#include "draw.h"
#include "global.h"
#include "helpers.h"
#include "atyu.h"
#include "frames/bongo.h"
#include <stdio.h>

#if OLED_BONGO_ENABLED

static uint8_t bongo_current_idle_frame     = 0;
static uint8_t bongo_current_pre_idle_frame = 0;
static uint8_t bongo_current_tap_frame      = 0;
static uint8_t bongo_current_caps_frame     = 0;

void draw_bongo(void) {
    led_t   led_state = host_keyboard_led_state();
    uint8_t mod_state = get_mods();

    // assumes 1 frame prep stage
    // mode 0 = default, mode 1 = pre idle
    void animation_phase(uint8_t mode) {
        if (led_state.caps_lock) {
            bongo_current_caps_frame = (bongo_current_caps_frame + 1) % BONGO_CAPS_FRAMES;
            oled_write_raw_P(bongo_caps[abs((BONGO_CAPS_FRAMES - 1) - bongo_current_caps_frame)], DEFAULT_ANIM_SIZE);
            return;
        }
        bongo_current_caps_frame = 0;
        if (mode == 1) {
            bongo_current_pre_idle_frame = (bongo_current_pre_idle_frame + 1) % BONGO_PRE_IDLE_FRAMES;
            oled_write_raw_P(bongo_pre_idle[abs((BONGO_PRE_IDLE_FRAMES - 1) - bongo_current_pre_idle_frame)], DEFAULT_ANIM_SIZE);
        } else {
            bongo_current_idle_frame = (bongo_current_idle_frame + 1) % BONGO_IDLE_FRAMES;
            oled_write_raw_P(bongo_idle[abs((BONGO_IDLE_FRAMES - 1) - bongo_current_idle_frame)], DEFAULT_ANIM_SIZE);
        }
    }

    if ((mod_state & MOD_MASK_CTRL) && bongo_state_tap != 1) {
        oled_write_raw_P(bongo_hiding[0], DEFAULT_ANIM_SIZE);
        anim_timer      = timer_read32();
        bongo_state_tap = 2;
    } else if ((mod_state & MOD_MASK_GUI) && bongo_state_tap != 1) {
        oled_write_raw_P(bongo_blushing[0], DEFAULT_ANIM_SIZE);
        anim_timer      = timer_read32();
        bongo_state_tap = 2;
    } else if (bongo_state_tap == 1) {
        if (led_state.caps_lock) {
            oled_write_raw_P(bongo_caps[1], DEFAULT_ANIM_SIZE);
        } else if (mod_state & MOD_MASK_CTRL) {
            bongo_current_tap_frame = (bongo_current_tap_frame + 1) % BONGO_TAP_FRAMES;
            oled_write_raw_P(bongo_hiding_tap[abs((BONGO_TAP_FRAMES - 1) - bongo_current_tap_frame)], DEFAULT_ANIM_SIZE);
        } else if (mod_state & MOD_MASK_GUI) {
            bongo_current_tap_frame = (bongo_current_tap_frame + 1) % BONGO_TAP_FRAMES;
            oled_write_raw_P(bongo_blushing_tap[abs((BONGO_TAP_FRAMES - 1) - bongo_current_tap_frame)], DEFAULT_ANIM_SIZE);
        } else if (wpm() > 140) {
            bongo_current_tap_frame = (bongo_current_tap_frame + 1) % BONGO_TAP_FRAMES;
            oled_write_raw_P(bongo_tap_cute[abs((BONGO_TAP_FRAMES - 1) - bongo_current_tap_frame)], DEFAULT_ANIM_SIZE);
        } else {
            bongo_current_tap_frame = (bongo_current_tap_frame + 1) % BONGO_TAP_FRAMES;
            oled_write_raw_P(bongo_tap[abs((BONGO_TAP_FRAMES - 1) - bongo_current_tap_frame)], DEFAULT_ANIM_SIZE);
        }
        anim_timer      = timer_read32();
        bongo_state_tap = 2;
    } else if (timer_elapsed32(anim_timer) < 3000 && bongo_state_tap == 2) {
        if (led_state.caps_lock) {
            oled_write_raw_P(bongo_caps[0], DEFAULT_ANIM_SIZE);
        } else {
            oled_write_raw_P(bongo_prep[0], DEFAULT_ANIM_SIZE);
        }
    } else {
        bongo_state_tap = 0;
        if (get_timeout() + timer_elapsed32(oled_sleep_timer) > PRE_SLEEP_TIMEOUT && !led_state.caps_lock) {
            if (timer_elapsed32(anim_timer) > BONGO_SLEEP_FRAME_DURATION) {
                anim_timer = timer_read32();
                animation_phase(1);
            }
        } else if (timer_elapsed32(anim_timer) > BONGO_IDLE_FRAME_DURATION) {
            anim_timer = timer_read32();
            animation_phase(0);
        }
    }

    // Text drawing
    oled_set_cursor(0, 1);
    switch (date_time_mode) {
        case 0:
            oled_write(get_time(), false);
            break;
        case 1:
            oled_write(get_date(false), false);
            break;
    }

    if (biton32(layer_state) > 0) {
        oled_set_cursor(19, 2);
        oled_write_P(PSTR("L"), false);
        oled_write_char(get_highest_layer(layer_state) + 0x30, false);
    }

    if (led_state.caps_lock) {
        oled_set_cursor(0, 2);
        oled_write_P(PSTR("CAPS"), false);
    }

    if (encoder_mode != ENC_MODE_VOLUME) {
        oled_set_cursor(0, 3);
        oled_write(get_enc_mode(), false);
    }

    if (date_time_mode != 2) {
        oled_set_cursor(18, 3);
        if (wpm() > 0) {
            oled_write(get_u8_str(wpm(), '0'), false);
        } else {
            oled_write_P(PSTR("wpm"), false);
        }
    }
}

#endif
