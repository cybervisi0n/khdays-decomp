/* Ov272_TickOrbitTarget -- orbit tick of the ov272 enemy (x3 with ov272/ov279). The target is re-acquired into +8 (none:
 * the tick hands over to Ov272_EnterPounce). The goal point sits on the circle of radius +0x68
 * around the target's +0x74 point at the +0x64 angle, lifted 3.0; the +0x30 step heads from the
 * +0x4c point towards it at up to 1.0. The +0x5c bob phase advances (half sine, lifted 3.0, into
 * +0x60, with the +0x34 climb following the owner's +0x13c height), and once the +0x50 timer
 * reaches 1.0 the tick hands over to Ov272_EnterPounce. */

#include "nitro/fx_types.h"

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)
static inline int FX_MUL(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

extern int Ov107_FindNearestObject(int owner, int flag);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern void Ov272_EnterPounce(int *node);

void Ov272_TickOrbitTarget(int *node)
{
    int *state = (int *)node[1];
    int height = *(int *)(*state + 0x13c);
    VecFx32 goal;
    int target;
    int len;
    int bob;

    state[2] = Ov107_FindNearestObject(*state, 0);
    target = state[2];
    if (target == 0) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov272_EnterPounce);
        return;
    }
    goal.x = *(int *)(target + 0x74) + FX_MUL(data_0203d210[ANG2IDX(state[0x19]) * 2], state[0x1a]);
    goal.y = *(int *)(target + 0x78) + 0x3000;
    goal.z = *(int *)(target + 0x7c) + FX_MUL(data_0203d210[ANG2IDX(state[0x19]) * 2 + 1], state[0x1a]);
    VEC_Subtract(&goal, (void *)state[0x13], (VecFx32 *)(state + 0xc));
    len = VEC_Normalize((VecFx32 *)(state + 0xc), (VecFx32 *)(state + 0xc));
    if (len > 0x1000) {
        len = 0x1000;
    }
    ScaleVec3Fx12(len, (VecFx32 *)(state + 0xc), (VecFx32 *)(state + 0xc));
    state[0x17] += *(int *)(*node + 0x2c);
    if (state[0x17] > 0x2000) {
        state[0x17] -= 0x4000;
    }
    bob = data_0203d210[ANG2IDX(FX_MUL(state[0x17], 0x3244) / 2) * 2] / 2 + 0x3000;
    state[0x18] = bob;
    state[0xd] = height > bob ? -0x80 : 0x80;
    state[0x14] += *(int *)(*node + 0x2c);
    if (state[0x14] < 0x1000) {
        return;
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov272_EnterPounce);
}
