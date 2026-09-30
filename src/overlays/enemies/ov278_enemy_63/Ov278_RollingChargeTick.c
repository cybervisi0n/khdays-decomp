/* Rolling-charge tick: the nearest target (020cab14) becomes the +8 mark (none: pose 0xa). While
 * the actor is blocked (+0x17a bit 1) the +0x3c velocity is reflected off the actor's +0x114 wall
 * normal at half its speed. The step 1/(+0x53 + 1) (64-bit divide) steers the velocity towards the
 * old mark on x and z by 64 steps and caps its speed at 0.75 steps; y clears and the result is
 * kept in +0x18. The four +0x4c hit cooldowns count down. The +0x3a4 part's segment (+0x78), moved
 * by the step and with its radius x 3, sweeps the actor list: an entity whose +0x1b4 slot is
 * cooled down is pushed 0.375 away horizontally from the part (kind 2); on acceptance the message
 * data_ov278_020d638e carries its +0x74 point raised by 0.5 to the +0x24 hook, its slot cools for
 * 8 ticks, reaction 0 mode 0x4e fires there and the velocity bounces off it at full speed. The +0x28
 * timer accumulates the frame rate; past 5.0 the +4 part's +0xa8 flag clears and the tick hands
 * over to Ov278_ReactDecayAimAndCancelAction. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int value; } Fx32;
typedef struct { u16 id; u8 kind; u8 cmd; u8 flag; u8 pos[9]; } Cmd14;
typedef struct { VecFx32 p0; VecFx32 dir; int nLength; int nRadius; } Segment;

#define PACK(cmd, dead, src, at)                                              \
    (dead) = (src);                                                           \
    ((u8 *)&(cmd))[at] = (u8)(((unsigned int)(dead).value >> 0x10 & 0x7f)     \
                              | ((unsigned int)(dead).value >> 0x18 & 0x80)); \
    ((u8 *)&(cmd))[(at) + 1] = (u8)((unsigned int)(dead).value >> 8);         \
    ((u8 *)&(cmd))[(at) + 2] = (u8)(dead).value

struct Bits17a { unsigned char b0 : 1, b1 : 1; };

extern int Ov107_FindNearestObject(int obj, int kind);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int VEC_Mag(const void *v);
extern void ScaleVec3Fx12(int scale, const void *v, void *out);
extern int VEC_DotProduct(const void *a, const void *b);
extern void VEC_Subtract(const void *a, const void *b, void *out);
extern int VEC_Normalize(const void *v, void *out);
extern long long func_020201b8(long long a, long long b);
extern void VEC_Add(const void *a, const void *b, void *out);
extern int Ov107_CollectSegmentOverlaps(int owner, Segment *seg, int *hits);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern const Cmd14 data_ov278_020d638e;
extern void Ov278_ReactDecayAimAndCancelAction(int *node);

void Ov278_RollingChargeTick(int *node)
{
    int *state = (int *)node[1];
    int actor;
    int owner = *state;
    int prev = state[2];
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;
    int hits[4];
    Segment seg;
    VecFx32 neg;
    VecFx32 refl;
    VecFx32 push;
    VecFx32 raw;
    Cmd14 msg;
    VecFx32 neg2;
    VecFx32 refl2;
    VecFx32 normal;
    long long q;
    int accel;
    int cap;
    int speed;
    int len;
    u8 *cb;
    int k;
    int n;
    int i;

    state[2] = Ov107_FindNearestObject(owner, 0);
    if (state[2] == 0) {
        *(u8 *)(*state + 0x1c7) = 0xa;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    actor = *state;
    if (((struct Bits17a *)(actor + 0x17a))->b1) {
        len = VEC_Mag(state + 0xf);
        ScaleVec3Fx12(-0x1000, state + 0xf, &neg);
        ScaleVec3Fx12(VEC_DotProduct(&neg, (void *)(actor + 0x114)) << 1, (void *)(actor + 0x114), &refl);
        VEC_Subtract(&refl, &neg, &refl);
        VEC_Normalize(&refl, state + 0xf);
        ScaleVec3Fx12(len / 2, state + 0xf, state + 0xf);
    }
    q = func_020201b8(0x100000000LL, (long long)(*((signed char *)state + 0x53) + 1));
    state[0xa] += *(int *)(node[0] + 0x2c);
    accel = (int)(((q << 6) + 0x80000000LL) >> 32);
    if (*(int *)(owner + 0x74) > *(int *)(prev + 0x74)) {
        state[0xf] -= accel;
    }
    if (*(int *)(owner + 0x74) < *(int *)(prev + 0x74)) {
        state[0xf] += accel;
    }
    if (*(int *)(owner + 0x7c) > *(int *)(prev + 0x7c)) {
        state[0x11] -= accel;
    }
    if (*(int *)(owner + 0x7c) < *(int *)(prev + 0x7c)) {
        state[0x11] += accel;
    }
    cap = (int)((q * 0xc00 + 0x80000000LL) >> 32);
    speed = VEC_Mag(state + 0xf);
    if (speed > cap) {
        speed = cap;
    }
    state[0x10] = 0;
    VEC_Normalize(state + 0xf, state + 0xf);
    ScaleVec3Fx12(speed, state + 0xf, state + 0xf);
    *(VecFx32 *)(state + 6) = *(VecFx32 *)(state + 0xf);
    cb = (u8 *)state;
    for (k = 0; k < 4; k++) {
        if (cb[k + 0x4c] != 0) {
            cb[k + 0x4c]--;
        }
    }
    seg = *(Segment *)(**(int **)(*state + 0x3a4) + 0x78);
    VEC_Add(&seg.p0, state + 6, &seg.p0);
    seg.nRadius *= 3;
    n = Ov107_CollectSegmentOverlaps(*state, &seg, hits);
    for (i = 0; i < n; i++) {
        if (((u8 *)state)[0x4c + *(u8 *)(hits[i] + 0x1b4)] != 0) {
            continue;
        }
        VEC_Subtract((void *)(hits[i] + 0x74), (void *)(**(int **)(*state + 0x3a4) + 4), &push);
        push.y = 0;
        VEC_Normalize(&push, &push);
        ScaleVec3Fx12(0x600, &push, &push);
        if (Ov107_InvokeHitCallback(hits[i], *state, *state, 2, &push, 0) == 0) {
            continue;
        }
        msg = data_ov278_020d638e;
        raw = *(VecFx32 *)(hits[i] + 0x74);
        raw.y += 0x800;
        PACK(msg, scratchX, *(Fx32 *)&raw.x, 5);
        PACK(msg, scratchY, *(Fx32 *)&raw.y, 8);
        PACK(msg, scratchZ, *(Fx32 *)&raw.z, 11);
        if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
            (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
        }
        ((u8 *)state)[0x4c + *(u8 *)(hits[i] + 0x1b4)] = 8;
        Ov107_BuildAndSendUpdate(*state, 0, 0x4e, &raw);
        VEC_Subtract((void *)(**(int **)(*state + 0x3a4) + 4), (void *)(hits[i] + 0x74), &normal);
        normal.y = 0;
        VEC_Normalize(&normal, &normal);
        len = VEC_Mag(state + 0xf);
        ScaleVec3Fx12(-0x1000, state + 0xf, &neg2);
        ScaleVec3Fx12(VEC_DotProduct(&neg2, &normal) << 1, &normal, &refl2);
        VEC_Subtract(&refl2, &neg2, &refl2);
        VEC_Normalize(&refl2, state + 0xf);
        ScaleVec3Fx12(0x1000, state + 0xf, state + 0xf);
    }
    if (state[0xa] < 0x5000) {
        return;
    }
    *(u8 *)(state[1] + 0xa8) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov278_ReactDecayAimAndCancelAction);
}
