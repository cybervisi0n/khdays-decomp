/*
 * Per-tick AI decision for the ov130 chaser.
 *
 * Speed falls with the actor's remaining hit points, so a hurt chaser closes
 * more slowly. It re-aims (the facing tick), turns the new heading into a
 * velocity through the shared sin/cos table, and runs down the move timer.
 * While the timer is live, or the owner is busy, nothing else happens.
 *
 * When it expires the chaser reconsiders by health band: above three quarters
 * it charges on a coin flip; between a half and three quarters it holds;
 * between a quarter and a half it rolls a d100 and either charges (under 20)
 * or backs off (under 60); below a quarter it always backs off. If nothing
 * requested a state, it notifies instead of taking a slot.
 */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

static inline void VEC_Set(VecFx32 *vec, int x, int y, int z) {
    vec->x = x;
    vec->y = y;
    vec->z = z;
}

extern int func_02020400(int num, int den);
extern void Ov130_UpdateChaseFacing(int *self);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void SetIndexedSlot(int self, int idx, int cb);
extern const short data_0203d210[];

void Ov130_DecideChaseMove(int *self)
{
    int *nd = (int *)self[1];
    int actor = *nd;
    int speed;
    int idx;
    int cap;
    int hp;
    int roll;
    int timer;
    VecFx32 *vel;

    if (*(short *)(actor + 0x218) == 0) {
        speed = 0;
    } else {
        speed = 0xc00 - func_02020400(*(short *)(actor + 0x21a) << 11,
                                      *(short *)(actor + 0x218));
    }
    Ov130_UpdateChaseFacing(self);

    idx = (int)(((unsigned)(((long long)(int)(unsigned)nd[9] * 0x28be60db9391LL +
                 0x80000000000LL) >> 0x20) << 4) >> 0x10) >> 4;
    vel = (VecFx32 *)(nd + 6);
    vel->x = (int)data_0203d210[idx * 2];
    vel->y = 0;
    vel->z = (int)data_0203d210[idx * 2 + 1];
    ScaleVec3Fx12(speed, vel, vel);

    timer = nd[0xe] - *(int *)(*self + 0x2c);
    nd[0xe] = timer;
    if (timer <= 0) {
        nd[0xe] = 0;
    }
    if (*(unsigned char *)(nd[1] + 0xad) != 0) {
        return;
    }
    if (nd[0xe] <= 0) {
        cap = *(short *)(actor + 0x218);
        hp = *(short *)(actor + 0x21a);
        if (hp >= cap * 0x4b / 100) {
            if (RandNextScaled(2) != 0) {
                *(char *)(*nd + 0x1c7) = 2;
            }
        } else if (hp < cap * 0x32 / 100) {
            if (hp >= cap * 0x19 / 100) {
                roll = RandNextScaled(100);
                if (roll < 0x14) {
                    *(char *)(*nd + 0x1c7) = 2;
                } else if (roll < 0x3c) {
                    *(char *)(*nd + 0x1c7) = 5;
                }
            } else {
                *(char *)(*nd + 0x1c7) = 5;
            }
        }
        if (*(signed char *)(*nd + 0x1c7) != -1) {
            SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), 0);
            return;
        }
    }
    Ov107_PostTagUpdate((Actor *)(*nd), 2, 0);
}
