/* Pounce hold entry: places the owner's transform (+4) at a point built from the +0x3d8 item's
 * +0x14 x / +0x1c z and the +0x3b8 item's +0x18 y, points the entry's +0x14 at the owner's +0xad
 * byte, clears bit 1 of the owner's +0x5c, binds its channels 0 / 4 / 1 / 2 with (1, 0),
 * re-inits it, clears the +8 timer and +0x10 latch and moves the node to 020cf638. */

#include "nitro/fx_types.h"

extern void Srt_SetTranslation(int srt, VecFx32 *pos);
extern void SetSubitemState(int obj, int slot, int a, int b);
extern void RefreshObjectCallbacks(int obj, int a);
extern void SetIndexedSlot(int self, int idx, void *cb);
extern void Ov244_PounceHoldTick(void);

void Ov244_EnterPounceHold(int param_1) {
    int *node = *(int **)(param_1 + 4);
    node[6] = *(int *)(*(int *)(node[1] + 0x3d8) + 0x14);
    node[7] = *(int *)(*(int *)(node[1] + 0x3b8) + 0x18);
    node[8] = *(int *)(*(int *)(node[1] + 0x3d8) + 0x1c);
    Srt_SetTranslation(node[0] + 4, (VecFx32 *)(node + 6));
    node[5] = node[0] + 0xad;
    *(int *)(node[0] + 0x5c) &= ~2;
    SetSubitemState(node[0], 0, 1, 0);
    SetSubitemState(node[0], 4, 1, 0);
    SetSubitemState(node[0], 1, 1, 0);
    SetSubitemState(node[0], 2, 1, 0);
    RefreshObjectCallbacks(node[0], 0);
    node[2] = 0;
    *((unsigned char *)node + 0x10) = 0;
    SetIndexedSlot(param_1, *(signed char *)((char *)param_1 + 0x20), &Ov244_PounceHoldTick);
}
