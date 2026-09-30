/* ov258 .rodata 0x020d16f4-0x020d1850: the local initializer templates of the actor's functions,
 * one object per function in the original unit's order. Q12 fixed point for the vectors. */

/* Ov258_PlayRigMove: per-move poses of the +0x384 body rig and the +0x3ac tail rig. */

#include "nitro/fx_types.h"

typedef struct { int id[16]; } MovePoses;

/* Ov258_Construct (constructor): poses of the 43 hidden parts. */
typedef struct { int id[43]; } PartPoses;

/* Ov258_SwingTick_2: the five hit reaction variants a strike picks from at random; the object
 * pads to the section end. */
typedef struct { short mode[5]; short pad; } ReactionVariants;

const MovePoses data_ov258_020d16f4 = { { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 } };
const MovePoses data_ov258_020d1734 = { { 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33 } };

const PartPoses data_ov258_020d1774 = { {
    34, 35, 36, 37, 38, 39, 41, 42, 43, 44, 45, 46, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
    48, 49, 49, 50, 50, 51, 51, 51, 51, 51, 51, 52, 53, 54, 55, 56, 57, 57, 58, 59, 60,
} };

/* Ov258_StompTick: strike push (0, 1.25, 0). */
const VecFx32 data_ov258_020d1820 = { 0, 0x1400, 0 };

/* Ov258_StompTick: reach 7.875 ahead, turned by the heading. */
const VecFx32 data_ov258_020d182c = { 0, 0, 0x7e00 };

/* Ov258_TickBarrage: offset 5.0 behind. */
const VecFx32 data_ov258_020d1838 = { 0, 0, -0x5000 };

const ReactionVariants data_ov258_020d1844 = { { 1, 9, 5, 6, 10 }, 0 };
