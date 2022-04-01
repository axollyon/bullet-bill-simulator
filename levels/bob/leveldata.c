#include <ultra64.h>
#include "sm64.h"
#include "surface_terrains.h"
#include "moving_texture_macros.h"
#include "level_misc_macros.h"
#include "macro_preset_names.h"
#include "special_preset_names.h"
#include "textures.h"
#include "dialog_ids.h"
#include "src/game/puppycam2.h"

#include "make_const_nonconst.h"
#include "levels/bob/logo.inc.c"

struct sPuppyAngles chaseAngles =
{
    /* pos */ {0, PUPPY_NULL, -850},
    /* focus */ {PUPPY_NULL, -3000, 8000},
    /* yaw */ PUPPY_NULL,
    /* pitch */ 0x3000,
    /* zoom */ 1000,
};

#include "levels/bob/leveldata.inc.c"
