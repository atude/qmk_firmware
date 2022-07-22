#include "satisfaction75.h"
#include "pets/draw_luna.h"
#include "pets/draw_kirby.h"
#include "pets/draw_pusheen.h"
#include "atyu.h"
#include <stdio.h>

void draw_current_pet(void) {
    switch (pet_mode) {
        default:
#if OLED_PET_LUNA_ENABLED
        case PET_LUNA:
            return draw_luna();
#endif
#if OLED_PET_KIRBY_ENABLED
        case PET_KIRBY:
            return draw_kirby();
#endif
#if OLED_PET_PUSHEEN_ENABLED
        case PET_PUSHEEN:
            return draw_pusheen();
#endif
    }
}
