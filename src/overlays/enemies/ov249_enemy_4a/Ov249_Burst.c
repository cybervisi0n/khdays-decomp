/* Burst of the ov249 enemy: effects 9 and 3 (flag 1), 4 (flag 2) and 5 spawn at
 * the owner's +0x494 contact point, reaction 0x145 mode 0xa fires at the +0xc position, the +0x4c
 * timer and the +0x61/+0x62 flags reset and the tick hands over to Ov249_AiLungeStrikeTick. */

#include "nitro/fx_types.h"

extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov249_AiLungeStrikeTick(int *node);

void Ov249_Burst(int *node)
{
    int *state = (int *)node[1];

    func_ov107_020c0b90(*state, 9, *(VecFx32 *)(*state + 0x494), 1);
    func_ov107_020c0b90(*state, 3, *(VecFx32 *)(*state + 0x494), 1);
    func_ov107_020c0b90(*state, 4, *(VecFx32 *)(*state + 0x494), 2);
    func_ov107_020c0b90(*state, 5, *(VecFx32 *)(*state + 0x494), 0);
    Ov107_BuildAndSendUpdate(*state, 0x145, 0xa, (void *)state[3]);
    state[0x13] = 0;
    *((unsigned char *)state + 0x61) = 0;
    *((unsigned char *)state + 0x62) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov249_AiLungeStrikeTick);
}
