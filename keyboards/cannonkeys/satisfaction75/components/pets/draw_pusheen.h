#pragma once

#include "satisfaction75.h"
#include "draw.h"
#include "global.h"
#include "../frames/pusheen.h"
#include "atyu_config.h"
#include <stdio.h>

uint8_t pusheen_current_frame = 0;
uint8_t pusheen_anim_type     = 0;

void draw_pusheen(void) {
    led_t   led_state = host_keyboard_led_state();
    uint8_t mod_state = get_mods();

    void animate_pusheen(void) {
        pusheen_current_frame = (pusheen_current_frame + 1) % 2;

        if (led_state.caps_lock) {
            oled_write_raw_P(pusheen_cool[abs(1 - pusheen_current_frame)], DEFAULT_ANIM_SIZE);
        } else if (mod_state & MOD_MASK_CTRL) {
            oled_write_raw_P(pusheen_idle[abs(1 - pusheen_current_frame)], DEFAULT_ANIM_SIZE);
        } else if (get_timeout() + timer_elapsed32(oled_sleep_timer) > PRE_SLEEP_TIMEOUT) {
            oled_write_raw_P(pusheen_sleep[abs(1 - pusheen_current_frame)], DEFAULT_ANIM_SIZE);
        } else if (wpm() < 40) {
            oled_write_raw_P(pusheen_wait[abs(1 - pusheen_current_frame)], DEFAULT_ANIM_SIZE);
        } else {
            // Filter random anims
            if (pusheen_anim_type == 1) {
                oled_write_raw_P(pusheen_eat[abs(1 - pusheen_current_frame)], DEFAULT_ANIM_SIZE);
            } else if (pusheen_anim_type == 2) {
                oled_write_raw_P(pusheen_drink[abs(1 - pusheen_current_frame)], DEFAULT_ANIM_SIZE);
            } else if (pusheen_anim_type == 3) {
                oled_write_raw_P(pusheen_walk[abs(1 - pusheen_current_frame)], DEFAULT_ANIM_SIZE);
            } else if (pusheen_anim_type == 4) {
                oled_write_raw_P(pusheen_lick[abs(1 - pusheen_current_frame)], DEFAULT_ANIM_SIZE);
            } else if (pusheen_anim_type == 5) {
                oled_write_raw_P(pusheen_play[abs(1 - pusheen_current_frame)], DEFAULT_ANIM_SIZE);
            }
        }
    }

    /* animation timer */
    if (wpm() < 40 && timer_elapsed32(anim_timer) > PUSHEEN_IDLE_FRAME_DURATION) {
        pusheen_anim_type = 0;
        anim_timer        = timer_read32();
        animate_pusheen();
    } else if (wpm() >= 40 && timer_elapsed32(anim_timer) > PUSHEEN_WALK_FRAME_DURATION) {
        if (pusheen_anim_type == 0) {
            // random animation based on timer
            pusheen_anim_type = (timer_elapsed32(anim_timer) % 5) + 1;
        }
        anim_timer = timer_read32();
        animate_pusheen();
    }

    draw_details_panel(false);
    if (date_time_mode == 2) {
        oled_set_cursor(14, 1);
        oled_write_P(PSTR("PUSHEEN"), false);
    }
}
