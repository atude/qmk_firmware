#pragma once

#include <stdio.h>

#define ANIM_FRAME_DURATION 200 // how long each frame lasts in ms - not used for bongo, set dynamically
#define DEFAULT_ANIM_SIZE 512   // for 128*32 frames

static uint32_t anim_timer    = 0;
static bool     space_pressed = false;

// Dynamic variables specific to modes
static uint8_t bongo_state_tap  = 0;
static bool    showed_jump = true;
