/* Re-aim the strafe: ask the target tracker for the turn rate, rebuild the local direction
 * from the object's own frame, and slew towards it. Once the busy byte at state[0x13] clears,
 * switch to animation 9, clear the counter at state[0x10], drop the 0x40 bit of the hw60 high
 * byte and hand off to the next step.
 *
 * Matched byte-exact 2026-07-23, first compile. One of three byte-identical siblings. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern void ScaleVec3Fx12(int a, void *b, void *c);
extern void SetIndexedSlot(void *node, int idx, void *cb);
extern void Ov293_ConfigSubStateThenAdvanceSlot(void);

void Ov293_AiFollowPartUntilAnimEnd(int *node) {
    int *state = (int *)node[1];
    VecFx32 v;
    int n;

    n = Ov107_ActionResource_GetOffsetAndScale(*(int *)(state[0] + 0x39c), &v);
    Vec3TransformViaTempMtx(state + 7, (void *)(state[0] + 0xa0), &v);
    ScaleVec3Fx12(n, state + 7, state + 7);
    if (*(unsigned char *)state[0x13] != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)state[0], 9, 1);
    state[0x10] = 0;
    {
        unsigned short hw60 = *(unsigned short *)(state[0] + 0x60);
        *(unsigned short *)(state[0] + 0x60) =
            (hw60 & ~0xff00) |
            (((unsigned int)(unsigned short)((((unsigned int)hw60 << 0x10) >> 0x18) & ~0x40) << 0x18) >> 0x10);
    }
    SetIndexedSlot(node, *(signed char *)((int)node + 0x20), Ov293_ConfigSubStateThenAdvanceSlot);
}
