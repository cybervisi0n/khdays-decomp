/* Sway tick of the ov252 actor: +0x64 accumulates the frame rate, the +0xc velocity follows the +0x574
 * part's +0x2c vector turned by the +0x54 heading (height from the part's +0x30) and scaled by +0x70 +
 * 0.5; in phases 3 and 4 (+0x579) after 1.83 it rises (4) or sinks (3) at 0.625. Once the partner holds
 * no queued move the next +0x84 pose plays, the part takes motion 4 (phase 1) or 7 (phase 2), +0x64
 * clears and the node moves on to 020cf7d8. */

#include "nitro/fx_types.h"

extern VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void Ov107_PostTagUpdate(int actor, int pose, int loop);
extern int Ov107_StartAnim(int part, int motion, int mode);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_SwayTurnTick(void);

void Ov252_SwayTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;
    unsigned char phase;

    state[0x19] += *(int *)(node[0] + 0x2c);
    v = Ov252_TurnVecY(state[0x15], (VecFx32 *)(*(int *)(*state + 0x574) + 0x2c));
    *(VecFx32 *)(state + 3) = v;
    state[4] = *(int *)(*(int *)(*state + 0x574) + 0x30);
    ScaleVec3Fx12(state[0x1c] + 0x800, (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    phase = *(unsigned char *)(*state + 0x579);
    if (!(phase != 3 && phase != 4) && state[0x19] >= 0x1d38) {
        state[4] = phase == 4 ? 0xa00 : -0xa00;
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate(*state, ++*((unsigned char *)state + 0x84), 0);
    switch (*(unsigned char *)(*state + 0x579)) {
    case 1:
        Ov107_StartAnim(*(int *)(*state + 0x574), 4, 0);
        break;
    case 2:
        Ov107_StartAnim(*(int *)(*state + 0x574), 7, 0);
        break;
    }
    state[0x19] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_SwayTurnTick);
}
