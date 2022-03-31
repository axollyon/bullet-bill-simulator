/******************************************************************************
 *                    mdraw.c: Drawing code by devwizard.                     *
 ******************************************************************************/

#ifndef __MDRAW_H__
#define __MDRAW_H__

#include <ultra64.h>

#ifndef __ASSEMBLER__

/* Printing justification */
#define MPRINT_LJUST    0
#define MPRINT_RJUST    1
#define MPRINT_CJUST    2

typedef unsigned int uint;

/* MSprite texture info */
typedef struct msprite
{
    const void *timg;           /* pointer to the texture data */
    u32 fmt:4;                  /* texture format number */
    u32 siz:4;                  /* texture size number */
    u32 w:12;                   /* width of the texture */
    u32 h:12;                   /* height of the texture */
}
MSPRITE;

/* Filtering */
extern void mfilter(uint filter);

/* Drawing sprite */
extern void msprite(const MSPRITE *msp, int x, int y, uint w, uint h);

/* Printing */
extern void mprint_start(void);
extern void mprint(int x, int y, uint s, uint max, uint just, u8 r, u8 g, u8 b, u8 a, const char *str);

#define MCHAR_ABUTTON     '\x80'
#define MCHAR_BBUTTON     '\x81'
#define MCHAR_LRARROW     '\x82'

#define MSTR_ABUTTON      "\x80"
#define MSTR_BBUTTON      "\x81"
#define MSTR_LRARROW      "\x82"

#endif /* __ASSEMBLER__ */

#endif /* __MDRAW_H__ */
