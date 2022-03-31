#include <PR/ultratypes.h>
#include "types.h"

// Enum Definitions

enum PatternIds {
    PATTERN_A,
    PATTERN_B,
    PATTERN_C,
    PATTERN_D,
    PATTERN_E,
    PATTERN_F,
    PATTERN_G,
    PATTERN_H,
    PATTERN_I,
    PATTERN_J,
    PATTERN_K,
    PATTERN_L,
    PATTERN_M,
    PATTERN_N,
    PATTERN_O,
    PATTERN_P,
    PATTERN_Q,
    PATTERN_R,
    PATTERN_S,
    PATTERN_T,
    PATTERN_U,
    PATTERN_V,
    PATTERN_W,
    PATTERN_X,
    PATTERN_Y,
    PATTERN_Z,
    PATTERN_TERMINATOR = 0xFF
};

enum ObstacleIds {
    OBSTACLE_BLOCK_1,
    OBSTACLE_BLOCK_2,
    OBSTACLE_BLOCK_3,
    OBSTACLE_BLOCK_4,
    OBSTACLE_COIN,
    OBSTACLE_GOOMBA,
    OBSTACLE_TERMINATOR = 0xFF
};


// Struct Definitions

struct Obstacle {
    u8 id;
    Vec3f pos;
};

struct ObstaclePattern {
    u8 tileLength;
    struct Obstacle *obstacles;
    u8 *nextBlacklist;
};


// Pattern Definitions

struct Obstacle patternA[] = {
    { OBSTACLE_BLOCK_1, {-320.f, 0.f,   0.f} }, // row 1
    { OBSTACLE_BLOCK_1, { 320.f, 0.f,   0.f} },
    { OBSTACLE_BLOCK_1, {-320.f, 0.f, 160.f} }, // row 2
    { OBSTACLE_COIN,    {   0.f, 0.f, 160.f} },
    { OBSTACLE_BLOCK_1, { 320.f, 0.f, 160.f} },
    { OBSTACLE_BLOCK_1, {-320.f, 0.f, 320.f} }, // row 3
    { OBSTACLE_BLOCK_1, { 320.f, 0.f, 320.f} },
    { OBSTACLE_BLOCK_1, {-320.f, 0.f, 480.f} }, // row 4
    { OBSTACLE_COIN,    {   0.f, 0.f, 480.f} },
    { OBSTACLE_BLOCK_1, { 320.f, 0.f, 480.f} },
    { OBSTACLE_BLOCK_1, {-320.f, 0.f, 640.f} }, // row 5
    { OBSTACLE_BLOCK_1, { 320.f, 0.f, 640.f} },
    { OBSTACLE_TERMINATOR, {0.f, 0.f,   0.f} }
};
u8 patternABlacklist[] = {
    PATTERN_TERMINATOR,
};

struct Obstacle patternB[] = {
    { OBSTACLE_BLOCK_1, {-320.f, 0.f,   0.f} }, // row 1
    { OBSTACLE_COIN,    {-160.f, 0.f,   0.f} },
    { OBSTACLE_BLOCK_1, {   0.f, 0.f,   0.f} },
    { OBSTACLE_BLOCK_1, { 320.f, 0.f,   0.f} },
    { OBSTACLE_BLOCK_1, {-320.f, 0.f, 160.f} }, // row 2
    { OBSTACLE_COIN,    {-160.f, 0.f, 160.f} },
    { OBSTACLE_BLOCK_1, {   0.f, 0.f, 160.f} },
    { OBSTACLE_COIN,    { 160.f, 0.f, 160.f} },
    { OBSTACLE_BLOCK_1, { 320.f, 0.f, 160.f} },
    { OBSTACLE_BLOCK_1, {-320.f, 0.f, 320.f} }, // row 3
    { OBSTACLE_COIN,    {-160.f, 0.f, 320.f} },
    { OBSTACLE_BLOCK_1, {   0.f, 0.f, 320.f} },
    { OBSTACLE_BLOCK_1, { 320.f, 0.f, 320.f} },
    { OBSTACLE_TERMINATOR, {0.f, 0.f,   0.f} }
};
u8 patternBBlacklist[] = {
    PATTERN_B,
    PATTERN_C,
    PATTERN_D,
    PATTERN_F,
    PATTERN_G,
    PATTERN_TERMINATOR,
};

struct Obstacle patternC[] = {
    { OBSTACLE_BLOCK_1, {-320.f, 0.f,   0.f} }, // row 1
    { OBSTACLE_BLOCK_1, {   0.f, 0.f,   0.f} },
    { OBSTACLE_COIN,    { 160.f, 0.f,   0.f} },
    { OBSTACLE_BLOCK_1, { 320.f, 0.f,   0.f} },
    { OBSTACLE_BLOCK_1, {-320.f, 0.f, 160.f} }, // row 2
    { OBSTACLE_COIN,    {-160.f, 0.f, 160.f} },
    { OBSTACLE_BLOCK_1, {   0.f, 0.f, 160.f} },
    { OBSTACLE_COIN,    { 160.f, 0.f, 160.f} },
    { OBSTACLE_BLOCK_1, { 320.f, 0.f, 160.f} },
    { OBSTACLE_BLOCK_1, {-320.f, 0.f, 320.f} }, // row 3
    { OBSTACLE_BLOCK_1, {   0.f, 0.f, 320.f} },
    { OBSTACLE_COIN,    { 160.f, 0.f, 320.f} },
    { OBSTACLE_BLOCK_1, { 320.f, 0.f, 320.f} },
    { OBSTACLE_TERMINATOR, {0.f, 0.f,   0.f} }
};
u8 patternCBlacklist[] = {
    PATTERN_B,
    PATTERN_C,
    PATTERN_D,
    PATTERN_F,
    PATTERN_G,
    PATTERN_TERMINATOR,
};

struct Obstacle patternD[] = {
    { OBSTACLE_BLOCK_2, {-240.f, 0.f,   0.f} }, // row 1
    { OBSTACLE_COIN,    {   0.f, 0.f,   0.f} },
    { OBSTACLE_BLOCK_2, { 240.f, 0.f,   0.f} },
    { OBSTACLE_TERMINATOR, {0.f, 0.f,   0.f} }
};
u8 patternDBlacklist[] = {
    PATTERN_B,
    PATTERN_C,
    PATTERN_E,
    PATTERN_H,
    PATTERN_I,
    PATTERN_TERMINATOR,
};

struct Obstacle patternE[] = {
    { OBSTACLE_COIN,    {-160.f, 0.f, 160.f} }, // row 2
    { OBSTACLE_BLOCK_1, {   0.f, 0.f, 160.f} },
    { OBSTACLE_COIN,    { 160.f, 0.f, 160.f} },
    { OBSTACLE_BLOCK_3, {   0.f, 0.f, 320.f} }, // row 3
    { OBSTACLE_BLOCK_1, {   0.f, 0.f, 480.f} }, // row 4
    { OBSTACLE_TERMINATOR, {0.f, 0.f,   0.f} }
};
u8 patternEBlacklist[] = {
    PATTERN_A,
    PATTERN_B,
    PATTERN_C,
    PATTERN_D,
    PATTERN_F,
    PATTERN_G,
    PATTERN_TERMINATOR,
};

struct Obstacle patternF[] = {
    { OBSTACLE_COIN,    {   0.f, 0.f, 160.f} }, // row 2
    { OBSTACLE_COIN,    {-320.f, 0.f, 640.f} }, // row 5
    { OBSTACLE_BLOCK_4, {  80.f, 0.f, 640.f} },
    { OBSTACLE_TERMINATOR, {0.f, 0.f,   0.f} }
};
u8 patternFBlacklist[] = {
    PATTERN_A,
    PATTERN_B,
    PATTERN_C,
    PATTERN_D,
    PATTERN_G,
    PATTERN_TERMINATOR,
};

struct Obstacle patternG[] = {
    { OBSTACLE_COIN,    {   0.f, 0.f, 160.f} }, // row 2
    { OBSTACLE_COIN,    { 320.f, 0.f, 640.f} }, // row 5
    { OBSTACLE_BLOCK_4, { -80.f, 0.f, 640.f} },
    { OBSTACLE_TERMINATOR, {0.f, 0.f,   0.f} }
};
u8 patternGBlacklist[] = {
    PATTERN_A,
    PATTERN_B,
    PATTERN_C,
    PATTERN_D,
    PATTERN_F,
    PATTERN_TERMINATOR,
};

struct Obstacle patternH[] = {
    { OBSTACLE_COIN,    {-160.f, 0.f,   0.f} }, // row 1
    { OBSTACLE_BLOCK_1, {   0.f, 0.f,   0.f} },
    { OBSTACLE_COIN,    { 160.f, 0.f,   0.f} },
    { OBSTACLE_BLOCK_1, {-320.f, 0.f, 320.f} }, // row 3
    { OBSTACLE_COIN,    {   0.f, 0.f, 320.f} },
    { OBSTACLE_BLOCK_1, { 320.f, 0.f, 320.f} },
    { OBSTACLE_COIN,    {-160.f, 0.f, 640.f} }, // row 5
    { OBSTACLE_BLOCK_1, {   0.f, 0.f, 640.f} },
    { OBSTACLE_COIN,    { 160.f, 0.f, 640.f} },
    { OBSTACLE_TERMINATOR, {0.f, 0.f,   0.f} }
};
u8 patternHBlacklist[] = {
    PATTERN_D,
    PATTERN_F,
    PATTERN_G,
    PATTERN_TERMINATOR,
};

struct Obstacle patternI[] = {
    { OBSTACLE_COIN,    {-320.f, 0.f, 160.f} }, // row 2
    { OBSTACLE_BLOCK_1, {   0.f, 0.f, 160.f} },
    { OBSTACLE_COIN,    { 320.f, 0.f, 160.f} },
    { OBSTACLE_TERMINATOR, {0.f, 0.f,   0.f} }
};
u8 patternIBlacklist[] = {
    PATTERN_D,
    PATTERN_F,
    PATTERN_G,
    PATTERN_TERMINATOR,
};


// Pattern Array

#define PATTERN_COUNT 9

struct ObstaclePattern patternArray[] = {
    {5, patternA, patternABlacklist},
    {3, patternB, patternBBlacklist},
    {3, patternC, patternCBlacklist},
    {1, patternD, patternDBlacklist},
    {6, patternE, patternEBlacklist},
    {5, patternF, patternFBlacklist},
    {5, patternG, patternGBlacklist},
    {5, patternH, patternHBlacklist},
    {3, patternI, patternIBlacklist},
};