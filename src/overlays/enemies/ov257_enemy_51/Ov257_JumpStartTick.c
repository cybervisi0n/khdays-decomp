/* Jump start tick of an ov257 state: the +0x40 rate is the frame rate x 3, the nearest target
 * (020cab14) becomes +0x60 and the +0x10 step heads for it (Ov257_SteerToTarget). The +0x54 timer
 * accumulates the frame rate and at 0.33 reaction +0x408 mode 0x1b fires once at the +4 point
 * (+0x76). Once the +0xc idle byte clears the +0x64 jump velocity is set: towards the target on
 * the flat, as long as a 50th of the distance, and 1.0 up less a 50th of it (at least 1/16);
 * without a target just 1/16 up. Animation 0x1d and the +0x3d0 part's motion 0x1a play looped and
 * the tick hands over to Ov257_JumpTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov107_FindNearestObject(int obj, int *out);
extern int Ov257_SteerToTarget(int *state, int target, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int VEC_Mag(const VecFx32 *v);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov257_JumpTick(int *node);

void Ov257_JumpStartTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    VecFx32 d;
    int speed;
    int found;

    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 10;
    state[0x18] = Ov107_FindNearestObject(*state, &found);
    Ov257_SteerToTarget(state, state[0x18], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    state[0x15] += *(int *)(node[0] + 0x2c);
    if (*((unsigned char *)state + 0x76) == 0 && state[0x15] >= 0x555) {
        Ov107_BuildAndSendUpdate(state[0], (short)*(int *)(*state + 0x408), 0x1b, (void *)state[1]);
        *((unsigned char *)state + 0x76) = 1;
    }
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    if (state[0x18] != 0) {
        VEC_Subtract((void *)(state[0x18] + 0x74), (void *)(*state + 0x74), &d);
        d.y = 0;
        VEC_Normalize(&d, (VecFx32 *)(state + 0x19));
        ScaleVec3Fx12(VEC_Mag(&d) / 50, (VecFx32 *)(state + 0x19), (VecFx32 *)(state + 0x19));
        state[0x1a] = 0x1000 - VEC_Mag(&d) / 50;
        if (state[0x1a] < 0x100) {
            state[0x1a] = 0x100;
        }
    } else {
        state[0x19] = 0;
        state[0x1a] = 0x100;
        state[0x1b] = 0;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x1d, 1);
    Ov107_StartAnim(*(int *)(*state + 0x3d0), 0x1a, 1);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov257_JumpTick);
}
