/* Ov247_AiDescentProbe_2 -- the fall tick: drop the object, orient it to the surface it is over, and
 * hand off once the landing timer expires.
 *
 * Losing the target (Ov107_FindNearestObject returning 0) ends the state through Task_MarkFinished.
 *
 * The probe is a straight -0x32000 Y ray (Ov247_ProbeGround); when it finds something, the hit
 * is turned into an orientation (Quat_FromTwoVectors against the constant axis data_02042264) and
 * pushed into the sub-object at ctx[2]+4, whose height at +0x20 also creeps up by 0x100 a tick.
 *
 * The position at ctx+0x10 is rebuilt every tick from *(ctx[0]+0x398): x and z are copied
 * straight over (+0x14 / +0x1c of that block) and y advances by FX_Mul(delta * 30, 0x800) --
 * half of it, kept unfolded because that is the multiply-then-shift the ROM emits.
 *
 * The timer at +0x28 runs to 0x1000, then resets and hands off to Ov247_AiDescentProbe.
 *
 * Stack (0x28): q at sp+0, v at sp+0x10, hit at sp+0x1c -- declared back-to-front, and `v` uses an
 * aggregate initialiser so its address is materialised into a register (see codegen-cracks.md);
 * ctx and delta take declaration-initialisers so they are read before the zeroing, as the ROM
 * does. */

#include "nitro/fx_types.h"

typedef struct {
    int x;
    int y;
    int z;
    int w;
} Quaternion;

extern int Ov107_FindNearestObject(int owner, int a);
extern void Task_MarkFinished(int self);
extern int Ov247_ProbeGround(int *ctx, const VecFx32 *ray, VecFx32 *hit);
extern void Quat_FromTwoVectors(Quaternion *out, const VecFx32 *axis, const VecFx32 *hit);
extern void Srt_SetRotationQuat(int obj, const Quaternion *q);
extern void Srt_SetTranslation(int obj, const VecFx32 *v);
extern void SetIndexedSlot(int self, int action, void *cb);
extern void Ov247_AiDescentProbe(void);
extern VecFx32 data_02042264;

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov247_AiDescentProbe_2(int self) {
    int *ctx = *(int **)(self + 4);
    int delta = *(int *)(*(int *)self + 0x2c) * 30;
    VecFx32 hit;
    VecFx32 v = {0, 0, 0};
    Quaternion q;

    ctx[0xb] = Ov107_FindNearestObject(ctx[0], 0);
    if (ctx[0xb] == 0) {
        Task_MarkFinished(self);
        return;
    }

    v.y = -0x32000;
    if (Ov247_ProbeGround(ctx, &v, &hit) != 0) {
        Quat_FromTwoVectors(&q, &data_02042264, &hit);
        Srt_SetRotationQuat(ctx[2] + 4, &q);
        ctx[8] += 0x100;
        Srt_SetTranslation(ctx[2] + 4, (const VecFx32 *)((char *)ctx + 0x1c));
    }

    ctx[4] = *(int *)(*(int *)(ctx[0] + 0x398) + 0x14);
    ctx[5] += FX_Mul(delta, 0x800);
    ctx[6] = *(int *)(*(int *)(ctx[0] + 0x398) + 0x1c);
    Srt_SetTranslation(ctx[1] + 4, (const VecFx32 *)((char *)ctx + 0x10));

    ctx[0xa] += *(int *)(*(int *)self + 0x2c);
    if (ctx[0xa] <= 0x1000) {
        return;
    }

    ctx[0xa] = 0;
    SetIndexedSlot(self, *(signed char *)(self + 0x20), Ov247_AiDescentProbe);
}
