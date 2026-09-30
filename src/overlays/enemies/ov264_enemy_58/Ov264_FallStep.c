/*
 * Airborne step: drift the actor along its heading, and on landing hand off or bail out.
 *
 * The heading in state[0x11] is turned into a table index the usual way (the 0x28be60db9391
 * fixed-point scale, the same constant the sibling steer routine uses), then the sin/cos pair at
 * data_0203d210[idx*2] and [idx*2+1] becomes a flat direction. That direction is scaled by
 * state[0x12] / 40 into the step at state+0x20 and added to the owner's drift at owner+0x424.
 *
 * The node kind is read UP FRONT, before both vector calls, because the ROM keeps it live in a
 * callee-saved register across them; anything other than kind 3 returns here. For kind 3 the
 * owner's +0x60 high byte loses bit 0x40 and the landing flag decides the outcome: with a landing
 * point published, copy it to state+0x38, lift it by the owner's ground offset at +0x80, run the
 * three landing calls and arm the follow-up; without one, 020cce44 takes over.
 *
 * Two shapes had to be written exactly:
 *  - the +0x60 update is the BITFIELD form, not the extract/reassemble one: the ROM has the
 *    lsl #0x10 / lsr #0x10 truncation pair, which is the catalogued discriminator.
 *  - the landing flag is a ONE-BIT field, not a masked byte. The ROM reads it with
 *    ldrb + lsl #0x1f + lsrs #0x1f; a plain `& 1` test compiles to tst #1 and loses 4 bytes,
 *    because the short arm then falls inline instead of being placed out of line.
 *  - 020c0b90 takes the landing point BY VALUE: three words in r2, r3 and [sp], with the trailing
 *    zero as a separate fourth argument at [sp+4].
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
extern void Ov264_ProcessHitTargets(int *state, int a, int b);
extern void Ov264_TickDash(void);
extern short data_0203d210[];

void Ov264_FallStep(int *self) {
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
        Ov107_BuildAndSendUpdate(*state, 0x15d, 7, state + 0xe);
        state[0x14] = 0;
        SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), Ov264_TickDash);
    } else {
        Ov264_ProcessHitTargets(state, 1, 0);
    }
}
