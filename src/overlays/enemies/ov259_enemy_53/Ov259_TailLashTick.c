/* Tail-lash tick: the segment of the owner's +0x384 part (+0x78), moved to 0.19 above the actor's
 * +0x74 point and lengthened by 2.56, is swept over the actor list on behalf of the +0x38c owner. The
 * first entity that accepts a 0.125 push away from the part's +4 point, lifted by 0.375 (kind 1),
 * gets effect 0xb at its +0x74 point and pose 0 is requested (the original also scales and adds two
 * uninitialised scratch vectors there, whose result is unused). Otherwise the +0x24 timer
 * accumulates the frame rate and past 0.6, or once blocked (+0x17a bit 1), pose 0 is requested. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 p0; VecFx32 dir; int nLength; int nRadius; } Segment;
struct Bits17a { unsigned char b0 : 1, b1 : 1; };

extern int Ov107_CollectSegmentOverlaps(int owner, Segment *seg, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const void *a, const void *b, void *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov259_TailLashTick(int *node)
{
    int *state = (int *)node[1];
    Segment seg;
    VecFx32 pos;
    int hits[4];
    VecFx32 push;
    VecFx32 b;
    VecFx32 a;
    int i;
    int n;

    seg = *(Segment *)(**(int **)(*state + 0x384) + 0x78);
    pos = *(VecFx32 *)(*state + 0x74);
    pos.y += 0x300;
    seg.p0 = pos;
    seg.nLength += 0x2900;
    n = Ov107_CollectSegmentOverlaps(*(int *)(*state + 0x38c), &seg, hits);
    for (i = 0; i < n; i++) {
        VEC_Subtract((void *)(hits[i] + 0x74), (void *)(**(int **)(*state + 0x384) + 4), &push);
        VEC_Normalize(&push, &push);
        ScaleVec3Fx12(0x200, &push, &push);
        push.y = 0x600;
        if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x38c), 1, &push, 0) == 0) {
            continue;
        }
        ScaleVec3Fx12(seg.nRadius, &a, &a);
        VEC_Add(&a, &b, &a);
        func_ov107_020c0b90(*(int *)(*state + 0x38c), 0xb, *(VecFx32 *)(hits[i] + 0x74), 0);
        *(u8 *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    state[9] += *(int *)(node[0] + 0x2c);
    if (state[9] > 0x990) {
        *(u8 *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    if (((struct Bits17a *)(*state + 0x17a))->b1 == 0) {
        return;
    }
    *(u8 *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
