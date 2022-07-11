#pragma once

#include "satisfaction75.h"
#include "draw.h"
#include "global.h"
#include "../frames/kirby.h"
#include "atyu_config.h"
#include <stdio.h>

uint8_t kirby_current_idle_frame      = 0;
uint8_t kirby_current_walk_frame      = 0;
uint8_t kirby_current_jump_frame      = 0;
uint8_t kirby_current_inhale_frame    = 0;
uint8_t kirby_current_exhale_frame    = 0;
uint8_t kirby_current_caps_idle_frame = 0;
uint8_t kirby_caps_state              = 0;  // 0 = no caps, 1 = in caps, 2 = exiting caps

void draw_kirby(void) {
    led_t   led_state = host_keyboard_led_state();
    uint8_t mod_state = get_mods();

    void animate_kirby(void) {
        if (wpm() <= KIRBY_MIN_WALK_SPEED) {
            kirby_current_idle_frame = (kirby_current_idle_frame + 1) % KIRBY_IDLE_FRAMES;
            if (led_state.caps_lock) {
                oled_write_raw_P(kirby_caps_idle[abs((KIRBY_IDLE_FRAMES - 1) - kirby_current_idle_frame)], DEFAULT_ANIM_SIZE);
            } else {
                oled_write_raw_P(kirby_idle[abs((KIRBY_IDLE_FRAMES - 1) - kirby_current_idle_frame)], DEFAULT_ANIM_SIZE);
            }
        } else {
            if (led_state.caps_lock) {
                kirby_current_walk_frame = (kirby_current_walk_frame + 1) % KIRBY_CAPS_WALK_FRAMES;
                oled_write_raw_P(kirby_caps_walk[abs((KIRBY_CAPS_WALK_FRAMES - 1) - kirby_current_walk_frame)], DEFAULT_ANIM_SIZE);
            } else {
                kirby_current_walk_frame = (kirby_current_walk_frame + 1) % KIRBY_WALK_FRAMES;
                oled_write_raw_P(kirby_walk[abs((KIRBY_WALK_FRAMES - 1) - kirby_current_walk_frame)], DEFAULT_ANIM_SIZE);
            }
        }
    }

    if (kirby_caps_state == 1) {
        // Inhale - prepare caps
        if (timer_elapsed32(anim_timer) > KIRBY_INHALE_FRAME_DURATION) {
            oled_write_raw_P(kirby_inhale[min(kirby_current_inhale_frame, (KIRBY_INHALE_FRAMES - 1))], DEFAULT_ANIM_SIZE);
            kirby_current_inhale_frame = kirby_current_inhale_frame + 1;
            anim_timer                 = timer_read32();
            if (kirby_current_inhale_frame > KIRBY_INHALE_FRAMES - 1) {
                kirby_caps_state = 2;
            }
        }
    } else if (kirby_caps_state == 3) {
        // Exhale - exit caps
        if (timer_elapsed32(anim_timer) > KIRBY_EXHALE_FRAME_DURATION) {
            oled_write_raw_P(kirby_exhale[min(kirby_current_exhale_frame, (KIRBY_EXHALE_FRAMES - 1))], DEFAULT_ANIM_SIZE);
            kirby_current_exhale_frame = kirby_current_exhale_frame + 1;
            anim_timer                 = timer_read32();
            if (kirby_current_exhale_frame > KIRBY_EXHALE_FRAMES - 1) {
                kirby_caps_state = 0;
            }
        }
    } else if (!showed_jump) {
        // Jump
        if (timer_elapsed32(anim_timer) > KIRBY_JUMP_FRAME_DURATION) {
            oled_write_raw_P(kirby_jump[min(kirby_current_jump_frame, (KIRBY_JUMP_FRAMES - 1))], DEFAULT_ANIM_SIZE);
            kirby_current_jump_frame = kirby_current_jump_frame + 1;
            anim_timer               = timer_read32();
            if (kirby_current_jump_frame > KIRBY_JUMP_FRAMES - 1) {
                showed_jump = true;
            }
        }
    } else {
        if (led_state.caps_lock && kirby_caps_state == 0) {
            // Go into caps mode
            kirby_current_inhale_frame = 0;
            kirby_caps_state           = 1;
        } else if (!led_state.caps_lock && kirby_caps_state == 2) {
            // Exit caps mode
            kirby_current_exhale_frame = 0;
            kirby_caps_state           = 3;
        } else if (space_pressed && showed_jump && !led_state.caps_lock) {
            kirby_current_jump_frame = 0;
            showed_jump              = false;
        } else if (mod_state & MOD_MASK_CTRL && !led_state.caps_lock) {
            // 2 = crouched state in jump anim
            oled_write_raw_P(kirby_jump[2], DEFAULT_ANIM_SIZE);
        } else {
            if ((wpm() <= KIRBY_MIN_WALK_SPEED && timer_elapsed32(anim_timer) > KIRBY_IDLE_FRAME_DURATION) || (wpm() > KIRBY_MIN_WALK_SPEED && timer_elapsed32(anim_timer) > (led_state.caps_lock ? get_frame_duration() * 2 : get_frame_duration())) || (kirby_current_jump_frame > 0 && timer_elapsed32(anim_timer) > KIRBY_JUMP_FRAME_DURATION) || (kirby_current_inhale_frame > 0 && timer_elapsed32(anim_timer) > KIRBY_INHALE_FRAME_DURATION) || (kirby_current_exhale_frame > 0 && timer_elapsed32(anim_timer) > KIRBY_EXHALE_FRAME_DURATION)) {
                animate_kirby();
                anim_timer                 = timer_read32();
                kirby_current_jump_frame   = 0;
                kirby_current_inhale_frame = 0;
                kirby_current_exhale_frame = 0;
            }
        }
    }

    draw_details_panel(false);
    if (date_time_mode == 2) {
        oled_set_cursor(16, 1);
        oled_write_P(PSTR("KIRBY"), false);
    }
}
