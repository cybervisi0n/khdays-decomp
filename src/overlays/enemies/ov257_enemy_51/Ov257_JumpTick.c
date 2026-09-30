/* Jump tick of an ov257 state: the +0x40 rate clears, the +0x10 step heads for the +0x60 target
 * (Ov257_SteerToTarget) plus the +0x64 velocity, whose height falls by 3/64 per tick; on the
 * ground (+0x17a bit 0) the horizontal velocity clears. Once falling within 12.0 of the floor
 * (+0x13c), animation 0x1e plays, the +0x3d0 part plays motion 0x1b, +0x44, +0x73 and +0x72 clear
 * and the tick hands over to Ov257_LandingSlamTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Bits17a { unsigned char b0 : 1; };

extern int Ov257_SteerToTarget(int *state, int target, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const void *a, const void *b, void *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov257_LandingSlamTick(int *node);

void Ov257_JumpTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    state[0x10] = 0;
    Ov257_SteerToTarget(state, state[0x18], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    VEC_Add(state + 4, state + 0x19, state + 4);
    state[0x1a] -= 0xc0;
    if (((struct Bits17a *)(*state + 0x17a))->b0) {
        state[0x19] = 0;
        state[0x1b] = 0;
    }
    if (state[0x1a] >= 0 || *(int *)(*state + 0x13c) >= 0xc000) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x1e, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3d0), 0x1b, 0);
    state[0x11] = 0;
    *((unsigned char *)state + 0x73) = 0;
    *((unsigned char *)state + 0x72) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov257_LandingSlamTick);
}
