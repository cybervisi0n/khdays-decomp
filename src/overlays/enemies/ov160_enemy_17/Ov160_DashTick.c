/* Dash tick of the ov160 enemy. The +0x28 distance first takes the owner's rate; the +0x14
 * rotation turns data_02042258 into the forward direction, which scaled by the +0x24 length
 * becomes the +8 vector, and the length eases a thirtieth towards 0x400. The segment from the
 * +4 point along the forward direction (length, radius 1.0) is swept over the actor list of
 * the +0x38c item: the first candidate accepted along the zero vector (flag 0x200) whose +0x18c
 * body exists is kept in +0x2c, started (ov022 ad8e0, 1), +0x30 is cleared and the tick hands
 * over to Ov160_GrabTick. Otherwise, with a target ahead (dot product of the forward
 * direction and the unit direction to its +0x74 not negative), the +0x14 rotation slerps
 * (f55c) by 30 x rate x 1/16 towards the rotation facing it and is normalised. The distance
 * then advances by the length and, once the object reports contact or the distance passes
 * 30.0, the overlay's 14-byte position message (data_ov160_020cf7c8, the +4 point packed as
 * 24-bit values, byte 4 set above 1.0 + 0x100) goes to the +0x24 hook, reaction 0x151 mode 7
 * fires at the +4 point, the sub-state clears and the state ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int value; } Fx32;
typedef struct { u8 hi, mid, lo; } Fx24;
typedef struct { int q[4]; } Quat;
typedef struct { VecFx32 origin; VecFx32 dir; int nLength; int nRadius; } Segment;

typedef struct {
    u16 id;             /* +0x0 */
    u8 kind;            /* +0x2 */
    u8 cmd;             /* +0x3 */
    u8 flag;            /* +0x4 */
    Fx24 pos[3];        /* +0x5 */
} PosMsg;

struct Ov160Contact { u8 bGrounded : 1, bBlocked : 1; };

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

static inline void PackFx24(Fx24 *dst, int v) {
    dst->hi = ((u32)v >> 16 & 0x7f) | ((u32)v >> 24 & 0x80);
    dst->mid = (u32)v >> 8;
    dst->lo = v;
}

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_CollectSegmentOverlaps(int item, Segment *query, int *results);
extern int Ov107_InvokeHitCallback(int ent, int actor, int item, int mode, const VecFx32 *dir, int flag);
extern void Ov022_ToggleBit13ByMode(int effect, int a);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int Ov107_FindNearestObject(int owner, int flag);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern void Quat_Slerp(Quat *out, int t, Quat *a, Quat *b);
extern void Ov107_BuildAndSendUpdate(int item, int id, int mode, void *at);
extern const VecFx32 data_02042258;
extern const VecFx32 data_02041dc8;
extern void Ov160_GrabTick(int *node);
extern const PosMsg data_ov160_020cf7c8;

void Ov160_DashTick(int *node)
{
    int *state = (int *)node[1];
    Segment query;
    VecFx32 fwd;
    int results[4];
    Quat q;
    VecFx32 d;
    PosMsg msg;
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;
    Fx32 *pPos;
    int target;
    int i;
    int n;

    state[0xa] += *(int *)(node[0] + 0x2c);
    Vec3TransformViaTempMtx(&fwd, state + 5, &data_02042258);
    ScaleVec3Fx12(state[9], &fwd, (VecFx32 *)(state + 2));
    state[9] += (0x400 - state[9]) / 30;
    query.origin = *(VecFx32 *)state[1];
    query.dir = fwd;
    query.nLength = state[9];
    query.nRadius = 0x1000;
    n = Ov107_CollectSegmentOverlaps(*(int *)(*state + 0x38c), &query, results);
    for (i = 0; i < n; i++) {
        if (Ov107_InvokeHitCallback(results[i], *state, *(int *)(*state + 0x38c), 1, &data_02041dc8, 0x200) != 0) {
            state[0xb] = *(int *)(results[i] + 0x18c);
            if (state[0xb] != 0) {
                Ov022_ToggleBit13ByMode(state[0xb], 1);
                state[0xc] = 0;
                SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov160_GrabTick);
                return;
            }
        }
    }
    target = Ov107_FindNearestObject(*state, 0);
    if (target != 0) {
        VEC_Subtract((VecFx32 *)(target + 0x74), (VecFx32 *)state[1], &d);
        VEC_Normalize(&d, &d);
        Quat_FromTwoVectors(&q, &data_02042258, &d);
        if (VEC_DotProduct(&fwd, &d) >= 0) {
            Quat_Slerp((Quat *)(state + 5), FX_Mul(*(int *)(node[0] + 0x2c) * 30, 0x100), (Quat *)(state + 5), &q);
            Vec4_Normalize((Quat *)(state + 5), (Quat *)(state + 5));
        }
    }
    state[0xa] += state[9];
    if (((struct Ov160Contact *)(*state + 0x17a))->bGrounded == 0
        && ((struct Ov160Contact *)(*state + 0x17a))->bBlocked == 0
        && state[0xa] <= 0x1e000) {
        return;
    }
    msg = data_ov160_020cf7c8;
    pPos = (Fx32 *)state[1];
    scratchX = pPos[0];
    PackFx24(&msg.pos[0], scratchX.value);
    scratchY = pPos[1];
    PackFx24(&msg.pos[1], scratchY.value);
    scratchZ = pPos[2];
    PackFx24(&msg.pos[2], scratchZ.value);
    if (*(int *)(*state + 0x13c) - 0x1000 > 0x100) {
        msg.flag = 1;
    }
    if (*(void (**)(int, PosMsg *, int))(*state + 0x24) != 0) {
        (*(void (**)(int, PosMsg *, int))(*state + 0x24))(*state, &msg, 0xe);
    }
    Ov107_BuildAndSendUpdate(*(int *)(*state + 0x38c), 0x151, 7, (void *)state[1]);
    *(u8 *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
