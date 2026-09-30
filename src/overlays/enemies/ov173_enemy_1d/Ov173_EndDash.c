/* Dash end of the ov173 enemy (x2: ov173/174): with a target still held (+0x10) the +0x48 timer
 * restarts and the dash tick (Ov173_DashTick) runs again; otherwise the +8 position is
 * pushed to the actor (mode 5), reaction 0x141 mode 7 fires there and sub-state 2 is requested. */

#include "nitro/fx_types.h"

extern void func_ov107_020c0b90(int obj, int a, VecFx32 v, int d);
extern void Ov107_BuildAndSendUpdate(int obj, int a, int b, void *at);
extern void SetIndexedSlot(void *node, int idx, void *cb);
extern void Ov173_DashTick(void);

void Ov173_EndDash(int *node) {
    int *state = (int *)node[1];

    if (state[4] != 0) {
        state[0x12] = 0;
        SetIndexedSlot(node, *(signed char *)((int)node + 0x20), Ov173_DashTick);
        return;
    }
    func_ov107_020c0b90(state[0], 5, *(VecFx32 *)state[2], 0);
    Ov107_BuildAndSendUpdate(state[0], 0x141, 7, (void *)state[2]);
    *(unsigned char *)(state[0] + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((int)node + 0x20), 0);
}
