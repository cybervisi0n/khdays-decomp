/* Tail-sweep tick: the owner's +0x3ec part is the sweeping limb. Until the +0x6a flag is set the
 * +0x70 timer accumulates the frame rate and at 0.8 the flag is set and reaction 0x122 mode 0xe fires
 * at the part point (+0x14). The +0x1c timer accumulates the frame rate; between 0.77 and 0.97 a
 * 1.4-long, 1.0-thick segment from the part point along the part's +4 rotation (data_02042258) is
 * swept over the actor list: every entity whose +2 id bit is clear in the +0x69 mask is pushed 1.0
 * away from the owner, never downwards (kind 0); on acceptance the 14-byte message
 * data_ov213_020d2eac carries its +0x74 point to the owner's +0x24 hook, its bit is set and
 * reaction 0x122 mode 7 fires there. Once the +8 idle byte clears, pose 5 is requested. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int value; } Fx32;
typedef struct { u16 id; u8 kind; u8 cmd; u8 flag; u8 pos[9]; } Cmd14;
typedef struct { VecFx32 p0; VecFx32 dir; int nLength; int nRadius; } Segment;

#define PACK(cmd, dead, src, at)                                              \
    (dead) = (src);                                                           \
    ((u8 *)&(cmd))[at] = (u8)(((unsigned int)(dead).value >> 0x10 & 0x7f)     \
                              | ((unsigned int)(dead).value >> 0x18 & 0x80)); \
    ((u8 *)&(cmd))[(at) + 1] = (u8)((unsigned int)(dead).value >> 8);         \
    ((u8 *)&(cmd))[(at) + 2] = (u8)(dead).value

extern const VecFx32 data_02042258;
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern int Ov107_CollectSegmentOverlaps(int owner, Segment *seg, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, void *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const Cmd14 data_ov213_020d2eac;

void Ov213_TailSweepTick(int *node)
{
    int obj;
    int *state = (int *)node[1];
    int part = *(int *)(*state + 0x3ec);
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;
    int hits[4];
    Segment seg;
    VecFx32 push;
    int n;
    int i;

    if (*((u8 *)state + 0x6a) == 0) {
        state[0x1c] += *(int *)(node[0] + 0x2c);
        if (state[0x1c] >= 0xccc) {
            *((u8 *)state + 0x6a) = 1;
            Ov107_BuildAndSendUpdate(*state, 0x122, 0xe, (void *)(part + 0x14));
        }
    }
    state[7] += *(int *)(node[0] + 0x2c);
    n = state[7];
    if (n > 0xc44 && n < 0xf77) {
        seg.p0 = *(VecFx32 *)(part + 0x14);
        Vec3TransformViaTempMtx(&seg.dir, (void *)(part + 4), &data_02042258);
        seg.nLength = 0x1666;
        seg.nRadius = 0x1000;
        n = Ov107_CollectSegmentOverlaps(*state, &seg, hits);
        for (i = 0; i < n; i++) {
            Cmd14 msg;

            obj = hits[i];
            if ((*((u8 *)state + 0x69) & (1 << *(u16 *)(obj + 2))) != 0) {
                continue;
            }
            VEC_Subtract((void *)(obj + 0x74), (void *)(*state + 0x74), &push);
            if (push.y < 0) {
                push.y = 0;
            }
            VEC_Normalize(&push, &push);
            ScaleVec3Fx12(0x1000, &push, &push);
            if (Ov107_InvokeHitCallback(hits[i], *state, *state, 0, &push, 0) == 0) {
                continue;
            }
            msg = data_ov213_020d2eac;
            PACK(msg, scratchX, *(Fx32 *)(obj + 0x74), 5);
            PACK(msg, scratchY, *(Fx32 *)(obj + 0x78), 8);
            PACK(msg, scratchZ, *(Fx32 *)(obj + 0x7c), 11);
            if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
                (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
            }
            *((u8 *)state + 0x69) |= 1 << *(u16 *)(hits[i] + 2);
            Ov107_BuildAndSendUpdate(*state, 0x122, 7, (void *)(obj + 0x74));
        }
    }
    if (*(u8 *)state[2] != 0) {
        return;
    }
    *(u8 *)(*state + 0x1c7) = 5;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
