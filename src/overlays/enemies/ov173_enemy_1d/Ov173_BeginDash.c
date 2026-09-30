/* Enter the dash of the ov173 enemy (x2: ov173/174): play animation 1, push the +8 position to
 * the actor (mode 5), fire reaction 0x141 mode 7 there, then set bit 1 and bit 7 of the +0x60
 * high byte around raising bit 0 of +0x1ae, clear the +0x48 counter and advance to
 * Ov173_DashTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void func_ov107_020c0b90(int obj, int a, VecFx32 v, int d);
extern void Ov107_BuildAndSendUpdate(int obj, int a, int b, void *at);
extern void SetIndexedSlot(void *node, int idx, void *cb);
extern void Ov173_DashTick(void);

void Ov173_BeginDash(int *node) {
    int *state = (int *)node[1];

    Ov107_PostTagUpdate((Actor *)state[0], 1, 1);
    func_ov107_020c0b90(state[0], 5, *(VecFx32 *)state[2], 0);
    Ov107_BuildAndSendUpdate(state[0], 0x141, 7, (void *)state[2]);
    {
        unsigned short hw60 = *(unsigned short *)(state[0] + 0x60);
        *(unsigned short *)(state[0] + 0x60) =
            (hw60 & ~0xff00) | (((((unsigned int)hw60 << 0x10) >> 0x18 | 2) << 0x18) >> 0x10);
    }
    *(unsigned short *)(state[0] + 0x1ae) |= 1;
    {
        unsigned short hw60 = *(unsigned short *)(state[0] + 0x60);
        *(unsigned short *)(state[0] + 0x60) =
            (hw60 & ~0xff00) | (((((unsigned int)hw60 << 0x10) >> 0x18 | 0x80) << 0x18) >> 0x10);
    }
    state[0x12] = 0;
    SetIndexedSlot(node, *(signed char *)((int)node + 0x20), Ov173_DashTick);
}
