#pragma once

#include <stdio.h>

#define OLED_CLOCK_ENABLED false
#define OLED_BONGO_ENABLED true
#define OLED_PETS_ENABLED true

#define OLED_BONGO_FILLED false

enum oled_modes {
    OLED_DEFAULT,
#if OLED_CLOCK_ENABLED
    OLED_CLOCK,
#endif
#if OLED_BONGO_ENABLED
    OLED_BONGO,
#endif
#if OLED_PETS_ENABLED
    OLED_PETS,
#endif
    OLED_OFF,
    _NUM_OLED_MODES
};

#if OLED_PETS_ENABLED
enum pet_modes {
    PET_LUNA,
    PET_KIRBY,
    PET_PUSHEEN,
    _NUM_PET_MODES,
};
#endif
