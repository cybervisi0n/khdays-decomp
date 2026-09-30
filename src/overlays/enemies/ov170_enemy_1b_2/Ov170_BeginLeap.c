/* Leap entry of the ov169 enemy (and its byte-identical twins). With a target in reach the
 * +0x30/+0x38 counters reset, the +0xc velocity becomes the direction to the target scaled by
 * 0x666 and the flight lasts distance/0x666 frames; the +0x3c drop is pre-integrated over half of
 * them (the frame-time inverse times -0x100 accumulates into a falling step), the +0x40 shadow
 * copies the +0x10 height, and a flight shorter than 0xf000 (after the <<12 scaling) rescales the
 * velocity to 0xf000. The +0x18 spin copies the actor's +0x390 vector with a random yaw offset in
 * [-0x165, 0x165]. In every case reaction 0x13f mode 5 fires, the +0x60 flags drop bit 7 and set
 * bit 0 of the high byte, the +0x388 item's +8 low byte sets bit 0, the +0x24 origin copies the
 * +8 position and the tick hands off to the leap state. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

struct hw60 { unsigned short lo : 8, hi : 8; };
struct b8 { unsigned f : 8; };

extern int Ov107_FindNearestObject(int actor, int mode);
extern void VEC_Subtract(void *a, void *b, void *d);
extern int VEC_Normalize(void *a, void *d);
extern void ScaleVec3Fx12(int scale, void *v, void *d);
extern int FX_Inv(int v);
extern int FX_Div(int a, int b);
extern void Ov107_BuildAndSendUpdate(int actor, int a, int id, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov170_LeapTick(int *node);

static inline int FX_Mul(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov170_BeginLeap(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    unsigned short *hw;
    unsigned int h;
    int target;
    int step;
    int i;

    target = Ov107_FindNearestObject(*state, 0);
    if (target != 0) {
        state[0xe] = 0;
        state[0xc] = 0;
        VEC_Subtract((void *)(target + 0x74), (void *)state[2], &dir);
        state[0xd] = VEC_Normalize(&dir, &dir) / 0x666;
        ScaleVec3Fx12(0x666, &dir, (void *)(state + 3));
        state[0x10] = state[4];
        state[0xf] = 0;
        i = 0;
        step = 0;
        for (; i < state[0xd] / 2; i++) {
            step += FX_Mul(FX_Inv(*(int *)(Ov107_GetActorManager() + 0x40)), -0x100);
            state[0xf] -= step;
        }
        state[0xd] <<= 12;
        if (state[0xd] != 0 && state[0xd] < 0xf000) {
            ScaleVec3Fx12(FX_Div(state[0xd], 0xf000), (void *)(state + 3), (void *)(state + 3));
            state[0x10] = state[4];
            state[0xd] = 0xf000;
        }
        *(VecFx32 *)(state + 6) = *(VecFx32 *)(*state + 0x390);
        state[7] += RandNextScaled(0x2cb) - 0x165;
    }
    Ov107_BuildAndSendUpdate(*state, 0x13f, 5, (void *)state[2]);
    ((struct hw60 *)(*state + 0x60))->hi &= ~0x80;
    hw = (unsigned short *)(*state + 0x60);
    h = *hw;
    /* hw60.hi |= 1 -- explicit-shift form (bitfield |= adds a redundant mask) */
    *hw = h & ~0xff00 | (((((unsigned int)h << 0x10) >> 0x18 | 1) << 0x18) >> 0x10);
    ((struct b8 *)(*(int *)(*state + 0x388) + 8))->f |= 1;
    *(VecFx32 *)(state + 9) = *(VecFx32 *)state[2];
    *(u8 *)(state + 0x12) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov170_LeapTick);
}
