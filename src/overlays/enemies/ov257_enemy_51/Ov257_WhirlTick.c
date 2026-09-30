/* Whirl tick of an ov257 state: the +0x54 timer accumulates the frame rate and at 2.53 reaction
 * +0x408 mode 0x19 fires once at the +4 point (+0x76). The +0x40 rate clears and the +0x10 step heads
 * for the +0x60 target (Ov257_SteerToTarget). The +0x44 timer accumulates the frame rate; between
 * 0.33 and 0.83 the segment of each of the owner's four parts (+0x3c0..+0x3cc, +0x78), moved by the
 * step and with its radius x 1.5, is swept over the actor list: every entity whose +2 id bit is
 * clear in the +0x73 mask is pushed 1.0 away from the segment start (kind 3); on acceptance the
 * 14-byte message data_ov257_020d32e8 carries its +0x74 point to the owner's +0x24 hook, its bit is
 * set and reaction +0x408 mode 9 fires there. Once the +0xc idle byte clears, the +0x4c delay is
 * drawn from the owner's [+0x224, +0x228] range and sub-state 2 is requested. */

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

extern int Ov257_SteerToTarget(int *state, int target, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const void *a, const void *b, void *out);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int Ov107_CollectSegmentOverlaps(int owner, Segment *seg, int *hits);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const Cmd14 data_ov257_020d32e8;

static inline int RandRange(int lo, int hi)
{
    int d = hi - lo;

    if (d < 0) {
        d = -d;
    }
    return lo + RandNextScaled(d + 1);
}

static inline int FX_Mul(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov257_WhirlTick(int *node)
{
    int obj;
    int *pHits;
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;
    int part;
    int n;
    int i;

    state[0x15] += *(int *)(node[0] + 0x2c);
    if (*((u8 *)state + 0x76) == 0 && state[0x15] >= 0x2888) {
        Ov107_BuildAndSendUpdate(state[0], (short)*(int *)(*state + 0x408), 0x19, (void *)state[1]);
        *((u8 *)state + 0x76) = 1;
    }
    state[0x10] = 0;
    Ov257_SteerToTarget(state, state[0x18], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    state[0x11] += *(int *)(node[0] + 0x2c);
    n = state[0x11];
    if (n > 0x555 && n < 0xd55) {
        for (part = 0; part < 4; part++) {
            int hits[4];
            Segment seg;

            seg = *(Segment *)(((int *)*state)[0xf0 + part] + 0x78);
            VEC_Add(&seg.p0, state + 4, &seg.p0);
            seg.nRadius = FX_Mul(seg.nRadius, 0x1800);
            n = Ov107_CollectSegmentOverlaps(*state, &seg, hits);
            i = 0;
            if (i < n) {
                pHits = hits;
                do {
                    VecFx32 push;
                    Cmd14 msg;

                    obj = pHits[i];
                    if ((*((u8 *)state + 0x73) & (1 << *(u16 *)(obj + 2))) == 0) {
                        VEC_Subtract((void *)(obj + 0x74), &seg.p0, &push);
                        VEC_Normalize(&push, &push);
                        ScaleVec3Fx12(0x1000, &push, &push);
                        if (Ov107_InvokeHitCallback(pHits[i], *state, *state, 3, &push, 0) != 0) {
                            msg = data_ov257_020d32e8;
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
    state[0x13] = RandRange(*(int *)(*state + 0x224), *(int *)(*state + 0x228));
    *(u8 *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
