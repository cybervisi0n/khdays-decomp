/* Dive-impact tick: a 1.5 sphere at the actor's +4 point sweeps the actor list on behalf of the
 * +0x384 rider. The first entity that accepts a 0.25 push in a random horizontal direction
 * (kind 5) receives the 14-byte message data_ov277_020d3718, flag 0x10, carrying the point of its
 * +0x80 radius towards the actor; the rider's reaction 0x165 mode 8 fires at the +4 point and
 * pose 0 is requested. Otherwise the +0x14 timer accumulates the frame rate and, once grounded
 * (+0x17a bit 0) or past 10.0, the message data_ov277_020d3726 carries the +4 point lowered by 1.5,
 * reaction 0x165 mode 9 fires there, the hook receives the message and pose 0 is requested. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int value; } Fx32;
typedef struct { u16 id; u8 kind; u8 cmd; u8 flag; u8 pos[9]; } Cmd14;
typedef struct { VecFx32 center; int nRadius; } Sphere;

#define PACK(cmd, dead, src, at)                                              \
    (dead) = (src);                                                           \
    ((u8 *)&(cmd))[at] = (u8)(((unsigned int)(dead).value >> 0x10 & 0x7f)     \
                              | ((unsigned int)(dead).value >> 0x18 & 0x80)); \
    ((u8 *)&(cmd))[(at) + 1] = (u8)((unsigned int)(dead).value >> 8);         \
    ((u8 *)&(cmd))[(at) + 2] = (u8)(dead).value

extern int Ov107_CollectSphereOverlaps(int owner, Sphere *sphere, int *hits);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, void *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern void VEC_Add(const void *a, const void *b, void *out);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const Cmd14 data_ov277_020d3718;
extern const Cmd14 data_ov277_020d3726;
struct Bits17a { unsigned char b0 : 1; };

static inline int RandRange(int lo, int hi)
{
    int d = hi - lo;

    if (d < 0) {
        d = -d;
    }
    return lo + RandNextScaled(d + 1);
}

void Ov277_DiveImpactTick(int *node)
{
    int *state = (int *)node[1];
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;
    Fx32 scratch2Z;
    Fx32 scratch2Y;
    Fx32 scratch2X;
    Sphere sphere;
    int hits[4];
    int n;
    int i;

    sphere.center = *(VecFx32 *)state[1];
    sphere.nRadius = 0x1800;
    n = Ov107_CollectSphereOverlaps(*(int *)(*state + 0x384), &sphere, hits);
    for (i = 0; i < n; i++) {
        VecFx32 push = {0, 0, 0};
        VecFx32 point;
        Cmd14 msg;

        push.x = RandRange(0, 0x1000);
        push.z = RandRange(0, 0x1000);
        VEC_Normalize(&push, &push);
        ScaleVec3Fx12(0x400, &push, &push);
        if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x384), 5, &push, 0) == 0) {
            continue;
        }
        msg = data_ov277_020d3718;
        VEC_Subtract((void *)state[1], (void *)(hits[i] + 0x74), &point);
        VEC_Normalize(&point, &point);
        ScaleVec3Fx12(*(int *)(hits[i] + 0x80), &point, &point);
        VEC_Add((void *)(hits[i] + 0x74), &point, &point);
        PACK(msg, scratchX, *(Fx32 *)&point.x, 5);
        PACK(msg, scratchY, *(Fx32 *)&point.y, 8);
        PACK(msg, scratchZ, *(Fx32 *)&point.z, 11);
        msg.flag = 0x10;
        if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
            (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
        }
        Ov107_BuildAndSendUpdate(*(int *)(state[0] + 0x384), 0x165, 8, (void *)state[1]);
        *(u8 *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    state[5] += *(int *)(node[0] + 0x2c);
    if (((struct Bits17a *)(*state + 0x17a))->b0 == 0 && state[5] < 0xa000) {
        return;
    }
    {
        VecFx32 raw;
        Cmd14 msg;

        msg = data_ov277_020d3726;
        raw = *(VecFx32 *)state[1];
        raw.y -= 0x1800;
        PACK(msg, scratch2X, *(Fx32 *)&raw.x, 5);
        PACK(msg, scratch2Y, *(Fx32 *)&raw.y, 8);
        PACK(msg, scratch2Z, *(Fx32 *)&raw.z, 11);
        Ov107_BuildAndSendUpdate(*(int *)(state[0] + 0x384), 0x165, 9, (void *)state[1]);
        if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
            (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
        }
    }
    *(u8 *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
