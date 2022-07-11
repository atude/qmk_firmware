#include "satisfaction75.h"
#include "pets/draw_luna.h"
#include "pets/draw_kirby.h"
#include "pets/draw_pusheen.h"
#include <stdio.h>

void draw_current_pet(void) {
    switch (pet_mode) {
        default:
        case PET_LUNA:
            return draw_luna();
        case PET_PUSHEEN:
            return draw_pusheen();
        case PET_KIRBY:
            return draw_kirby();
    }
}
