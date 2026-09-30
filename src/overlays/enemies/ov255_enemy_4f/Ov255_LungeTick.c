/* Lunge tick of an ov255 state: at 0.5 on the +0x50 timer reaction +0x3f8 mode 8 fires once at
 * the owner's +0x3b0 body point (+0x65). The +0x40 rate follows the frame rate and the +0x10 step
 * heads for the +0x5c target (Ov255_SteerToTarget). The +0x44 timer accumulates the frame rate;
 * past 0.3 the owner's +0x24 hook receives note 3 of data_ov255_020d2b20 once (+0x62), and between
 * 0.2 and 0.37 the owner's +0x3a0 segment (+0x78, radius 1.0) is swept over the actor list: every
 * entity whose +2 id bit is clear in the +0x63 mask is pushed 2.0 upwards (kind 4); on acceptance
 * the 14-byte message of data_ov255_020d2b60 carries its +0x74 point to the hook, its bit is set
 * and reaction +0x3f8 mode 9 fires there. Once the +0xc idle byte clears, the +0x54 cooldown is
 * re-rolled in [+0x224, +0x228] and sub-state 2 is requested. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int value; } Fx32;
typedef struct { u16 lo; u16 hi; } Cmd4;
typedef struct { u16 id; u8 kind; u8 cmd; u8 flag; u8 pos[9]; } Cmd14;
typedef struct { VecFx32 p0; VecFx32 dir; int nLength; int nRadius; } Segment;

#define PACK(cmd, dead, src, at)                                              \
    (dead) = (src);                                                           \
    ((u8 *)&(cmd))[at] = (u8)(((unsigned int)(dead).value >> 0x10 & 0x7f)     \
                              | ((unsigned int)(dead).value >> 0x18 & 0x80)); \
    ((u8 *)&(cmd))[(at) + 1] = (u8)((unsigned int)(dead).value >> 8);         \
    ((u8 *)&(cmd))[(at) + 2] = (u8)(dead).value

extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern int Ov255_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_CollectSegmentOverlaps(int owner, Segment *seg, int *hits);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const Cmd14 data_ov255_020d2b60;
extern const Cmd4 data_ov255_020d2b20[];
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);

static inline int RandRange(int lo, int hi)
{
    int d = hi - lo;

    if (d < 0) {
        d = -d;
    }
    return lo + RandNextScaled(d + 1);
}

void Ov255_LungeTick(int *node)
{
    int obj;
    int *state = (int *)node[1];
    VecFx32 dir;
    int hits[4];
    Segment seg;
    int speed;
    Cmd4 note;
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;
    int n;
    int i;

    state[0x14] += *(int *)(node[0] + 0x2c);
    if (*((u8 *)state + 0x65) == 0 && state[0x14] >= 0x800) {
        Ov107_BuildAndSendUpdate(*state, (short)*(int *)(*state + 0x3f8), 8, (void *)(*(int *)(*state + 0x3b0) + 0x14));
        *((u8 *)state + 0x65) = 1;
    }
    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 30;
    Ov255_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    state[0x11] += *(int *)(node[0] + 0x2c);
    if (*((u8 *)state + 0x62) == 0 && state[0x11] > 0x4cc) {
        Cmd4 *p = &note;

        p->hi = data_ov255_020d2b20[3].hi;
        p->lo = data_ov255_020d2b20[3].lo;
        if (*(void (**)(int, Cmd4 *, int))(*state + 0x24) != 0) {
            (*(void (**)(int, Cmd4 *, int))(*state + 0x24))(*state, p, 4);
        }
        *((u8 *)state + 0x62) = 1;
    }
    n = state[0x11];
    if (n > 0x333 && n < 0x5dd) {
        seg = *(Segment *)(*(int *)(*state + 0x3a0) + 0x78);
        seg.nRadius = 0x1000;
        n = Ov107_CollectSegmentOverlaps(*state, &seg, hits);
        for (i = 0; i < n; i++) {
            VecFx32 push;
            Cmd14 msg;

            obj = hits[i];
            if ((*((u8 *)state + 0x63) & (1 << *(u16 *)(obj + 2))) != 0) {
                continue;
            }
            push.x = 0;
            push.y = 0x2000;
            push.z = 0;
            if (Ov107_InvokeHitCallback(obj, *state, *state, 4, &push, 0) == 0) {
                continue;
            }
            msg = data_ov255_020d2b60;
            PACK(msg, scratchX, *(Fx32 *)(obj + 0x74), 5);
            PACK(msg, scratchY, *(Fx32 *)(obj + 0x78), 8);
            PACK(msg, scratchZ, *(Fx32 *)(obj + 0x7c), 11);
            if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
                (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
            }
            *((u8 *)state + 0x63) |= 1 << *(u16 *)(hits[i] + 2);
            Ov107_BuildAndSendUpdate(*state, (short)*(int *)(*state + 0x3f8), 9, (void *)(obj + 0x74));
        }
    }
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    state[0x15] = RandRange(*(int *)(*state + 0x224), *(int *)(*state + 0x228));
    *(u8 *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
