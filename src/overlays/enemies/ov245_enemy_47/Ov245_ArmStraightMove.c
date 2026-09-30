/* Ov245_ArmStraightMove -- arm a straight-line move: latch the caller's distance into both +0x10 and
 * +0x14, clear the elapsed counter at +0x1c, reset the owner's velocity at +0x3bc to the shared
 * zero constant and put it in state 1. */

#include "nitro/fx_types.h"

extern int data_02041dc8;

void Ov245_ArmStraightMove(int *node, int v) {
    node[5] = v;
    node[4] = v;
    node[7] = 0;
    *(VecFx32 *)(node[0] + 0x3bc) = *(VecFx32 *)&data_02041dc8;
    *(unsigned char *)(node[0] + 0x1c7) = 1;
}
