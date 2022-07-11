#pragma once

#include "satisfaction75.h"
#include "draw.h"
#include "global.h"
#include "../frames/luna.h"
#include "atyu_config.h"
#include <stdio.h>

uint8_t luna_current_frame = 0;

void draw_luna(void) {
    led_t   led_state = host_keyboard_led_state();
    uint8_t mod_state = get_mods();

    void animate_luna(void) {
        luna_current_frame = (luna_current_frame + 1) % 2;

        if (led_state.caps_lock) {
            oled_write_raw_P(luna_bark[abs(1 - luna_current_frame)], DEFAULT_ANIM_SIZE);
        } else if (mod_state & MOD_MASK_CTRL) {
            oled_write_raw_P(luna_sneak[abs(1 - luna_current_frame)], DEFAULT_ANIM_SIZE);
        } else if (wpm() <= LUNA_MIN_WALK_SPEED) {
            oled_write_raw_P(luna_sit[abs(1 - luna_current_frame)], DEFAULT_ANIM_SIZE);
        } else if (wpm() <= LUNA_MIN_RUN_SPEED) {
            oled_write_raw_P(luna_walk[abs(1 - luna_current_frame)], DEFAULT_ANIM_SIZE);
        } else {
            oled_write_raw_P(luna_run[abs(1 - luna_current_frame)], DEFAULT_ANIM_SIZE);
        }
    }

    /* animation timer */
    if (space_pressed && !showed_jump && wpm() > LUNA_MIN_RUN_SPEED) {
        oled_write_raw_P(luna_jump[0], DEFAULT_ANIM_SIZE);
        showed_jump = true;
    } else if (timer_elapsed32(anim_timer) > ANIM_FRAME_DURATION) {
        anim_timer  = timer_read32();
        showed_jump = false;
        animate_luna();
    }

    draw_details_panel(false);
    if (date_time_mode == 2) {
        oled_set_cursor(17, 1);
        oled_write_P(PSTR("LUNA"), false);
    }
}
