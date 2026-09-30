/* Rise tick of the ov252 actor: the guard sweep runs (020ce370); once +0x64 reaches 0.4 with +0x89 at
 * 1 it drops to 0 and sound 0x148/4 plays at the +8 point. The +0xc velocity is damped to 0.875 and
 * gains the +0x574 part's +0x2c vector turned by the +0x54 heading; once the partner holds no queued
 * move +0x7c is half the frame rate, it faces the target, +0x88 = 3 with a lift height set and the node
 * moves on to 020d1abc. */

#include "nitro/fx_types.h"

extern void Ov252_GuardSweep(int *node);
extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, void *at);
extern VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov252_CheckTarget(int *node, VecFx32 *delta, int face);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_HoverTick_2(void);

void Ov252_RiseTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    Ov252_GuardSweep(node);
    state[0x19] += *(int *)(node[0] + 0x2c);
    if (state[0x19] >= 0x660 && *((unsigned char *)state + 0x89) == 1) {
        *((unsigned char *)state + 0x89) -= 1;
        Ov107_BuildAndSendUpdate(*state, 0x148, 4, (void *)state[2]);
    }
    v = Ov252_TurnVecY(state[0x15], (VecFx32 *)(*(int *)(*state + 0x574) + 0x2c));
    ScaleVec3Fx12(0xe00, (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    VEC_Add((VecFx32 *)(state + 3), &v, (VecFx32 *)(state + 3));
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    state[0x1f] = *(int *)(node[0] + 0x2c) / 2;
    Ov252_CheckTarget(node, 0, 1);
    if (state[0x1e] != 0) {
        *((unsigned char *)state + 0x88) = 3;
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_HoverTick_2);
}
