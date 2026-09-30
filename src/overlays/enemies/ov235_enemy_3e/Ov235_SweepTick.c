/* Sweep tick of an ov235 state (the cf7b8 shape): at 0.5 on the +0x54 timer reaction +0x3c8
 * mode 8 fires once at the owner's +0x3b4 body point (+0x65). The +0x40 rate follows the frame
 * rate and the +0x10 step heads for the +0x5c target (Ov235_SteerToTarget). The +0x44 timer
 * accumulates the frame rate; between 0.13 and 0.57 the segment from the owner's +0x74 centre
 * along the step (radius 2.25) is swept over the actor list: every entity whose +2 id bit is clear
 * in the +0x63 mask is pushed 5.0 away from the centre (kind 4); on acceptance the 14-byte message
 * of data_ov235_020d2518 carries its +0x74 point to the owner's +0x24 hook, its bit is set and
 * reaction +0x3c8 mode 9 fires there. Once the +0xc idle byte clears, the +0x4c cooldown is
 * re-rolled in [+0x224, +0x228] and sub-state 2 is requested. */

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
extern int Ov235_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_CollectSegmentOverlaps(int owner, Segment *seg, int *hits);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const Cmd14 data_ov235_020d2518;
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

void Ov235_SweepTick(int *node)
{
    int obj;
    int *state = (int *)node[1];
    VecFx32 dir;
    int hits[4];
    Segment seg;
    int speed;
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;
    int n;
    int i;

    state[0x15] += *(int *)(node[0] + 0x2c);
    if (*((u8 *)state + 0x65) == 0 && state[0x15] >= 0x800) {
        Ov107_BuildAndSendUpdate(*state, (short)*(int *)(*state + 0x3c8), 8, (void *)(*(int *)(*state + 0x3b4) + 0x14));
        *((u8 *)state + 0x65) = 1;
    }
    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 30;
    Ov235_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    state[0x11] += *(int *)(node[0] + 0x2c);
    if (state[0x11] > 0x222 && state[0x11] < 0x911) {
        seg.p0 = *(VecFx32 *)(*state + 0x74);
        n = VEC_Normalize((VecFx32 *)(state + 4), &seg.dir);
        seg.nLength = n;
        seg.nRadius = 0x2400;
        n = Ov107_CollectSegmentOverlaps(*state, &seg, hits);
        for (i = 0; i < n; i++) {
            VecFx32 push;
            Cmd14 msg;

            obj = hits[i];
            if ((*((u8 *)state + 0x63) & (1 << *(u16 *)(obj + 2))) != 0) {
                continue;
            }
            VEC_Subtract((void *)(obj + 0x74), &seg.p0, &push);
            VEC_Normalize(&push, &push);
            ScaleVec3Fx12(0x5000, &push, &push);
            if (Ov107_InvokeHitCallback(hits[i], *state, *state, 4, &push, 0) == 0) {
                continue;
            }
            msg = data_ov235_020d2518;
            PACK(msg, scratchX, *(Fx32 *)(obj + 0x74), 5);
            PACK(msg, scratchY, *(Fx32 *)(obj + 0x78), 8);
            PACK(msg, scratchZ, *(Fx32 *)(obj + 0x7c), 11);
            if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
                (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
            }
            *((u8 *)state + 0x63) |= 1 << *(u16 *)(hits[i] + 2);
            Ov107_BuildAndSendUpdate(*state, (short)*(int *)(*state + 0x3c8), 9, (void *)(obj + 0x74));
        }
    }
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    state[0x13] = RandRange(*(int *)(*state + 0x224), *(int *)(*state + 0x228));
    *(u8 *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
