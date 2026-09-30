/* Attack tick of the ov299 enemy: once the +0x24 timer reaches the +0x2c delay, the +4 target
 * position is taken; with a +8 target actor it is instead that actor's +0x190 position offset
 * by a random direction (yaw within +-0x3244 of straight ahead) at a random distance of
 * +-0x100, +-0x1800 or +-0x3000 (30/30/40 % rolls). The point is raised 0x14000 and the number
 * of 0x60-accelerating fall steps that cover 0x14000 is counted; the +0xc velocity becomes the
 * facing (sin, 0, cos) of the +0x28 yaw at 0x800 and the point is pulled back by that many
 * steps along it before c5c4 launches the actor there; d47dc takes over. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void VEC_Add(const void *a, const VecFx32 *b, VecFx32 *d);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov299_AiEnterProjectileFlight(int *node);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

static inline int RandRange(int low, int high)
{
    int span = high - low;
    if (span < 0) span = -span;
    return low + RandNextScaled(span + 1);
}

void Ov299_AttackTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 target;
    VecFx32 dir;
    int speed;
    int ang;
    int roll;
    unsigned int idx;
    int y;
    int v;
    int n;

    state[9] += *(int *)(*node + 0x2c);
    if (state[9] < state[0xb]) {
        return;
    }
    target = *(VecFx32 *)state[1];
    if (state[2] != 0) {
        ang = RandNextScaled(0x6489) - 0x3244;
        roll = RandNextScaled(100);
        if (roll < 30) {
            speed = 0x100;
        } else if (roll < 60) {
            speed = 0x1800;
        } else {
            speed = 0x3000;
        }
        idx = ANG2IDX(ang);
        dir.x = data_0203d210[idx * 2];                                       /* FX_SinIdx */
        dir.y = 0;
        dir.z = data_0203d210[idx * 2 + 1];                                   /* FX_CosIdx */
        ScaleVec3Fx12(RandRange(-speed, speed), &dir, &dir);
        VEC_Add((void *)(state[2] + 0x190), &dir, &target);
    }
    target.y += 0x14000;
    y = 0;
    v = 0;
    n = 0;
    do {
        y += v;
        v += 0x60;
        n++;
    } while (y < 0x14000);
    idx = ANG2IDX(state[0xa]);
    state[3] = data_0203d210[idx * 2];                                        /* FX_SinIdx */
    state[4] = 0;
    state[5] = data_0203d210[idx * 2 + 1];                                    /* FX_CosIdx */
    ScaleVec3Fx12(0x800, (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    target.x -= state[3] * n;
    target.z -= state[5] * n;
    Ov107_MoveNodeAndRelayout((Actor *)(*state), &target);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov299_AiEnterProjectileFlight);
}
