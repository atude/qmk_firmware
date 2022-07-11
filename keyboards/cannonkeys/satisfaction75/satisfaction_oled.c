#include "satisfaction75.h"
#include "draw.h"
#include "global.h"
#include "helpers.h"
#include "atyu_config.h"
#include <stdio.h>

#if OLED_CLOCK_ENABLED
#include "components/draw_big_clock.h"
#endif
#if OLED_BONGO_ENABLED
#include "components/draw_bongo.h"
#endif
#if OLED_PETS_ENABLED
#include "components/draw_pets.h"
#endif
#if OLED_GIF_ENABLED
#include "components/draw_gif.h"
#endif

void draw_settings(void);  // set time/date and settings
void draw_matrix(void);

#ifdef OLED_ENABLE

/* Matrix display is 12 x 12 pixels - def position is 0, 0*/
#    define MATRIX_DISPLAY_X 1
#    define MATRIX_DISPLAY_Y 5
#    define MATRIX_SCALE 3

// Enc turn state
int8_t   prev_enc_turn_state  = 0;  // just for tracking
uint32_t enc_turn_state_timer = 0;
bool     show_enc_turn  = false;

// Generic variables
uint8_t  prev_oled_mode = 0;

oled_rotation_t oled_init_kb(oled_rotation_t rotation) { return OLED_ROTATION_0; }

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    switch (keycode) {
        case KC_SPC:
            if (record->event.pressed) {
                space_pressed = true;
                // showed_jump   = false;
            } else {
                space_pressed = false;
            }
            break;
        case KC_F22:
        case KC_F23:
        case KC_F24:
            anim_timer = 99;  // forces an animation reset
            break;
        case KC_CAPS:
        case KC_LCTL:
        case KC_RCTL:
        case KC_LGUI:
        case KC_RGUI:
#if OLED_PETS_ENABLED
            if (oled_mode == OLED_PETS) {
                anim_timer = 99;  // forces an animation reset
            }
#endif
            return true;
    }
    if (record->event.pressed) {
        bongo_state_tap = 1;
    }
    return true;
}

void reset_animations(void) {
    anim_timer  = 0;
    showed_jump = true;
    oled_clear();
    oled_on();
}

// Will run before every oled update, except for oled off
void bg_tasks(void) {
    // Updating encoder turn state
    if ((enc_turn_state >= 1 || enc_turn_state <= -1) && enc_turn_state != prev_enc_turn_state) {
        show_enc_turn  = true;
        prev_enc_turn_state  = enc_turn_state;
        enc_turn_state_timer = timer_read32();
    }
    if (timer_elapsed32(enc_turn_state_timer) > 500) {
        show_enc_turn = false;
    }
}

bool oled_task_kb(void) {
    if (!oled_task_user()) {
        return false;
    }
    if (!oled_task_needs_to_repaint()) {
        return false;
    }
    if (clock_set_mode) {
        oled_clear();
        draw_settings();
        return false;
    }
    if (prev_oled_mode != oled_mode) {
        reset_animations();
        prev_oled_mode = oled_mode;
    }
    if (oled_mode == OLED_OFF) {
        oled_off();
        return false;
    }
    bg_tasks();

    switch (oled_mode) {
        default:
        case OLED_DEFAULT:
            oled_clear();
            draw_matrix();
            break;
#if OLED_CLOCK_ENABLED
        case OLED_CLOCK:
            oled_clear();
            draw_big_clock(show_enc_turn);
            break;
#endif
#if OLED_BONGO_ENABLED
        case OLED_BONGO:
            draw_bongo();
            break;
#endif
#if OLED_PETS_ENABLED
        case OLED_PETS:
            draw_current_pet();
            break;
#endif
#if OLED_GIF_ENABLED
        case OLED_GIF:
            draw_gif();
            break;
#endif
    }
    return false;
}

// Request a repaint of the OLED image without resetting the OLED sleep timer.
// Used for things like clock updates that should not keep the OLED turned on
// if there is no other activity.
void oled_request_repaint(void) {
    if (is_oled_on()) {
        oled_repaint_requested = true;
    }
}

// Request a repaint of the OLED image and reset the OLED sleep timer.
// Needs to be called after any activity that should keep the OLED turned on.
void oled_request_wakeup(void) { oled_wakeup_requested = true; }

// Check whether oled_task_user() needs to repaint the OLED image.  This
// function should be called at the start of oled_task_user(); it also handles
// the OLED sleep timer and the OLED_OFF mode.
bool oled_task_needs_to_repaint(void) {
    // In the OLED_OFF mode the OLED is kept turned off; any wakeup requests
    // are ignored.
    if ((oled_mode == OLED_OFF) && !clock_set_mode) {
        oled_wakeup_requested  = false;
        oled_repaint_requested = false;
        oled_off();
        return false;
    }

    // If OLED wakeup was requested, reset the sleep timer and do a repaint.
    if (oled_wakeup_requested) {
        oled_wakeup_requested  = false;
        oled_repaint_requested = false;
        oled_sleep_timer       = timer_read32() + get_timeout();
        oled_on();
        return true;
    }

    // If OLED repaint was requested, just do a repaint without touching the
    // sleep timer.
    if (oled_repaint_requested) {
        oled_repaint_requested = false;
        return true;
    }

    // If the OLED is currently off, skip the repaint (which would turn the
    // OLED on if the image is changed in any way).
    if (!is_oled_on()) {
        return false;
    }

    // If the sleep timer has expired while the OLED was on, turn the OLED off.
    if (timer_expired32(timer_read32(), oled_sleep_timer)) {
        oled_off();
        return false;
    }

    // Always perform a repaint if the OLED is currently on.  (This can
    // potentially be optimized to avoid unneeded repaints if all possible
    // state changes are covered by oled_request_repaint() or
    // oled_request_wakeup(), but then any missed calls to these functions
    // would result in displaying a stale image.)
    return true;
}

// Custom matrix
void draw_matrix() {
    // matrix
    for (uint8_t x = 0; x < MATRIX_ROWS; x++) {
        for (uint8_t y = 0; y < MATRIX_COLS; y++) {
            bool on = (matrix_get_row(x) & (1 << y)) > 0;

            if (on) {
                if (matrix_is_on(5, 5)) {
                    // spacebar
                    draw_rectangle(MATRIX_DISPLAY_X + (7 * MATRIX_SCALE) - 3, MATRIX_DISPLAY_Y + (7 * MATRIX_SCALE), MATRIX_SCALE * 5, MATRIX_SCALE, true);
                }
                if (matrix_is_on(3, 0)) {
                    // caps lock
                    draw_rectangle(MATRIX_DISPLAY_X + ((0 + 2) * MATRIX_SCALE), MATRIX_DISPLAY_Y + ((3 + 2) * MATRIX_SCALE), MATRIX_SCALE * 2, MATRIX_SCALE, true);
                }
                if (matrix_is_on(4, 0)) {
                    // lshift
                    draw_rectangle(MATRIX_DISPLAY_X + ((0 + 2) * MATRIX_SCALE), MATRIX_DISPLAY_Y + ((4 + 2) * MATRIX_SCALE), MATRIX_SCALE * 2, MATRIX_SCALE, true);
                }
                if (matrix_is_on(1, 13)) {
                    // backspace and enter
                    draw_rectangle(-2 + MATRIX_DISPLAY_X + ((13 + 2) * MATRIX_SCALE), MATRIX_DISPLAY_Y + ((1 + 2) * MATRIX_SCALE), MATRIX_SCALE * 2, MATRIX_SCALE, true);
                }
                if (matrix_is_on(3, 13)) {
                    // backspace and enter
                    draw_rectangle(-2 + MATRIX_DISPLAY_X + ((13 + 2) * MATRIX_SCALE), MATRIX_DISPLAY_Y + ((3 + 2) * MATRIX_SCALE), MATRIX_SCALE * 2, MATRIX_SCALE, true);
                }
                if (matrix_is_on(5, 15)) {
                    // align right arrow key
                    draw_rectangle(-3 + MATRIX_DISPLAY_X + ((15 + 2) * MATRIX_SCALE), MATRIX_DISPLAY_Y + ((5 + 2) * MATRIX_SCALE), MATRIX_SCALE, MATRIX_SCALE, true);
                }
                // dont print misaligned parts
                if (!(x == 5 && y == 15)) {
                    // rest of characters
                    draw_rectangle(MATRIX_DISPLAY_X + ((y + 2) * MATRIX_SCALE), MATRIX_DISPLAY_Y + ((x + 2) * MATRIX_SCALE), MATRIX_SCALE, MATRIX_SCALE, true);
                }
            }
        }
    }

    // outline
    // top
    draw_line_h(MATRIX_DISPLAY_X + 2, MATRIX_DISPLAY_Y, 19 * MATRIX_SCALE - 3);
    draw_line_h(MATRIX_DISPLAY_X + 1, MATRIX_DISPLAY_Y + 1, 19 * MATRIX_SCALE - 1);
    // bottom
    draw_line_h(MATRIX_DISPLAY_X + 1, MATRIX_DISPLAY_Y - 1 + (9 * MATRIX_SCALE), 19 * MATRIX_SCALE - 1);
    draw_line_h(MATRIX_DISPLAY_X + 2, MATRIX_DISPLAY_Y + (9 * MATRIX_SCALE), 19 * MATRIX_SCALE - 3);
    // left
    draw_line_v(MATRIX_DISPLAY_X, MATRIX_DISPLAY_Y + 2, 9 * MATRIX_SCALE - 3, true);
    draw_line_v(MATRIX_DISPLAY_X + 1, MATRIX_DISPLAY_Y + 2, 9 * MATRIX_SCALE - 2, true);
    // right
    draw_line_v(MATRIX_DISPLAY_X + (19 * MATRIX_SCALE), MATRIX_DISPLAY_Y + 2, 9 * MATRIX_SCALE - 3, true);
    draw_line_v(MATRIX_DISPLAY_X - 1 + (19 * MATRIX_SCALE), MATRIX_DISPLAY_Y + 2, 9 * MATRIX_SCALE - 2, true);

    // encoder
    draw_rectangle(MATRIX_DISPLAY_X + 51, MATRIX_DISPLAY_Y + 8, 3, 4, true);
    draw_line_h(MATRIX_DISPLAY_X + 51, MATRIX_DISPLAY_Y + 12, 3);
    draw_line_v(MATRIX_DISPLAY_X + 50, MATRIX_DISPLAY_Y + 9, 3, true);
    draw_line_v(MATRIX_DISPLAY_X + 54, MATRIX_DISPLAY_Y + 9, 3, true);

    // oled location
    for (int i = -1; i < MATRIX_SCALE - 1; i++) {
        draw_line_h(2 + MATRIX_DISPLAY_X + (14 * MATRIX_SCALE), -1 + MATRIX_DISPLAY_Y + i + (2 * MATRIX_SCALE), 3 * MATRIX_SCALE);
    }

    // caps lock
    led_t   led_state = host_keyboard_led_state();
    uint8_t mod_state = get_mods();

    if (led_state.caps_lock) {
        draw_rectangle(MATRIX_DISPLAY_X + ((0 + 2) * MATRIX_SCALE), MATRIX_DISPLAY_Y + ((3 + 2) * MATRIX_SCALE), MATRIX_SCALE * 2, MATRIX_SCALE, true);
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
            oled_set_cursor(16, 1);
            oled_write_P(PSTR("SAT75"), false);
            break;
    }

    draw_mods_square(mod_state, enc_turn_state, show_enc_turn, 11, 1);
    draw_info_panel(led_state, layer_state, get_enc_mode(), 11, 3, false);
}

void draw_settings() {
    oled_set_cursor(0, 0);
    oled_write_P(PSTR("SETTINGS"), false);
    oled_set_cursor(12, 0);
    oled_write_P(PSTR("atude v13"), false);
    oled_set_cursor(0, 2);
    if (time_config_idx >= 0 && time_config_idx < 2) {
        oled_write(get_time(), false);
        oled_set_cursor(17, 2);
        oled_write_P(PSTR("TIME"), false);
    } else if (time_config_idx >= 2 && time_config_idx < 5) {
        oled_write(get_date(true), false);
        oled_set_cursor(17, 2);
        oled_write_P(PSTR("DATE"), false);
    } else if (time_config_idx >= 5 && time_config_idx < 6) {
        switch (date_time_format_mode) {
            default:
            case 0:
                oled_write_P(PSTR("12h dd/mm/yy"), false);
                break;
            case 1:
                oled_write_P(PSTR("24h dd/mm/yy"), false);
                break;
            case 2:
                oled_write_P(PSTR("12h mm/dd/yy"), false);
                break;
            case 3:
                oled_write_P(PSTR("24h mm/dd/yy"), false);
                break;
        }
        oled_set_cursor(15, 2);
        oled_write_P(PSTR("FORMAT"), false);
    } else {
        switch (timeout_mode) {
            default:
            case 0:
                oled_write_P(PSTR("1m30s"), false);
                break;
            case 1:
                oled_write_P(PSTR("2m"), false);
                break;
            case 2:
                oled_write_P(PSTR("5m"), false);
                break;
            case 3:
                oled_write_P(PSTR("1m"), false);
                break;
        }
        oled_set_cursor(14, 2);
        oled_write_P(PSTR("TIMEOUT"), false);
    }

    if (clock_set_mode) {
        switch (time_config_idx) {
            case 0:  // hour
            default:
                draw_line_h(0, 24, 10);
                break;
            case 1:  // minute
                draw_line_h(18, 24, 10);
                break;
            case 2:  // year
                draw_line_h(36, 24, 24);
                break;
            case 3:  // month or day (depending on date_time_format_mode)
                if (!is_month_first_date()) {
                    draw_line_h(18, 24, 10);
                } else {
                    draw_line_h(0, 24, 10);
                }
                break;
            case 4:  // day or month (depending on date_time_format_mode)
                if (!is_month_first_date()) {
                    draw_line_h(0, 24, 10);
                } else {
                    draw_line_h(18, 24, 10);
                }
                break;
            case 5:  // date time format mode, timeout mode
            case 6:
                break;
        }
    }
}

#endif
