/* Bite tick of an ov257 state: the +0x54 timer accumulates the frame rate and at 0.2 reaction
 * +0x408 mode 0x16 fires once at the +4 point (+0x76). The +0x40 rate is the frame rate x 3;
 * without a nearest target (020cab14, kept in +0x60) sub-state 2 is requested, otherwise the +0x10
 * step heads for it (Ov257_SteerToTarget, never past the gap). The +0x44 timer accumulates the
 * frame rate; between 0.27 and 0.73 the four segments of the owner's +0x3c0 parts (+0x78), moved
 * by the step and with a doubled radius, are swept over the actor list: every entity whose +2 id
 * bit is clear in the +0x73 mask is pushed 0.25 along the +0x1c heading; on acceptance the 14-byte
 * message of data_ov257_020d32a2 carries its +0x74 point to the owner's +0x24 hook, its bit is set
 * and reaction +0x408 mode 9 fires there. Once the +0xc idle byte clears, animation 0xe plays, the
 * +0x3d0 part plays motion 0xd, +0x44, +0x73 and +0x76 clear and the tick hands over to
 * Ov257_SecondBiteTick. */

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

extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern int Ov107_FindNearestObject(int obj, int kind);
extern int Ov257_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_CollectSegmentOverlaps(int owner, Segment *seg, int *hits);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void Ov107_PostTagUpdate(int owner, int anim, int mode);
extern void Ov107_StartAnim(int part, int motion, int mode);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const Cmd14 data_ov257_020d32a2;
extern const VecFx32 data_02042258;
extern void VEC_Add(const void *a, const void *b, void *out);
extern void Ov257_SecondBiteTick(int *node);

static inline int FX_Mul(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov257_BiteTick(int *node)
{
    int i;
    int obj;
    int *pHits;
    int *state = (int *)node[1];
    int k;
    int n;
    int speed;
    VecFx32 dir;
    int hits[4];
    Segment seg;
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;

    state[0x15] += *(int *)(node[0] + 0x2c);
    if (*((u8 *)state + 0x76) == 0 && state[0x15] >= 0x333) {
        Ov107_BuildAndSendUpdate(state[0], (short)*(int *)(*state + 0x408), 0x16, (void *)state[1]);
        *((u8 *)state + 0x76) = 1;
    }
    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 10;
    state[0x18] = Ov107_FindNearestObject(*state, 0);
    if (state[0x18] == 0) {
        *(u8 *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    n = Ov257_SteerToTarget(state, state[0x18], &dir, &speed);
    if (n < speed) {
        speed = n;
    }
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    state[0x11] += *(int *)(node[0] + 0x2c);
    if (state[0x11] > 0x444 && state[0x11] < 0xbbb) {
        for (k = 0; k < 4; k++) {
            seg = *(Segment *)(((int *)*state)[0xf0 + k] + 0x78);
            VEC_Add(&seg.p0, state + 4, &seg.p0);
            seg.nRadius = FX_Mul(seg.nRadius, 0x2000);
            n = Ov107_CollectSegmentOverlaps(*state, &seg, hits);
            i = 0;
            if (i < n) {
                pHits = hits;
                do {
                    VecFx32 push;
                    Cmd14 msg;

                    obj = pHits[i];
                    if ((*((u8 *)state + 0x73) & (1 << *(u16 *)(obj + 2))) == 0) {
                        Vec3TransformViaTempMtx(&push, state + 7, &data_02042258);
                        ScaleVec3Fx12(0x400, &push, &push);
                        if (Ov107_InvokeHitCallback(pHits[i], *state, *state, 0, &push, 0) != 0) {
                            msg = data_ov257_020d32a2;
                            PACK(msg, scratchX, *(Fx32 *)(obj + 0x74), 5);
                            PACK(msg, scratchY, *(Fx32 *)(obj + 0x78), 8);
                            PACK(msg, scratchZ, *(Fx32 *)(obj + 0x7c), 11);
                            if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
                                (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
                            }
                            *((u8 *)state + 0x73) |= 1 << *(u16 *)(pHits[i] + 2);
                            Ov107_BuildAndSendUpdate(*state, (short)*(int *)(*state + 0x408), 9, (void *)(obj + 0x74));
                        }
                    }
                    i++;
                } while (i < n);
            }
        }
    }
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    Ov107_PostTagUpdate(*state, 0xe, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3d0), 0xd, 0);
    state[0x11] = 0;
    *((u8 *)state + 0x73) = 0;
    *((u8 *)state + 0x76) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov257_SecondBiteTick);
}
