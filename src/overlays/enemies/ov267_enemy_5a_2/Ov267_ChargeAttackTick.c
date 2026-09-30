/* Charge-attack tick of the ov266 enemy: the +0x40 timer accumulates the owner's rate and,
 * while it lies in [0xbb0, 0xff0], the once-only +0x5a cue spawns effect 9 at the actor's +0x74
 * position and fires reaction 0x15e mode 0xe at the +8 point; the +0x28 velocity is copied to
 * +0x10 and decayed per 0x88 slice of the frame by 1.0 - 0.0625 x (slice / 0x88), then the
 * sweep sphere -- the actor's +0x508 point offset by its radius along the +0x34 heading, of half
 * that radius -- is pushed through cf914 (mode 2). Once the +4 item is idle (+0xad) a finished
 * cf8e0 check with the +0x64 flag set clears it and queues sub-state 5, otherwise sub-state 2. */

#include "nitro/fx_types.h"

extern void func_ov107_020c0b90(int actor, int effect, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern int FX_Div(int num, int den);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Ov267_AreaAttackSweep(int *state, int mode, VecFx32 *sphere);
extern int Ov267_IsField21aBelowField218Div10(int *state);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];

static inline int FX_Mul(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}
static inline unsigned short FX_RadToIdx(int rad) {
    return (unsigned short)((0x28BE60DB9391LL * rad + 0x80000000000LL) >> 44);
}

void Ov267_ChargeAttackTick(int *node)
{
    int *state = (int *)node[1];
    struct { VecFx32 v; int r; } sphere;
    VecFx32 dir;
    int actor;
    int rem;
    int step;
    unsigned short idx;

    state[0x10] += *(int *)(node[0] + 0x2c);
    if (state[0x10] >= 0xbb0 && state[0x10] <= 0xff0) {
        if (*(unsigned char *)((char *)state + 0x5a) == 0) {
            actor = *state;
            func_ov107_020c0b90(actor, 9, *(VecFx32 *)(actor + 0x74), 0);
            Ov107_BuildAndSendUpdate(*state, 0x15e, 0xe, (void *)state[2]);
            *(unsigned char *)((char *)state + 0x5a) = 1;
        }
        *(VecFx32 *)(state + 4) = *(VecFx32 *)(state + 0xa);
        for (rem = *(int *)(node[0] + 0x2c); rem > 0; rem -= 0x88) {
            step = rem <= 0x88 ? rem : 0x88;
            ScaleVec3Fx12(0x1000 - FX_Mul(FX_Div(step, 0x88), 0x100), (VecFx32 *)(state + 0xa), (VecFx32 *)(state + 0xa));
        }
        idx = FX_RadToIdx(state[0xd]);
        dir.x = data_0203d210[(idx >> 4) * 2];
        dir.y = 0;
        dir.z = data_0203d210[(idx >> 4) * 2 + 1];
        ScaleVec3Fx12(*(int *)(*state + 0x80), &dir, &dir);
        VEC_Add((VecFx32 *)(*state + 0x108 + 0x400), &dir, &sphere.v);
        sphere.r = FX_Mul(*(int *)(*state + 0x80), 0x800);
        Ov267_AreaAttackSweep(state, 2, &sphere.v);
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    if (Ov267_IsField21aBelowField218Div10(state) == 0 && state[0x19] != 0) {
        state[0x19] = 0;
        *(unsigned char *)(*state + 0x1c7) = 5;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    *(unsigned char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
