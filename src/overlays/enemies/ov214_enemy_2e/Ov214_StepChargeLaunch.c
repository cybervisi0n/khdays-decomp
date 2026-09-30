/*
 * Airborne step: drift the actor along its heading, and on landing hand off or bail out.
 */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct {
    unsigned short lo : 8;
    unsigned short hi : 8;
} Hw60;

typedef struct {
    unsigned char hasLanding : 1;
} LandingFlag;

static inline void VEC_Set(VecFx32 *vec, int x, int y, int z) {
    vec->x = x;
    vec->y = y;
    vec->z = z;
}

extern void ScaleVec3Fx12(int scale, void *v, void *dst);
extern void VEC_Add(void *a, void *b, void *dst);
extern void func_ov107_020c0b90(int obj, int a, VecFx32 v, int b);
extern void Ov107_BuildAndSendUpdate(int obj, int id, int a, void *v);
extern void SetIndexedSlot(int self, int idx, void *cb);
extern void Ov214_ProcessHitTargets(int *state, int a, int b);
extern void Ov214_TickDash(void);
extern short data_0203d210[];

void Ov214_StepChargeLaunch(int *self) {
    int *state = (int *)self[1];
    VecFx32 dir;
    int idx;
    short kind;

    idx = (int)(((unsigned)(((long long)(int)(unsigned)state[0x11] * 0x28be60db9391LL +
                 0x80000000000LL) >> 0x20) << 4) >> 0x10) >> 4;
    kind = *(short *)(*(int *)(*(int *)(*state + 0x420) + 0x88) + 2);
    VEC_Set(&dir, (int)data_0203d210[idx * 2], 0, (int)data_0203d210[idx * 2 + 1]);
    ScaleVec3Fx12(state[0x12] / 40, &dir, state + 8);
    VEC_Add(state + 8, (void *)(*state + 0x424), state + 8);
    if (kind != 3) {
        return;
    }

    ((Hw60 *)(*state + 0x60))->hi &= ~0x40;
    if (((LandingFlag *)(*state + 0x17a))->hasLanding) {
        *(VecFx32 *)(state + 0xe) = *(VecFx32 *)(*state + 0x180);
        state[0xf] -= *(int *)(*state + 0x80);
        func_ov107_020c0b90(*state, 3, *(VecFx32 *)(state + 0xe), 0);
        Ov107_PostTagUpdate((Actor *)(*state), 5, 0);
        Ov107_BuildAndSendUpdate(*state, 0x129, 7, state + 0xe);
        state[0x14] = 0;
        SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), Ov214_TickDash);
    } else {
        Ov214_ProcessHitTargets(state, 1, 0);
    }
}
