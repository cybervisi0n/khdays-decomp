/* Dash tick of an ov235 state: the +0x40 rate is the frame rate x 7.5 and the +0x10 step is the
 * +0x68 direction x 2.0. The segment from the owner's +0x3ac body point (+0x14) along the step is
 * swept over the actor list; every entity whose +0x1b4 kind bit is clear in the +0x63 mask is
 * pushed away from its closest point on the segment (flattened, then raised to 0.5 up, unit
 * length, kind 6). On acceptance the 14-byte message of data_ov235_020d2550 carries the entity's
 * +0x74 point to the owner's +0x24 hook, the entity becomes +0x74, its bit is set and reaction
 * +0x3c8 mode 0xa fires there. The +0x44 timer accumulates the frame rate; past 0.25 the hook
 * receives note 4 of data_ov235_020d24d0, animation 0x21 plays and the tick hands over to
 * Ov235_HoverTick. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

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

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int Ov107_CollectSegmentOverlaps(int owner, Segment *seg, int *hits);
extern int Segment_ClosestPoint(void *point, Segment *seg, fx64 *outDist);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void Ov107_PostTagUpdate(int owner, int anim, int mode);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const Cmd14 data_ov235_020d2550;
extern const Cmd4 data_ov235_020d24d0[];
extern void Ov235_HoverTick(int *node);

void Ov235_DashTick(int *node)
{
    int obj;
    int *state = (int *)node[1];
    Segment seg;
    int hits[4];
    fx64 t;
    Cmd4 note;
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;
    int n;
    int i;

    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 4;
    ScaleVec3Fx12(0x2000, (VecFx32 *)(state + 0x1a), (VecFx32 *)(state + 4));
    seg.p0 = *(VecFx32 *)(*(int *)(*state + 0x3ac) + 0x14);
    seg.nLength = VEC_Normalize((VecFx32 *)(state + 4), &seg.dir);
    seg.nRadius = 0xc00;
    n = Ov107_CollectSegmentOverlaps(*state, &seg, hits);
    i = 0;
    if (n > 0) {
        do {
            VecFx32 push = {0, 0, 0};
            VecFx32 proj;
            Cmd14 msg;

            obj = hits[i];
            if ((*((u8 *)state + 0x63) & (1 << *(u8 *)(obj + 0x1b4))) == 0) {
                Segment_ClosestPoint((void *)(obj + 0x74), &seg, &t);
                proj.x = (int)((t * seg.dir.x + 0x80000000LL) >> 32);
                proj.y = (int)((t * seg.dir.y + 0x80000000LL) >> 32);
                proj.z = (int)((t * seg.dir.z + 0x80000000LL) >> 32);
                VEC_Add(&seg.p0, &proj, &proj);
                VEC_Subtract((void *)(obj + 0x74), &proj, &push);
                push.y = 0;
                VEC_Normalize(&push, &push);
                push.y = 0x800;
                VEC_Normalize(&push, &push);
                ScaleVec3Fx12(0x1000, &push, &push);
                if (Ov107_InvokeHitCallback(hits[i], *state, *state, 6, &push, 0) != 0) {
                    msg = data_ov235_020d2550;
                    PACK(msg, scratchX, *(Fx32 *)(obj + 0x74), 5);
                    PACK(msg, scratchY, *(Fx32 *)(obj + 0x78), 8);
                    PACK(msg, scratchZ, *(Fx32 *)(obj + 0x7c), 11);
                    if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
                        (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
                    }
                    state[0x1d] = hits[i];
                    *((u8 *)state + 0x63) |= 1 << *(u8 *)(hits[i] + 0x1b4);
                    Ov107_BuildAndSendUpdate(*state, (short)*(int *)(*state + 0x3c8), 0xa, (void *)(obj + 0x74));
                }
            }
        } while (++i < n);
    }
    state[0x11] += *(int *)(node[0] + 0x2c);
    if (state[0x11] <= 0x400) {
        return;
    }
    {
        Cmd4 *p = &note;

        p->hi = data_ov235_020d24d0[4].hi;
        p->lo = data_ov235_020d24d0[4].lo;
        if (*(void (**)(int, Cmd4 *, int))(*state + 0x24) != 0) {
            (*(void (**)(int, Cmd4 *, int))(*state + 0x24))(*state, p, 4);
        }
    }
    Ov107_PostTagUpdate(*state, 0x21, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov235_HoverTick);
}
