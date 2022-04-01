#include <PR/ultratypes.h>

#include "sm64.h"
#include "actors/common1.h"
#include "gfx_dimensions.h"
#include "game_init.h"
#include "level_update.h"
#include "camera.h"
#include "print.h"
#include "ingame_menu.h"
#include "hud.h"
#include "segment2.h"
#include "area.h"
#include "save_file.h"
#include "print.h"
#include "engine/surface_load.h"
#include "engine/math_util.h"
#include "puppycam2.h"
#include "puppyprint.h"
#include "src/sprite/mdraw.h"
#include "src/sprite/sprites.h"
#include "audio/external.h"

#include "config.h"

/* @file hud.c
 * This file implements HUD rendering and power meter animations.
 * That includes stars, lives, coins, camera status, power meter, timer
 * cannon reticle, and the unused keys.
 **/

#define HUD_POWER_METER_X            140
#define HUD_POWER_METER_EMPHASIZED_Y 166
#define HUD_POWER_METER_Y            200
#define HUD_POWER_METER_HIDDEN_Y     300

#ifdef BREATH_METER
// #ifdef DISABLE_LIVES
// #define HUD_BREATH_METER_X         64
// #define HUD_BREATH_METER_Y        200
// #define HUD_BREATH_METER_HIDDEN_Y 300
// #else
#define HUD_BREATH_METER_X         40
#define HUD_BREATH_METER_Y         32
#define HUD_BREATH_METER_HIDDEN_Y -20
// #endif
#endif

// ------------- FPS COUNTER ---------------
// To use it, call print_fps(x,y); every frame.
#define FRAMETIME_COUNT 30

OSTime frameTimes[FRAMETIME_COUNT];
u8 curFrameTimeIndex = 0;
u8 selectedButtonID = 0;
s16 stickTimer = 0;
u8 curButtonMax = 0;
u8 menuID = 0;
s16 clickTimer = 0;
s16 cancelTimer = 0;
u8 clickButtonID = 0;
u8 clickMenuID = 0;
s8 sliderDir;
s8 lastAction = -1;
u16 highScore = 0;

#include "PR/os_convert.h"

// Call once per frame
f32 calculate_and_update_fps() {
    OSTime newTime = osGetTime();
    OSTime oldTime = frameTimes[curFrameTimeIndex];
    frameTimes[curFrameTimeIndex] = newTime;

    curFrameTimeIndex++;
    if (curFrameTimeIndex >= FRAMETIME_COUNT) {
        curFrameTimeIndex = 0;
    }
    return ((f32)FRAMETIME_COUNT * 1000000.0f) / (s32)OS_CYCLES_TO_USEC(newTime - oldTime);
}

void print_fps(s32 x, s32 y) {
    f32 fps = calculate_and_update_fps();
    char text[14];

    sprintf(text, "FPS %2.2f", fps);
#ifdef PUPPYPRINT
    print_small_text(x, y, text, PRINT_TEXT_ALIGN_LEFT, PRINT_ALL, FONT_OUTLINE);
#else
    print_text(x, y, text);
#endif

}

// ------------ END OF FPS COUNER -----------------

struct PowerMeterHUD {
    s8 animation;
    s16 x;
    s16 y;
};

struct CameraHUD {
    s16 status;
};

// Stores health segmented value defined by numHealthWedges
// When the HUD is rendered this value is 8, full health.
static s16 sPowerMeterStoredHealth;

static struct PowerMeterHUD sPowerMeterHUD = {
    POWER_METER_HIDDEN,
    HUD_POWER_METER_X,
    HUD_POWER_METER_HIDDEN_Y,
};

// Power Meter timer that keeps counting when it's visible.
// Gets reset when the health is filled and stops counting
// when the power meter is hidden.
s32 sPowerMeterVisibleTimer = 0;

#ifdef BREATH_METER
static s16 sBreathMeterStoredValue;
static struct PowerMeterHUD sBreathMeterHUD = {
    BREATH_METER_HIDDEN,
    HUD_BREATH_METER_X,
    HUD_BREATH_METER_HIDDEN_Y,
};
s32 sBreathMeterVisibleTimer = 0;
#endif

static struct CameraHUD sCameraHUD = { CAM_STATUS_NONE };

/**
 * Renders a rgba16 16x16 glyph texture from a table list.
 */
void render_hud_tex_lut(s32 x, s32 y, Texture *texture) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture);
    gSPDisplayList(gDisplayListHead++, &dl_hud_img_load_tex_block);
    gSPTextureRectangle(gDisplayListHead++, x << 2, y << 2, (x + 15) << 2, (y + 15) << 2,
                        G_TX_RENDERTILE, 0, 0, 4 << 10, 1 << 10);
}

/**
 * Renders a rgba16 8x8 glyph texture from a table list.
 */
void render_hud_small_tex_lut(s32 x, s32 y, Texture *texture) {
    gDPSetTile(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0,
                G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD);
    gDPTileSync(gDisplayListHead++);
    gDPSetTile(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 2, 0, G_TX_RENDERTILE, 0,
                G_TX_CLAMP, 3, G_TX_NOLOD, G_TX_CLAMP, 3, G_TX_NOLOD);
    gDPSetTileSize(gDisplayListHead++, G_TX_RENDERTILE, 0, 0, (8 - 1) << G_TEXTURE_IMAGE_FRAC, (8 - 1) << G_TEXTURE_IMAGE_FRAC);
    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture);
    gDPLoadSync(gDisplayListHead++);
    gDPLoadBlock(gDisplayListHead++, G_TX_LOADTILE, 0, 0, 8 * 8 - 1, CALC_DXT(8, G_IM_SIZ_16b_BYTES));
    gSPTextureRectangle(gDisplayListHead++, x << 2, y << 2, (x + 7) << 2, (y + 7) << 2, G_TX_RENDERTILE,
                        0, 0, 4 << 10, 1 << 10);
}

/**
 * Renders power meter health segment texture using a table list.
 */
void render_power_meter_health_segment(s16 numHealthWedges) {
    Texture *(*healthLUT)[] = segmented_to_virtual(&power_meter_health_segments_lut);

    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1,
                       (*healthLUT)[numHealthWedges - 1]);
    gDPLoadSync(gDisplayListHead++);
    gDPLoadBlock(gDisplayListHead++, G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES));
    gSP1Triangle(gDisplayListHead++, 0, 1, 2, 0);
    gSP1Triangle(gDisplayListHead++, 0, 2, 3, 0);
}

/**
 * Renders power meter display lists.
 * That includes the "POWER" base and the colored health segment textures.
 */
void render_dl_power_meter(s16 numHealthWedges) {
    Mtx *mtx = alloc_display_list(sizeof(Mtx));

    if (mtx == NULL) {
        return;
    }

    guTranslate(mtx, (f32) sPowerMeterHUD.x, (f32) sPowerMeterHUD.y, 0);

    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, &dl_power_meter_base);

    if (numHealthWedges != 0) {
        gSPDisplayList(gDisplayListHead++, &dl_power_meter_health_segments_begin);
        render_power_meter_health_segment(numHealthWedges);
        gSPDisplayList(gDisplayListHead++, &dl_power_meter_health_segments_end);
    }

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/**
 * Power meter animation called when there's less than 8 health segments
 * Checks its timer to later change into deemphasizing mode.
 */
void animate_power_meter_emphasized(void) {
    s16 hudDisplayFlags = gHudDisplay.flags;

    if (!(hudDisplayFlags & HUD_DISPLAY_FLAG_EMPHASIZE_POWER)) {
        if (sPowerMeterVisibleTimer == 45.0f) {
            sPowerMeterHUD.animation = POWER_METER_DEEMPHASIZING;
        }
    } else {
        sPowerMeterVisibleTimer = 0;
    }
}

/**
 * Power meter animation called after emphasized mode.
 * Moves power meter y pos speed until it's at 200 to be visible.
 */
static void animate_power_meter_deemphasizing(void) {
    s16 speed = 5;

    if (sPowerMeterHUD.y > HUD_POWER_METER_Y - 20) speed = 3;
    if (sPowerMeterHUD.y > HUD_POWER_METER_Y - 10) speed = 2;
    if (sPowerMeterHUD.y > HUD_POWER_METER_Y -  5) speed = 1;

    sPowerMeterHUD.y += speed;

    if (sPowerMeterHUD.y > HUD_POWER_METER_Y) {
        sPowerMeterHUD.y = HUD_POWER_METER_Y;
        sPowerMeterHUD.animation = POWER_METER_VISIBLE;
    }
}

/**
 * Power meter animation called when there's 8 health segments.
 * Moves power meter y pos quickly until it's at 301 to be hidden.
 */
static void animate_power_meter_hiding(void) {
    sPowerMeterHUD.y += 20;
    if (sPowerMeterHUD.y > HUD_POWER_METER_HIDDEN_Y) {
        sPowerMeterHUD.animation = POWER_METER_HIDDEN;
        sPowerMeterVisibleTimer = 0;
    }
}

/**
 * Handles power meter actions depending of the health segments values.
 */
void handle_power_meter_actions(s16 numHealthWedges) {
    // Show power meter if health is not full, less than 8
    if (numHealthWedges < 8 && sPowerMeterStoredHealth == 8
        && sPowerMeterHUD.animation == POWER_METER_HIDDEN) {
        sPowerMeterHUD.animation = POWER_METER_EMPHASIZED;
        sPowerMeterHUD.y = HUD_POWER_METER_EMPHASIZED_Y;
    }

    // Show power meter if health is full, has 8
    if (numHealthWedges == 8 && sPowerMeterStoredHealth == 7) {
        sPowerMeterVisibleTimer = 0;
    }

    // After health is full, hide power meter
    if (numHealthWedges == 8 && sPowerMeterVisibleTimer > 45.0f) {
        sPowerMeterHUD.animation = POWER_METER_HIDING;
    }

    // Update to match health value
    sPowerMeterStoredHealth = numHealthWedges;

#ifndef BREATH_METER
    // If Mario is swimming, keep power meter visible
    if (gPlayerCameraState->action & ACT_FLAG_SWIMMING) {
        if (sPowerMeterHUD.animation == POWER_METER_HIDDEN
            || sPowerMeterHUD.animation == POWER_METER_EMPHASIZED) {
            sPowerMeterHUD.animation = POWER_METER_DEEMPHASIZING;
            sPowerMeterHUD.y = HUD_POWER_METER_EMPHASIZED_Y;
        }
        sPowerMeterVisibleTimer = 0;
    }
#endif
}

/**
 * Renders the power meter that shows when Mario is in underwater
 * or has taken damage and has less than 8 health segments.
 * And calls a power meter animation function depending of the value defined.
 */
void render_hud_power_meter(void) {
    s16 shownHealthWedges = gHudDisplay.wedges;
    if (sPowerMeterHUD.animation != POWER_METER_HIDING) handle_power_meter_actions(shownHealthWedges);
    if (sPowerMeterHUD.animation == POWER_METER_HIDDEN) return;
    switch (sPowerMeterHUD.animation) {
        case POWER_METER_EMPHASIZED:    animate_power_meter_emphasized();    break;
        case POWER_METER_DEEMPHASIZING: animate_power_meter_deemphasizing(); break;
        case POWER_METER_HIDING:        animate_power_meter_hiding();        break;
        default:                                                             break;
    }
    render_dl_power_meter(shownHealthWedges);
    sPowerMeterVisibleTimer++;
}

#ifdef BREATH_METER
/**
 * Renders breath meter health segment texture using a table list.
 */
void render_breath_meter_segment(s16 numBreathWedges) {
    Texture *(*breathLUT)[];
    breathLUT = segmented_to_virtual(&breath_meter_segments_lut);
    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, (*breathLUT)[numBreathWedges - 1]);
    gDPLoadSync(gDisplayListHead++);
    gDPLoadBlock(gDisplayListHead++, G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES));
    gSP1Triangle(gDisplayListHead++, 0, 1, 2, 0);
    gSP1Triangle(gDisplayListHead++, 0, 2, 3, 0);
}

/**
 * Renders breath meter display lists.
 * That includes the base and the colored segment textures.
 */
void render_dl_breath_meter(s16 numBreathWedges) {
    Mtx *mtx = alloc_display_list(sizeof(Mtx));

    if (mtx == NULL) {
        return;
    }

    guTranslate(mtx, (f32) sBreathMeterHUD.x, (f32) sBreathMeterHUD.y, 0);
    gSPMatrix(      gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++),
                    G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList( gDisplayListHead++, &dl_breath_meter_base);
    if (numBreathWedges != 0) {
        gSPDisplayList(gDisplayListHead++, &dl_breath_meter_health_segments_begin);
        render_breath_meter_segment(numBreathWedges);
        gSPDisplayList(gDisplayListHead++, &dl_breath_meter_health_segments_end);
    }
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/**
 * Breath meter animation called after emphasized mode.
 * Moves breath meter y pos speed until it's visible.
 */
static void animate_breath_meter_sliding_in(void) {
    approach_s16_symmetric_bool(&sBreathMeterHUD.y, HUD_BREATH_METER_Y, 5);
    if (sBreathMeterHUD.y         == HUD_BREATH_METER_Y) {
        sBreathMeterHUD.animation = BREATH_METER_VISIBLE;
    }
}

/**
 * Breath meter animation called when there's 8 health segments.
 * Moves breath meter y pos quickly until it's hidden.
 */
static void animate_breath_meter_sliding_out(void) {
    approach_s16_symmetric_bool(&sBreathMeterHUD.y, HUD_BREATH_METER_HIDDEN_Y, 20);
    if (sBreathMeterHUD.y         == HUD_BREATH_METER_HIDDEN_Y) {
        sBreathMeterHUD.animation = BREATH_METER_HIDDEN;
    }
}

/**
 * Handles breath meter actions depending of the health segments values.
 */
void handle_breath_meter_actions(s16 numBreathWedges) {
    // Show breath meter if health is not full, less than 8
    if ((numBreathWedges < 8) && (sBreathMeterStoredValue == 8) && sBreathMeterHUD.animation == BREATH_METER_HIDDEN) {
        sBreathMeterHUD.animation = BREATH_METER_SHOWING;
        // sBreathMeterHUD.y         = HUD_BREATH_METER_Y;
    }
    // Show breath meter if breath is full, has 8
    if ((numBreathWedges == 8) && (sBreathMeterStoredValue  == 7)) sBreathMeterVisibleTimer  = 0;
    // After breath is full, hide breath meter
    if ((numBreathWedges == 8) && (sBreathMeterVisibleTimer > 45)) sBreathMeterHUD.animation = BREATH_METER_HIDING;
    // Update to match breath value
    sBreathMeterStoredValue = numBreathWedges;
    // If Mario is swimming, keep breath meter visible
    if (gPlayerCameraState->action & ACT_FLAG_SWIMMING) {
        if (sBreathMeterHUD.animation == BREATH_METER_HIDDEN) {
            sBreathMeterHUD.animation = BREATH_METER_SHOWING;
        }
        sBreathMeterVisibleTimer = 0;
    }
}

void render_hud_breath_meter(void) {
    s16 shownBreathAmount = gHudDisplay.breath;
    if (sBreathMeterHUD.animation != BREATH_METER_HIDING) handle_breath_meter_actions(shownBreathAmount);
    if (sBreathMeterHUD.animation == BREATH_METER_HIDDEN) return;
    switch (sBreathMeterHUD.animation) {
        case BREATH_METER_SHOWING:       animate_breath_meter_sliding_in();  break;
        case BREATH_METER_HIDING:        animate_breath_meter_sliding_out(); break;
        default:                                                             break;
    }
    render_dl_breath_meter(shownBreathAmount);
    sBreathMeterVisibleTimer++;
}
#endif

#define HUD_TOP_Y 209

/**
 * Renders the amount of lives Mario has.
 */
void render_hud_mario_lives(void) {
    print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y, ","); // 'Mario Head' glyph
    print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(38), HUD_TOP_Y, "*"); // 'X' glyph
    print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(54), HUD_TOP_Y, "%d", gHudDisplay.lives);
}

#ifdef VANILLA_STYLE_CUSTOM_DEBUG
void render_debug_mode(void) {
    print_text(180, 40, "DEBUG MODE");
    print_text_fmt_int(5, 20, "Z %d", gMarioState->pos[2]);
    print_text_fmt_int(5, 40, "Y %d", gMarioState->pos[1]);
    print_text_fmt_int(5, 60, "X %d", gMarioState->pos[0]);
    print_text_fmt_int(10, 100, "SPD %d", (s32) gMarioState->forwardVel);
    print_text_fmt_int(10, 120, "ANG 0*%04x", (u16) gMarioState->faceAngle[1]);
    print_fps(10,80);
}
#endif

#define HUD_COINS_X 78

/**
 * Renders the amount of coins collected.
 */
void render_hud_coins(void) {
    print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y, "$"); // 'Mario Head' glyph
    print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(38), HUD_TOP_Y, "*"); // 'X' glyph
    print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(54), HUD_TOP_Y, "%d", gHudDisplay.coins);
}

#define HUD_STARS_X 78

/**
 * Renders the amount of stars collected.
 * Disables "X" glyph when Mario has 100 stars or more.
 */
void render_hud_stars(void) {
    if (gHudFlash == HUD_FLASH_STARS && gGlobalTimer & 0x8) return;
    s8 showX = (gHudDisplay.stars < 100);
    print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X), HUD_TOP_Y, "^"); // 'Star' glyph
    if (showX) print_text((GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X) + 16), HUD_TOP_Y, "*"); // 'X' glyph
    print_text_fmt_int((showX * 14) + GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X - 16),
                       HUD_TOP_Y, "%d", gHudDisplay.stars);
}

/**
 * Unused function that renders the amount of keys collected.
 * Leftover function from the beta version of the game.
 */
void render_hud_keys(void) {
    s16 i;

    for (i = 0; i < gHudDisplay.keys; i++) {
        print_text((i * 16) + 220, 142, "|"); // unused glyph - beta key
    }
}

/**
 * Renders the timer when Mario start sliding in PSS.
 */
void render_hud_timer(void) {
    Texture *(*hudLUT)[58] = segmented_to_virtual(&main_hud_lut);
    u16 timerValFrames = gHudDisplay.timer;
    u16 timerMins = timerValFrames / (30 * 60);
    u16 timerSecs = (timerValFrames - (timerMins * 1800)) / 30;
    u16 timerFracSecs = ((timerValFrames - (timerMins * 1800) - (timerSecs * 30)) & 0xFFFF) / 3;

#if MULTILANG
    switch (eu_get_language()) {
        case LANGUAGE_ENGLISH: print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(150), 185,  "TIME"); break;
        case LANGUAGE_FRENCH:  print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(155), 185, "TEMPS"); break;
        case LANGUAGE_GERMAN:  print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(150), 185,  "ZEIT"); break;
    }
#else
    print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(150), 185, "TIME");
#endif

    print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(91), 185, "%0d", timerMins);
    print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(71), 185, "%02d", timerSecs);
    print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(37), 185, "%d", timerFracSecs);

    gSPDisplayList(gDisplayListHead++, dl_hud_img_begin);
    render_hud_tex_lut(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(81), 32, (*hudLUT)[GLYPH_APOSTROPHE]);
    render_hud_tex_lut(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(46), 32, (*hudLUT)[GLYPH_DOUBLE_QUOTE]);
    gSPDisplayList(gDisplayListHead++, dl_hud_img_end);
}

/**
 * Sets HUD status camera value depending of the actions
 * defined in update_camera_status.
 */
void set_hud_camera_status(s16 status) {
    sCameraHUD.status = status;
}

/**
 * Renders camera HUD glyphs using a table list, depending of
 * the camera status called, a defined glyph is rendered.
 */
void render_hud_camera_status(void) {
    Texture *(*cameraLUT)[6] = segmented_to_virtual(&main_hud_camera_lut);
    s32 x = GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(54);
    s32 y = 205;

    if (sCameraHUD.status == CAM_STATUS_NONE) {
        return;
    }

    gSPDisplayList(gDisplayListHead++, dl_hud_img_begin);
    render_hud_tex_lut(x, y, (*cameraLUT)[GLYPH_CAM_CAMERA]);

    switch (sCameraHUD.status & CAM_STATUS_MODE_GROUP) {
        case CAM_STATUS_MARIO:
            render_hud_tex_lut(x + 16, y, (*cameraLUT)[GLYPH_CAM_MARIO_HEAD]);
            break;
        case CAM_STATUS_LAKITU:
            render_hud_tex_lut(x + 16, y, (*cameraLUT)[GLYPH_CAM_LAKITU_HEAD]);
            break;
        case CAM_STATUS_FIXED:
            render_hud_tex_lut(x + 16, y, (*cameraLUT)[GLYPH_CAM_FIXED]);
            break;
    }

    switch (sCameraHUD.status & CAM_STATUS_C_MODE_GROUP) {
        case CAM_STATUS_C_DOWN:
            render_hud_small_tex_lut(x + 4, y + 16, (*cameraLUT)[GLYPH_CAM_ARROW_DOWN]);
            break;
        case CAM_STATUS_C_UP:
            render_hud_small_tex_lut(x + 4, y - 8, (*cameraLUT)[GLYPH_CAM_ARROW_UP]);
            break;
    }

    gSPDisplayList(gDisplayListHead++, dl_hud_img_end);
}

/**
 * Handles any button inputs on the menu
 */
void handle_button_input(void) {
    // If scoring a file, pressing A just changes the coin score mode.
    if (gPlayer3Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
        clickTimer = 1;
        clickMenuID = menuID;
        clickButtonID = selectedButtonID;
    }
    if (gPlayer3Controller->buttonPressed & B_BUTTON) {
        cancelTimer = 1;
    }
}

/**
 * Handles any directional movement (joystick, and also d-pad)
 */
void handle_stick_input(void) {
    s16 dirX = gPlayer3Controller->rawStickX;
    s16 dirY = gPlayer3Controller->rawStickY;

    // Handle deadzone
    if (dirY > -48 && dirY < 48) {
        dirY = 0;
    }
    if (dirX > -48 && dirX < 48) {
        dirX = 0;
    }

    if (dirY != 0) {
        dirX = 0;
    }
    if (dirX != 0) {
        dirY = 0;
    }

    if (gPlayer3Controller->buttonDown & L_JPAD) dirX = -64;
    else if (gPlayer3Controller->buttonDown & R_JPAD) dirX = 64;
    if (gPlayer3Controller->buttonDown & U_JPAD) dirY = 64;
    else if (gPlayer3Controller->buttonDown & D_JPAD) dirY = -64;

    if (stickTimer == 0 && curButtonMax > 0) {
        sliderDir = dirX;

        if (dirY > 0) {
            if (selectedButtonID == 0) {
                selectedButtonID = curButtonMax;
            }
            else {
                selectedButtonID--;
            }
            stickTimer = 5;
        }
        else if (dirY < 0) {
            if (selectedButtonID == curButtonMax) {
                selectedButtonID = 0;
            }
            else {
                selectedButtonID++;
            }
            stickTimer = 5;
        }

        if (stickTimer == 5)
        {
            play_sound(SOUND_ACTION_BRUSH_HAIR, gGlobalSoundSource);
        }
    }
    else if (stickTimer > 0)
    {
        stickTimer--;
    }
}

/**
 * Handles all menu inputs
 */
void handle_input(void) {
    if (gMarioState->gameAction != 1)
        handle_stick_input();
    if (clickTimer == 0 && gMarioState->gameAction != 1) {
        handle_button_input();
    }
    else {
        clickTimer++; // This is a very strange way to implement a timer? It counts up and
                                // then resets to 0 instead of just counting down to 0.
        if (clickTimer == 5) {
            clickTimer = 0;
            clickButtonID = 0;
            clickMenuID = 0;
        }
    }
    if (cancelTimer != 0) {
        cancelTimer++; // This is a very strange way to implement a timer? It counts up and
                                // then resets to 0 instead of just counting down to 0.
        if (cancelTimer == 5) {
            cancelTimer = 0;
        }
    }
}

/**
 * Render HUD strings using hudDisplayFlags with it's render functions,
 * excluding the cannon reticle which detects a camera preset for it.
 */
void render_hud(void) {
    s16 hudDisplayFlags = gHudDisplay.flags;
    handle_input();

    if (hudDisplayFlags != HUD_DISPLAY_NONE) {
        int i = 0;
        create_dl_ortho_matrix();

        if (lastAction != gMarioState->gameAction) {
            highScore = save_file_get_high_score();
            lastAction = gMarioState->gameAction;
        }
        switch (gMarioState->gameAction) {
            case 0:
                {
                    switch (menuID) {
                        case 0: {
                            curButtonMax = 1;
                            if (highScore > 0) {
                                char highScoreText[0x10];
                                sprintf(highScoreText, "High Score: %d", highScore);
                                draw_rounded_box_ws_center(640, 404, 1320, 96, 32, 34, 26, 113, 0xFF);
                                mprint(640, 420, 64, -1, MPRINT_CJUST, 255, 255, 255, 255, highScoreText);
                            }
                            for (i = 0; i < 2; i++) {
                                u8 a = 64;
                                s32 selectOffset = 0;
                                if (selectedButtonID == i) {
                                    a += ((5 - stickTimer) * 191) / 5;
                                    selectOffset = ((5 - stickTimer) * 16) / 5;
                                }
                                switch (i) {
                                    case 0:
                                        draw_rounded_box_ws_center(640 + selectOffset, 532, 448, 120, 32, 34, 26, 113, 0xFF);
                                        mprint(640 + selectOffset, 548, 80, -1, MPRINT_CJUST, 255, 255, 255, a, "Play");
                                        break;
                                    case 1:
                                        draw_rounded_box_ws_center(640 + selectOffset, 692, 448, 120, 32, 34, 26, 113, 0xFF);
                                        mprint(640 + selectOffset, 708, 80, -1, MPRINT_CJUST, 255, 255, 255, a, "Credits");
                                        break;
                                }
                            }
                            mprint(1248, 908, 40, -1, MPRINT_RJUST, 255, 255, 255, 255, "axollyon 2022");
                            if (clickTimer == 2) {
                                switch (selectedButtonID) {
                                    case 0:
                                        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
                                        gMarioState->startRun = TRUE;
                                        break;
                                    case 1:
                                        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
                                        menuID = 1;
                                        break;
                                }
                            }
                            break;
                        }
                        case 1: {
                            curButtonMax = 0;
                            draw_rounded_box_ws_center(640, 0, 1320, 960, 32, 34, 26, 113, 0xFF);
                            mprint(640, 32, 80, -1, MPRINT_CJUST, 255, 255, 255, 255, "Bullet Bill Simulator\nby axollyon");
                            mprint(640, 220, 48, -1, MPRINT_CJUST, 255, 255, 255, 255, "Tools: HackerSM64, Fast64, STRM64, msprite");
                            mprint(640, 300, 48, -1, MPRINT_CJUST, 255, 255, 255, 255, "Music:\nMain Menu (Ver. 1) from NSMB Wii\nNSMB Overworld Theme (2020 Remix) by Mutty99");
                            mprint(640, 470, 48, -1, MPRINT_CJUST, 255, 255, 255, 255, "Models:\nBullet Bill from Mario Kart Arcade GP 2");
                            mprint(640, 595, 48, -1, MPRINT_CJUST, 255, 255, 255, 255, "Textures:\nSMB1 Ground Texture by cloud6625\nSMG2 HD Grass Texture by razius\nHard Block Texture from NSMBU");
                            mprint(640, 810, 48, -1, MPRINT_CJUST, 255, 255, 255, 255, "Code Help: Wiseguy, strat\nPlaytesters: anonymous_moose, SpK");
                            if (clickTimer == 2 || cancelTimer == 2) {
                                play_sound(SOUND_MENU_CLICK_CHANGE_VIEW, gGlobalSoundSource);
                                menuID = 0;
                            }
                            break;
                        }
                    }
                }
                break;
            case 1:
                {
                    char coins[0x10];
                    clickTimer = 0;
                    cancelTimer = 0;
                    sprintf(coins, "$x%d", gMarioState->numCoins);
                    draw_rounded_box_ws_center(0, 20, 880, 80, 32, 34, 26, 113, 0xFF);
                    mprint(220, 36, 50, -1, MPRINT_CJUST, 255, 255, 255, 255, coins);
                    draw_rounded_box_ws_center(1280, 20, 880, 80, 32, 34, 26, 113, 0xFF);
                    if (gMarioState->numCoins < 50)
                        mprint(1060, 36, 50, -1, MPRINT_CJUST, 255, 255, 255, 255, "No Stars");
                    else if (gMarioState->numCoins < 100)
                        mprint(1060, 36, 50, -1, MPRINT_CJUST, 255, 255, 255, 255, "Bronze `");
                    else if (gMarioState->numCoins < 250)
                        mprint(1060, 36, 50, -1, MPRINT_CJUST, 255, 255, 255, 255, "Silver ``");
                    else if (gMarioState->numCoins < 500)
                        mprint(1060, 36, 50, -1, MPRINT_CJUST, 255, 255, 255, 255, "Gold ```");
                    else
                        mprint(1060, 36, 50, -1, MPRINT_CJUST, 255, 255, 255, 255, "Platinum ````");
                }
                break;
            case 2:
                {
                    char scoreText[0x30];
                    char rank[0x10];
                    curButtonMax = 1;

                    if (gMarioState->numCoins < 50) 
                        sprintf(rank, "%s", "No Stars");
                    else if (gMarioState->numCoins < 100)
                        sprintf(rank, "%s", "Bronze `");
                    else if (gMarioState->numCoins < 250)
                        sprintf(rank, "%s", "Silver ``");
                    else if (gMarioState->numCoins < 500)
                        sprintf(rank, "%s", "Gold ```");
                    else
                        sprintf(rank, "%s", "Platinum ````");

                    if (gMarioState->newHigh == TRUE) {
                        sprintf(scoreText, "%s\nScore: %d\nNew High Score!", rank, gMarioState->numCoins);
                    }
                    else {
                        sprintf(scoreText, "%s\nScore: %d\nHigh Score: %d", rank, gMarioState->numCoins, highScore);
                    }
                    draw_rounded_box_ws_center(640, 152, 1320, 224, 32, 34, 26, 113, 0xFF);
                    mprint(640, 168, 64, -1, MPRINT_CJUST, 255, 255, 255, 255, scoreText);
                    for (i = 0; i < 2; i++) {
                        u8 a = 64;
                        s32 selectOffset = 0;
                        if (selectedButtonID == i) {
                            a += ((5 - stickTimer) * 191) / 5;
                            selectOffset = ((5 - stickTimer) * 16) / 5;
                        }
                        switch (i) {
                            case 0:
                                draw_rounded_box_ws_center(640 + selectOffset, 532, 448, 120, 32, 34, 26, 113, 0xFF);
                                mprint(640 + selectOffset, 548, 80, -1, MPRINT_CJUST, 255, 255, 255, a, "Restart");
                                break;
                            case 1:
                                draw_rounded_box_ws_center(640 + selectOffset, 692, 448, 120, 32, 34, 26, 113, 0xFF);
                                mprint(640 + selectOffset, 708, 80, -1, MPRINT_CJUST, 255, 255, 255, a, "To Menu");
                                break;
                        }
                    }
                    if (clickTimer == 2) {
                        switch (selectedButtonID) {
                            case 0:
                                play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
                                gMarioState->startRun = TRUE;
                                break;
                            case 1:
                                play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
                                gMarioState->gameAction = 0;
                                break;
                        }
                    }
                }
                break;
        }

        if (gSurfacePoolError & NOT_ENOUGH_ROOM_FOR_SURFACES) print_text(10, 40, "SURFACE POOL FULL");
        if (gSurfacePoolError & NOT_ENOUGH_ROOM_FOR_NODES) print_text(10, 60, "SURFACE NODE POOL FULL");

#ifdef VANILLA_STYLE_CUSTOM_DEBUG
        if (gCustomDebugMode) {
            render_debug_mode();
        }
#endif
#ifdef PUPPYPRINT
        print_set_envcolour(255, 255, 255, 255);
#endif
    }
}
