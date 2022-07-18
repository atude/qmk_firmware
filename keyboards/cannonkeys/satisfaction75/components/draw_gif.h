#pragma once

#include "satisfaction75.h"
#include "draw.h"
#include "global.h"
#include "atyu_config.h"
#include <stdio.h>

uint32_t gif_anim_timer = 0;
uint8_t gif_curr_frame = 0;

void draw_gif(void) {
    void animate_gif(void) {
		oled_write_raw_P(gif[gif_curr_frame], DEFAULT_ANIM_SIZE);
		gif_curr_frame = gif_curr_frame + 1;
		if (gif_curr_frame >= GIF_LENGTH) {
            gif_curr_frame = 0;
		}
	}

	if (timer_elapsed32(gif_anim_timer) > ANIM_GIF_SPEED) {
		gif_anim_timer = timer_read32();
		animate_gif();
	}

    draw_details_panel(true);
}
