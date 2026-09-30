/* Approach tick of an ov255 state: the +0x40 rate is the frame rate x 3 and the nearest target
 * (020cab14) becomes +0x5c; without one sub-state 2 is requested. Otherwise the path point is
 * resolved (Ov255_SteerToTarget) into the +0x10 step and, once the +0xc idle byte clears, the
 * next sub-state is 0xc when the +0x54 cooldown has run out and the gap between the two collision
 * radii exceeds 4.0, else 2. */

#include "nitro/fx_types.h"

extern int Ov107_FindNearestObject(int obj, int kind);
extern void Ov255_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov255_ApproachTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    VecFx32 d;
    int speed;

    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 10;
    state[0x17] = Ov107_FindNearestObject(*state, 0);
    if (state[0x17] == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    Ov255_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    {
        int target;
        int owner;
        int gap;

        owner = *state;
        target = state[0x17];

        VEC_Subtract((void *)(target + 0x74), (void *)(owner + 0x74), &d);
        gap = VEC_Normalize(&d, &d) - *(int *)(owner + 0x80) - *(int *)(target + 0x80);
        if (state[0x15] <= 0 && gap > 0x4000) {
            *(unsigned char *)(*state + 0x1c7) = 0xc;
        } else {
            *(unsigned char *)(*state + 0x1c7) = 2;
        }
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
