/* Lunge tick: the +0x5c speed eases a fifth of the way towards 1.0 and the forward unit vector of
 * the +0x38 rotation (data_02042258) scaled by it becomes the +0xc velocity. A 2.0 sphere around the
 * owner's +0x3ec part point (+0x14), moved by that velocity, sweeps the actor list: every entity
 * whose +2 id bit is clear in the +0x69 mask is pushed 1.0 away from the centre, never downwards
 * (kind 1); on acceptance the 14-byte message data_ov213_020d2e82 carries its +0x74 point to the
 * owner's +0x24 hook, its bit is set and reaction 0x122 mode 7 fires there. The +0x1c distance
 * accumulates the speed; past 16.0 animation 6 plays and the tick hands over to the retreat tick
 * Ov213_RetreatTick. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
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

extern const VecFx32 data_02042258;
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, void *out);
extern void VEC_Add(const void *a, const void *b, void *out);
extern int Ov107_CollectSphereOverlaps(int owner, Sphere *sphere, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const Cmd14 data_ov213_020d2e82;
extern void Ov213_RetreatTick(int *node);

void Ov213_LungeTick(int *node)
{
    int *state = (int *)node[1];
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;
    VecFx32 dir;
    Sphere sphere;
    int hits[4];
    VecFx32 push;
    int n;
    int i;

    state[0x17] += (0x1000 - state[0x17]) / 5;
    Vec3TransformViaTempMtx(&dir, state + 0xe, &data_02042258);
    VEC_Normalize(&dir, &dir);
    ScaleVec3Fx12(state[0x17], &dir, state + 3);
    VEC_Add((void *)(*(int *)(*state + 0x3ec) + 0x14), state + 3, &sphere.center);
    sphere.nRadius = 0x2000;
    n = Ov107_CollectSphereOverlaps(*state, &sphere, hits);
    for (i = 0; i < n; i++) {
        Cmd14 msg;

        if ((*((u8 *)state + 0x69) & (1 << *(u16 *)(hits[i] + 2))) != 0) {
            continue;
        }
        VEC_Subtract((void *)(hits[i] + 0x74), &sphere.center, &push);
        if (push.y < 0) {
            push.y = 0;
        }
        VEC_Normalize(&push, &push);
        ScaleVec3Fx12(0x1000, &push, &push);
        if (Ov107_InvokeHitCallback(hits[i], *state, *state, 1, &push, 0) == 0) {
            continue;
        }
        msg = data_ov213_020d2e82;
        PACK(msg, scratchX, *(Fx32 *)(hits[i] + 0x74), 5);
        PACK(msg, scratchY, *(Fx32 *)(hits[i] + 0x78), 8);
        PACK(msg, scratchZ, *(Fx32 *)(hits[i] + 0x7c), 11);
        if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
            (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
        }
        *((u8 *)state + 0x69) |= 1 << *(u16 *)(hits[i] + 2);
        Ov107_BuildAndSendUpdate(*state, 0x122, 7, (void *)(hits[i] + 0x74));
    }
    state[7] += state[0x17];
    if (state[7] <= 0x10000) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 6, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov213_RetreatTick);
}
