/* Seek entry: while aggressive (+0x78) bit 2 of the actor's +0x60 high byte is set. Without a
 * route point (020cd750) the next move is 2. Otherwise the pose / partner motion pair is 1 / 0
 * (aggressive) or 0xc / 7 (+0x75 / +0x76) and plays (partner looping); aggressive also starts the
 * +0x460 / +0x464 helpers and knocks the actor back in place (mode 0xb). The +0x10 / +0x60
 * counters and +0x71 flag clear and the node moves to 020ce7a0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern int Ov254_PickRoutePoint(int *state);
extern void Ov107_PostTagUpdate(int actor, int pose, int flag);
extern void Ov107_StartAnim(int part, int motion, int mode);
extern void Ov254_ForwardToAiIfReady_6(int helper);
extern void Ov254_ForwardToAiIfReady_8(int helper);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern void Ov254_AnimatedOrbitTick(void);

void Ov254_SeekEntry(int *node)
{
    int *state = (int *)node[1];

    if (state[0x1e] != 0) {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 4) << 0x18) >> 0x10);
    }
    if (Ov254_PickRoutePoint(state) == 0) {
        *(signed char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    *((u8 *)state + 0x75) = state[0x1e] != 0 ? 1 : 0xc;
    *((u8 *)state + 0x76) = state[0x1e] != 0 ? 0 : 7;
    Ov107_PostTagUpdate(*state, *((u8 *)state + 0x75), 0);
    Ov107_StartAnim(*(int *)(*state + 0x430), *((u8 *)state + 0x76), 1);
    if (state[0x1e] != 0) {
        Ov254_ForwardToAiIfReady_6(*(int *)(*state + 0x460));
        Ov254_ForwardToAiIfReady_8(*(int *)(*state + 0x464));
        func_ov107_020c0b90(*state, 0xb, data_02041dc8, 0);
    }
    state[4] = 0;
    state[0x18] = 0;
    *((u8 *)state + 0x71) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov254_AnimatedOrbitTick);
}
