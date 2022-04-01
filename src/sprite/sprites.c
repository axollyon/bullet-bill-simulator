#include <ultra64.h>
#include "mdraw.h"
#include "game/main.h"
#include "game/game_init.h"

// Segment 2 (can be used anywhere)

void text_init(u8 r, u8 g, u8 b, u8 a) {
    gDPSetCombineLERP(
        gDisplayListHead++,
        0, 0, 0, ENVIRONMENT, ENVIRONMENT, 0, TEXEL0, 0,
        0, 0, 0, ENVIRONMENT, ENVIRONMENT, 0, TEXEL0, 0
    );
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);
    mfilter(1);
    mprint_start();
    gDPSetEnvColor(gDisplayListHead++, r, g, b, a);
}

extern const u8 texture_button[];
MSPRITE sprite_button = {texture_button, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64};

void draw_button(int x, int y, uint w, uint h, u8 r, u8 g, u8 b, u8 a) {
    gDPSetCombineLERP(
        gDisplayListHead++,
        TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0,
        TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0
    );
    gDPSetEnvColor(gDisplayListHead++, r, g, b, a);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);
    mfilter(1);
    msprite(&sprite_button, x, y, w, h);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);
}

// Intro (can only be used in the "intro" level)

extern const u8 texture_axollyon[];
MSPRITE sprite_axollyon = {texture_axollyon, G_IM_FMT_I, G_IM_SIZ_8b, 374, 319};

void draw_axollyon(int x, int y, u8 alpha) { // Centered on the x axis for widescreen support - origin is at top center, not top left!
    uint w = gConfig.widescreen ? 280 : 374;
    gDPSetCombineLERP(
        gDisplayListHead++,
        0, 0, 0, ENVIRONMENT, TEXEL0, 0, ENVIRONMENT, 0,
        0, 0, 0, ENVIRONMENT, TEXEL0, 0, ENVIRONMENT, 0
    );
    gDPSetEnvColor(gDisplayListHead++, 218, 187, 237, alpha);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);
    mfilter(1);
    msprite(&sprite_axollyon, x - (w / 2), y, w, 319);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);
}

// BOB

extern const u8 texture_logo[];
MSPRITE sprite_logo = {texture_logo, G_IM_FMT_IA, G_IM_SIZ_8b, 761, 407};

void draw_logo(int x, int y) { // Centered on the x axis for widescreen support - origin is at top center, not top left!
    uint w = gConfig.widescreen ? 570 : 761;
    gDPSetCombineLERP(
        gDisplayListHead++,
        0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0,
        0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0
    );
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);
    mfilter(1);
    msprite(&sprite_logo, x - (w / 2), y, w, 407);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);
}