/* Applies a charge level once: when the level's bit is not set yet and the charge time has reached
 * the threshold, binds and rewinds the charge effect's tracks, places it at the character facing
 * its way and marks it active. */

#include "nitro/fx_types.h"

extern void BindAnimTrack(int a, unsigned short b, int c, int d);
extern void Anim_SetFrameWrapped(int a, int b, int c);

void Ov036_ApplyChargeLevelOnce(int self, int bit) {
    int *blk = (int *)(self + 0x2c80);
    int i;
    if (blk[2] & (1 << bit)) return;
    if (*(int *)(self + 0x7b0) < blk[1]) return;
    blk[2] |= (1 << bit);
    for (i = 0; i < 5; i++) {
        if (((short *)((char *)blk + 0xec))[(unsigned short)i] > 0) {
            BindAnimTrack((int)blk + 0xc, i, (int)blk + 0xec, 0);
            Anim_SetFrameWrapped((int)blk + 0xc, (unsigned short)i, 0);
        }
    }
    *(VecFx32 *)((char *)blk + 0xb0) = *(VecFx32 *)(self + 0x8c + 0x400);
    *(unsigned short *)((char *)blk + 0x88) =
        (unsigned short)(*(unsigned short *)(*(int *)(self + 0x20) + 0x80) - 0x8000) + 0x8000;
    *(unsigned short *)((char *)blk + 0xc) |= 0x20;
    blk[0] = 1;
}
