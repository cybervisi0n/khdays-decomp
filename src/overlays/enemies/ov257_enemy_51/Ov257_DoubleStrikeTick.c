/* Double-strike tick of an ov257 state: the +0x54 timer accumulates the frame rate and at 0.53
 * reaction +0x408 mode 0x1d fires once at the +4 point (+0x76). The +0x40 rate is the frame rate
 * x 0.6; without a nearest target (020cab14, kept in +0x60) sub-state 2 is requested, otherwise the
 * +0x10 step heads for it (Ov257_SteerToTarget). The +0x44 timer accumulates the frame rate; between
 * 0.5 and 1.0 the segment of the owner's +0x3c0 part (+0x78), moved by the step and with a doubled
 * radius, is swept over the actor list, and between 1.0 and 1.5 the +0x3c4 part's: every entity
 * whose +2 id bit is clear in that sweep's mask (+0x74 / +0x75) is pushed 0.25 away from the
 * segment start (kind 6); on acceptance the sweep's 14-byte message (data_ov257_020d32cc /
 * data_ov257_020d3304) carries its +0x74 point to the owner's +0x24 hook, its bit is set and
 * reaction +0x408 mode 0x20 fires there. Once the +0xc idle byte clears, animation 0x20 plays, the
 * +0x3d0 part plays motion 0x1d, +0x44, +0x74, +0x75 and +0x76 clear and the tick hands over to
 * Ov257_FollowUpStrikeTick. */

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

extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern int Ov107_FindNearestObject(int obj, int kind);
extern int Ov257_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const void *a, const void *b, void *out);
extern int Ov107_CollectSegmentOverlaps(int owner, Segment *seg, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void Ov107_PostTagUpdate(int owner, int anim, int mode);
extern void Ov107_StartAnim(int part, int motion, int mode);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const Cmd14 data_ov257_020d32cc;
extern const Cmd14 data_ov257_020d3304;
extern void Ov257_FollowUpStrikeTick(int *node);

static inline int FX_Mul(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov257_DoubleStrikeTick(int *node)
{
    int obj;
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;
    Fx32 scratch2Z;
    Fx32 scratch2Y;
    Fx32 scratch2X;
    int n;
    int i;
    int sweepObj;
    int j;
    int count;

    state[0x15] += *(int *)(node[0] + 0x2c);
    if (*((u8 *)state + 0x76) == 0 && state[0x15] >= 0x888) {
        Ov107_BuildAndSendUpdate(state[0], (short)*(int *)(*state + 0x408), 0x1d, (void *)state[1]);
        *((u8 *)state + 0x76) = 1;
    }
    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 50;
    state[0x18] = Ov107_FindNearestObject(*state, 0);
    if (state[0x18] == 0) {
        *(u8 *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    Ov257_SteerToTarget(state, state[0x18], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    state[0x11] += *(int *)(node[0] + 0x2c);
    n = state[0x11];
    if (n > 0x800 && n < 0xfff) {
        int hits[4];
        Segment seg;

        seg = *(Segment *)(*(int *)(*state + 0x3c0) + 0x78);
        VEC_Add(&seg.p0, state + 4, &seg.p0);
        seg.nRadius = FX_Mul(seg.nRadius, 0x2000);
        n = Ov107_CollectSegmentOverlaps(*state, &seg, hits);
        for (i = 0; i < n; i++) {
            VecFx32 push;
            Cmd14 msg;

            obj = hits[i];
            if ((*((u8 *)state + 0x74) & (1 << *(u16 *)(obj + 2))) != 0) {
                continue;
            }
            VEC_Subtract((void *)(obj + 0x74), &seg.p0, &push);
            VEC_Normalize(&push, &push);
            ScaleVec3Fx12(0x400, &push, &push);
            if (Ov107_InvokeHitCallback(hits[i], *state, *state, 6, &push, 0) == 0) {
                continue;
            }
            msg = data_ov257_020d32cc;
            PACK(msg, scratchX, *(Fx32 *)(obj + 0x74), 5);
            PACK(msg, scratchY, *(Fx32 *)(obj + 0x78), 8);
            PACK(msg, scratchZ, *(Fx32 *)(obj + 0x7c), 11);
            if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
                (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
            }
            *((u8 *)state + 0x74) |= 1 << *(u16 *)(hits[i] + 2);
            Ov107_BuildAndSendUpdate(*state, (short)*(int *)(*state + 0x408), 0x20, (void *)(obj + 0x74));
        }
    }
    if (state[0x11] > 0x1000 && state[0x11] < 0x17ff) {
        int hits[4];
        Segment seg;

        seg = *(Segment *)(*(int *)(*state + 0x3c4) + 0x78);
        VEC_Add(&seg.p0, state + 4, &seg.p0);
        seg.nRadius = FX_Mul(seg.nRadius, 0x2000);
        count = Ov107_CollectSegmentOverlaps(*state, &seg, hits);
        for (j = 0; j < count; j++) {
            VecFx32 push;
            Cmd14 msg;

            sweepObj = hits[j];
            if ((*((u8 *)state + 0x75) & (1 << *(u16 *)(sweepObj + 2))) != 0) {
                continue;
            }
            VEC_Subtract((void *)(sweepObj + 0x74), &seg.p0, &push);
            VEC_Normalize(&push, &push);
            ScaleVec3Fx12(0x400, &push, &push);
            if (Ov107_InvokeHitCallback(hits[j], *state, *state, 6, &push, 0) == 0) {
                continue;
            }
            msg = data_ov257_020d3304;
            PACK(msg, scratch2X, *(Fx32 *)(sweepObj + 0x74), 5);
            PACK(msg, scratch2Y, *(Fx32 *)(sweepObj + 0x78), 8);
            PACK(msg, scratch2Z, *(Fx32 *)(sweepObj + 0x7c), 11);
            if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
                (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
            }
            *((u8 *)state + 0x75) |= 1 << *(u16 *)(hits[j] + 2);
            Ov107_BuildAndSendUpdate(*state, (short)*(int *)(*state + 0x408), 0x20, (void *)(sweepObj + 0x74));
        }
    }
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    Ov107_PostTagUpdate(*state, 0x20, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3d0), 0x1d, 0);
    state[0x11] = 0;
    *((u8 *)state + 0x74) = 0;
    *((u8 *)state + 0x75) = 0;
    *((u8 *)state + 0x76) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov257_FollowUpStrikeTick);
}
