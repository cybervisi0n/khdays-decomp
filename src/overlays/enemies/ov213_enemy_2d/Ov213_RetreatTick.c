/* Retreat tick: decays the +0x5c speed by a fifth, rebuilds the forward unit vector from the
 * +0x38 rotation (transforming data_02042258), scales it by that speed into the +0xc velocity
 * and, unless the +8 flag byte is set, writes pose kind 5 into the actor's +0x1c7 and
 * dispatches with a null handler. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern VecFx32 data_02042258;
extern int VEC_Normalize(VecFx32 *v, VecFx32 *unit);
extern void ScaleVec3Fx12(int scale, void *src, void *dst);
extern void SetIndexedSlot(void *node, int idx, void *value);

void Ov213_RetreatTick(int *node) {
    int *state = (int *)node[1];
    VecFx32 dir;
    state[0x17] = state[0x17] + -state[0x17] / 5;
    Vec3TransformViaTempMtx(&dir, state + 0xe, &data_02042258);
    VEC_Normalize(&dir, &dir);
    ScaleVec3Fx12(state[0x17], &dir, state + 3);
    if (*(unsigned char *)state[2] != 0) return;
    *(signed char *)(*state + 0x1c7) = 5;
    SetIndexedSlot(node, *(signed char *)(node + 8), (void *)0);
}
