/* main .rodata 0x02041dc8-0x02041e2c: the shared zero vector and the 2D BG tables of the
 * screen/palette helpers (0x020242cc, 0x02024844..0x020248e0). */

/* One entry of a BG screen-size lookup: width and height in pixels and the BGnCNT screen-size
 * code they map to (LookupPairKey returns the code, -1 if absent). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct BgScreenSize {
    u16 width;
    u16 height;
    u16 sizeCode;
} BgScreenSize;

/* The zero vector: the default position/offset read by several hundred game functions. */
const VecFx32 data_02041dc8 = { 0, 0, 0 };

/* BGnCNT register offsets (from 0x04000000) of the main BG0/BG1 and sub BG0/BG1; 0 for the
 * BGs without an extended-palette slot. remapIndexIfHwFlagSet checks bit 13 through it. */
const u16 data_02041dd4[8] = {
    0x0008, 0x000a, 0, 0,
    0x1008, 0x100a, 0, 0,
};

/* Affine BG sizes (128..1024 square), used by Gfx_DispatchByPairKeyB. */
const BgScreenSize data_02041de4[4] = {
    { 128, 128, 0 }, { 256, 256, 1 }, { 512, 512, 2 }, { 1024, 1024, 3 },
};

/* Extended-affine (bitmap) BG sizes, same shape, used by Gfx_DispatchByPairKeyA. */
const BgScreenSize data_02041dfc[4] = {
    { 128, 128, 0 }, { 256, 256, 1 }, { 512, 512, 2 }, { 1024, 1024, 3 },
};

/* Text BG sizes, used by Cmd_DispatchWithFlag. */
const BgScreenSize data_02041e14[4] = {
    { 256, 256, 0 }, { 256, 512, 2 }, { 512, 256, 1 }, { 512, 512, 3 },
};
