/* Ov244_TickChargeUp -- charge-up tick of an ov244 part: keeps the part at its anchor's +0x14
 * position raised to the target's height + 0x200, accumulates the owner's rate on the +4 timer and
 * steps the +0x10 phase at 0x2800, 0x3214 and 0x3547, spawning effect 0x113 (variants 0xb and
 * 0xc) at the part on the last two steps. Once the part is idle (+0xad clear) its channels 0, 2,
 * 4 and 1 are started (mode 1), it is reset (0203c7ac) and the tick hands over to
 * Ov244_WaitRigIdleBlendOut. */

#include "nitro/fx_types.h"

extern void Srt_SetTranslation(void *pSrt, VecFx32 *pPos);
extern void Slot_Spawn(int nEffect, int nVariant, VecFx32 *pPos, int nFlags);
extern void SetSubitemState(int obj, int channel, int a, int b);
extern void RefreshObjectCallbacks(int obj, int n);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov244_WaitRigIdleBlendOut(int *node);

void Ov244_TickChargeUp(int *node)
{
    int *state = (int *)node[1];
    VecFx32 pos;

    pos = *(VecFx32 *)(*(char **)(state[3] + 0x3c8) + 0x14);
    pos.y = *(int *)(state[3] + 0xb4) + 0x200;
    Srt_SetTranslation((void *)(state[0] + 4), &pos);
    state[1] += *(int *)(*node + 0x2c);
    if (*(unsigned char *)(state + 4) == 0 && state[1] >= 0x2800) {
        (*(unsigned char *)(state + 4))++;
    } else if (*(unsigned char *)(state + 4) == 1 && state[1] >= 0x3214) {
        (*(unsigned char *)(state + 4))++;
        Slot_Spawn(0x113, 0xb, &pos, 0);
    } else if (*(unsigned char *)(state + 4) == 2 && state[1] >= 0x3547) {
        (*(unsigned char *)(state + 4))++;
        Slot_Spawn(0x113, 0xc, &pos, 0);
    }
    if (*(unsigned char *)(state[0] + 0xad) == 0) {
        SetSubitemState(state[0], 0, 1, 0);
        SetSubitemState(state[0], 2, 1, 0);
        SetSubitemState(state[0], 4, 1, 0);
        SetSubitemState(state[0], 1, 1, 0);
        RefreshObjectCallbacks(state[0], 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov244_WaitRigIdleBlendOut);
        return;
    }
}
