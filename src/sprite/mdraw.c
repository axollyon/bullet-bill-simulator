/******************************************************************************
 *                    mdraw.c: Drawing code by devwizard.                     *
 ******************************************************************************/

#include <ultra64.h>

#include "mdraw.h"
#include "game/main.h"
#include "game/game_init.h"
#include "sprites.h"

extern void *segment_to_virtual(const void *);
extern Gfx *video_gfx;

static struct {
    uint filter;
}
mdraw;

void mfilter(uint filter) {
    mdraw.filter = filter;
    gDPSetTextureFilter(video_gfx++, mdraw.filter ? G_TF_BILERP : G_TF_POINT);
}

/******************************************************************************
 *                               Drawing sprite                               *
 ******************************************************************************/

static const u8 msp_size[][2] = {
    {128, 64},
    { 64, 64},
    { 64, 32},
    { 32, 32},
};

void msprite(const MSPRITE *msp, int x, int y, uint w, uint h) {
    uint fmt = msp->fmt;
    uint siz = msp->siz;
    uint tw  = msp->w;
    uint th  = msp->h;
    uint cw;
    uint ch;
    uint dsdx;
    uint dtdy;
    uint ty;
    cw = msp_size[siz][0];
    ch = msp_size[siz][1];
    if (mdraw.filter)
    {
        cw /= 2;
    }
    dsdx = (0x400*4*tw + w/2) / w;
    dtdy = (0x400*4*th + h/2) / h;
    gDPSetTextureImage(video_gfx++, fmt, siz, tw, msp->timg);
    for (ty = 0; ty < th; ty += ch)
    {
        uint ult;
        uint lrt;
        int yl;
        int yh;
        int t;
        uint tx;
        ult = ty;
        lrt = ty + ch;
        if (lrt > th)
        {
            lrt = th;
        }
        yl = y + ult*h/th;
        yh = y + lrt*h/th;
        if (yl > 0x1000 || yh < 0)
        {
            continue;
        }
        t = ult << 5;
        if (yl < 0)
        {
            t -= (yl*dtdy) >> 7;
            yl = 0;
        }
        if (mdraw.filter)
        {
            if (ult > 0)
            {
                ult--;
            }
            if (lrt < th)
            {
                lrt++;
            }
            t -= 16;
        }
        for (tx = 0; tx < tw; tx += cw)
        {
            uint uls;
            uint lrs;
            int xl;
            int xh;
            int s;
            uint line;
            uls = tx;
            lrs = tx + cw;
            if (lrs > tw)
            {
                lrs = tw;
            }
            xl = x + uls*w/tw;
            xh = x + lrs*w/tw;
            if (xl > 0x1000 || xh < 0)
            {
                continue;
            }
            s = uls << 5;
            if (xl < 0)
            {
                s -= (xl*dsdx) >> 7;
                xl = 0;
            }
            if (mdraw.filter)
            {
                if (uls > 0)
                {
                    uls--;
                }
                if (lrs < tw)
                {
                    lrs++;
                }
                s -= 16;
            }
            line = (((lrs-uls) << siz >> 1) + 7) >> 3;
            gDPSetTile(
                video_gfx++, fmt, siz, line, 0, G_TX_LOADTILE, 0,
                G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD,
                G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD
            );
            gDPLoadSync(video_gfx++);
            gDPLoadTile(
                video_gfx++, G_TX_LOADTILE,
                (uls  ) << G_TEXTURE_IMAGE_FRAC,
                (ult  ) << G_TEXTURE_IMAGE_FRAC,
                (lrs-1) << G_TEXTURE_IMAGE_FRAC,
                (lrt-1) << G_TEXTURE_IMAGE_FRAC
            );
            gDPPipeSync(video_gfx++);
            gDPSetTile(
                video_gfx++, fmt, siz, line, 0, G_TX_RENDERTILE, 0,
                G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD,
                G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD
            );
            gDPSetTileSize(
                video_gfx++, G_TX_RENDERTILE,
                (uls  ) << G_TEXTURE_IMAGE_FRAC,
                (ult  ) << G_TEXTURE_IMAGE_FRAC,
                (lrs-1) << G_TEXTURE_IMAGE_FRAC,
                (lrt-1) << G_TEXTURE_IMAGE_FRAC
            );
            gSPTextureRectangle(
                video_gfx++, xl, yl, xh, yh, G_TX_RENDERTILE, s, t, dsdx, dtdy
            );
        }
    }
}

/******************************************************************************
 *                                  Printing                                  *
 ******************************************************************************/

extern const u8 *const texture_primary[];
extern const u8 primary_kern[];

static uint mprint_w(const u8 *kern, uint s, uint max, const char *str) {
    uint w = 0;
    while (max > 0 && *str != 0 && *str != '\n')
    {
        int c = *str;
        str++;
        max--;
        w += kern[c] * s/64;
    }
    return w;
}

void mprint_start(void) {
    gDPSetTile(
        video_gfx++, G_IM_FMT_I, G_IM_SIZ_16b,
        0, 0, G_TX_LOADTILE, 0,
        G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD,
        G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD
    );
    gDPSetTile(
        video_gfx++, G_IM_FMT_I, G_IM_SIZ_4b,
        ((64>>1)+7) >> 3, 0, G_TX_RENDERTILE, 0,
        G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD,
        G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD
    );
    gDPSetTileSize(
        video_gfx++, G_TX_RENDERTILE, 0, 0,
        (64-1) << G_TEXTURE_IMAGE_FRAC,
        (64-1) << G_TEXTURE_IMAGE_FRAC
    );
}

void mprint(int x, int y, uint s, uint max, uint just, u8 r, u8 g, u8 b, u8 a, const char *str) {
    const u8 *const *table = segment_to_virtual(texture_primary);
    const u8 *kern         = segment_to_virtual(primary_kern);
    uint w = gConfig.widescreen ? (uint)((f32)s * 0.75f) : s;
    uint h = s;
    int st = mdraw.filter ? -16 : 0;
    int dtx = (0x400*4*64 + w/2) / w;
    int dty = (0x400*4*64 + h/2) / h;
    int xl = x;
    text_init(r, g, b, a);
start:
    if (just > 0) {
        x -= mprint_w(kern, w, max, str) / just;
    }
    while (max > 0 && *str != 0) {
        int c = *str;
        str++;
        max--;
        switch (c) {
            case '\n': 
                x = xl;
                y += kern['\n'] * h/64;
                goto start;
                break;
            case MCHAR_ABUTTON:
            case MCHAR_BBUTTON:
                {
                    u8 buttonR, buttonG, buttonB, letterR, letterG, letterB;
                    char letter[2];
                    switch (c) {
                        case MCHAR_ABUTTON:
                            buttonR = 0x42; buttonG = 0x8D; buttonB = 0xFF;
                            letterR = 0x12; letterG = 0x31; letterB = 0x5F;
                            sprintf(letter, "%s", "A");
                            break;
                        case MCHAR_BBUTTON:
                            buttonR = 0x42; buttonG = 0xFF; buttonB = 0x8D;
                            letterR = 0x12; letterG = 0x5F; letterB = 0x31;
                            sprintf(letter, "%s", "B");
                            break;
                    }
                    draw_button(x, y, w, h, buttonR, buttonG, buttonB, a);
                    mprint(x + (int)(26.67f * w/64), y - 3 * w/64, s, -1, MPRINT_CJUST, letterR, letterG, letterB, a, letter);
                    text_init(r, g, b, a);
                    x += 54 * w/64;
                }
                break;
            case MCHAR_LRARROW:
                mprint(x, y, s, -1, MPRINT_LJUST, r, g, b, a, "<");
                mprint(x + 7 * w/64, y, s, -1, MPRINT_LJUST, r, g, b, a, "-");
                mprint(x + 48 * w/64, y, s, -1, MPRINT_RJUST, r, g, b, a, "-");
                mprint(x + 54 * w/64, y, s, -1, MPRINT_RJUST, r, g, b, a, ">");
                text_init(r, g, b, a);
                x += 54 * w/64;
                break;
            default:
                {
                    if (c == 'j') {
                        x -= w/8;
                    }
                    const u8 *timg = table[c];
                    if (timg != NULL) {
                        gDPSetTextureImage(
                            video_gfx++, G_IM_FMT_I, G_IM_SIZ_16b, 1, timg
                        );
                        gDPLoadSync(video_gfx++);
                        gDPLoadBlock(
                            video_gfx++, G_TX_LOADTILE, 0, 0,
                            ((64*64+G_IM_SIZ_4b_INCR) >> G_IM_SIZ_4b_SHIFT)-1,
                            CALC_DXT_4b(64)
                        );
                        gSPTextureRectangle(
                            video_gfx++, x, y, x+w, y+h, G_TX_RENDERTILE, st, st, dtx, dty
                        );
                        gDPPipeSync(video_gfx++);
                    }
                    x += kern[c] * w/64;
                }
                break;
        }
    }
    gDPSetTexturePersp(video_gfx++, G_TP_PERSP);
}
