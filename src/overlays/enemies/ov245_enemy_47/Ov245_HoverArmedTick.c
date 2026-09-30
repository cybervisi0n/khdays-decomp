/* Ov245_HoverArmedTick -- hover tick (armed variant): pushes the +0xc velocity along the +0x4c8
 * anchor's +0x2c direction by -1.0, accumulates the node origin's height change into +0x28
 * (tracking it at +0x30), then, once the +0x434 owner has no +0x39c target, or as soon as bit 0
 * of the owner's +0x1ac is set, resets the actor (020cce28 / pose 8), starts motion 2 of the
 * anchor, turns the accumulated height into the +0x30 ratio against 0xe40f (FX_Inv of the
 * negation) and moves the node to the hover tick (020ce2d4) or the strike (020ce33c). */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void Ov245_ResetMode(int actor);
extern int FX_Div(int num, int den);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov245_HoverTick(void);
extern void Ov245_ApplyRecoilPush(void);

void Ov245_HoverArmedTick(int *node) {
    int *state = (int *)node[1];
    int owner;

    ScaleVec3Fx12(-0x1000, (VecFx32 *)(*(int *)(*state + 0x4c8) + 0x2c), (VecFx32 *)(state + 3));
    state[10] += *(int *)(state[2] + 8) - state[0xc];
    state[0xc] = *(int *)(state[2] + 8);
    owner = *(int *)(*state + 0x434);
    if (*(int *)(owner + 0x39c) == 0) {
        Ov245_ResetMode(*state);
        Ov107_StartAnim(*(int *)(*state + 0x4c8), 2, 0);
        state[0xc] = FX_Div(-state[10], 0xe40f);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_HoverTick);
        return;
    }
    if ((*(unsigned short *)(owner + 0x100 + 0xac) & 1) == 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 8, 0);
    Ov107_StartAnim(*(int *)(*state + 0x4c8), 2, 0);
    state[0xc] = FX_Div(-state[10], 0xe40f);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_ApplyRecoilPush);
}
