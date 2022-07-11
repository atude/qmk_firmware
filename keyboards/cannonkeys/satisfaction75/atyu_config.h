#pragma once

#include <stdio.h>

#define OLED_CLOCK_ENABLED false
#define OLED_BONGO_ENABLED true
#define OLED_PETS_ENABLED true
#define OLED_BONGO_FILLED false
#define OLED_PET_LUNA_ENABLED true
#define OLED_PET_KIRBY_ENABLED false
#define OLED_PET_PUSHEEN_ENABLED true

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
#if OLED_PET_LUNA_ENABLED
    PET_LUNA,
#endif
#if OLED_PET_KIRBY_ENABLED
    PET_KIRBY,
#endif
#if OLED_PET_PUSHEEN_ENABLED
    PET_PUSHEEN,
#endif
    _NUM_PET_MODES,
};
#endif
