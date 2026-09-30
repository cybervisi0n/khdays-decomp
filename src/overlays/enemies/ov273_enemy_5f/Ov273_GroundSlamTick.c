/* Ground-slam tick: the +8 timer accumulates the frame rate. At stage 0 (+0xc), past 1.5 the stage
 * becomes 1 and animation 1 plays. At stage 1 a 8.0-long, 0.375-thick segment from the actor's +0xb0
 * point along data_02042264 is swept over the actor list: every entity whose +2 id bit is clear in
 * the +0xd mask is pushed 0.25 in a random horizontal direction (angle in [-pi, pi) through the
 * sine table, kind 4, on behalf of the +0x384 rider); on acceptance the 14-byte message
 * data_ov273_020d6bb4 carries its +0x74 point to the actor's +0x24 hook, its bit is set and the
 * rider's reaction 0 mode 0x4f fires there. The tick ends once the +0x388 part's rig is idle or
 * the rider's +0x1c4 flags have bit 1 or 3 set; in the latter case the part's animation 0 is
 * advanced by one frame step. Ending requests pose 0. */

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

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

extern const VecFx32 data_02042264;
extern const short data_0203d210[];
extern void Ov107_PostTagUpdate(int owner, int anim, int mode);
extern int Ov107_CollectSegmentOverlaps(int owner, Segment *seg, int *hits);
extern int RandNextScaled(int n);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, void *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern int Obj_GetCellScaledField(int part, int a, int b);
extern void callIfTableEntrySet(int part, int a, int frame);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const Cmd14 data_ov273_020d6bb4;

void Ov273_GroundSlamTick(int *node)
{
    int *state = (int *)node[1];
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;
    int hits[4];
    Segment seg;
    VecFx32 push;
    int n;
    int i;

    state[2] += *(int *)(node[0] + 0x2c);
    if (*((u8 *)state + 0xc) == 0) {
        if (state[2] > 0x1800) {
            *((u8 *)state + 0xc) = 1;
            Ov107_PostTagUpdate(*state, 1, 0);
        }
    } else if (*((u8 *)state + 0xc) == 1) {
        n = *state;
        seg.p0 = *(VecFx32 *)(n + 0xb0);
        seg.dir = data_02042264;
        seg.nLength = 0x8000;
        seg.nRadius = 0x600;
        n = Ov107_CollectSegmentOverlaps(*state, &seg, hits);
        for (i = 0; i < n; i++) {
            Cmd14 msg;
            unsigned int idx;
            int angle;

            if ((*((u8 *)state + 0xd) & (1 << *(u16 *)(hits[i] + 2))) != 0) {
                continue;
            }
            angle = RandNextScaled(0x6489) - 0x3244;
            idx = ANG2IDX(angle);
            push.x = data_0203d210[idx * 2];
            push.y = 0;
            push.z = data_0203d210[idx * 2 + 1];
            ScaleVec3Fx12(0x400, &push, &push);
            if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x384), 4, &push, 0) == 0) {
                continue;
            }
            msg = data_ov273_020d6bb4;
            PACK(msg, scratchX, *(Fx32 *)(hits[i] + 0x74), 5);
            PACK(msg, scratchY, *(Fx32 *)(hits[i] + 0x78), 8);
            PACK(msg, scratchZ, *(Fx32 *)(hits[i] + 0x7c), 11);
            if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
                (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
            }
            *((u8 *)state + 0xd) |= 1 << *(u16 *)(hits[i] + 2);
            Ov107_BuildAndSendUpdate(*(int *)(*state + 0x384), 0, 0x4f, (void *)(hits[i] + 0x74));
        }
    }
    if (*(u8 *)(*(int *)(*state + 0x388) + 0xad) != 0 && (*(u8 *)(*(int *)(*state + 0x384) + 0x1c4) & 0xa) == 0) {
        return;
    }
    if ((*(u8 *)(*(int *)(*state + 0x384) + 0x1c4) & 0xa) != 0) {
        n = Obj_GetCellScaledField(*(int *)(*state + 0x388), 0, 0);
        callIfTableEntrySet(*(int *)(*state + 0x388), 0, n + *(int *)(node[0] + 0x2c));
    }
    *(u8 *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
