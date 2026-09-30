/* Thrown-spear tick: the +0x24 travel grows by the +0x20 speed x the frame rate x 30 and the +8
 * velocity is the +0x14 heading at that speed. While not stuck (+0x44), the step segment from the +4
 * point (0.125 thick) sweeps the actor list on behalf of the +0x388 owner: the first valid target
 * (Ov253_IdIsFree) that accepts a 1/16 push along the flight (kind 2) stops the spear, the
 * owner's reaction 0x16b mode 6 fires at the +4 point and pose 2 is requested. Past 2.0 the flight
 * ray is also tested against the hit shapes (+0x144 list) of the stage's +0x80 objects that are
 * solid (+0x60 bit 0), not ghosted (bit 7) and not disabled (+0x1ac bit 2), with the same result.
 * Past 45.0, or once blocked/grounded (+0x17a bits 0, 1, 3), the owner's reaction mode 7 fires and
 * pose 2 is requested. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 p0; VecFx32 dir; int nLength; int nRadius; } Segment;
typedef struct { VecFx32 p0; VecFx32 dir; int nLength; } Ray;
typedef struct { u16 lo : 8; u16 hi : 8; } Hw60;
struct Bits17a { unsigned char b0 : 1, b1 : 1, b2 : 1, b3 : 1; };

extern const VecFx32 data_02041dc8;
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int Ov107_CollectSegmentOverlaps(int owner, Segment *seg, int *hits);
extern int Ov253_IdIsFree(int owner, int hit);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int *List_First(void *list);
extern int *List_Next(void *list);
extern int Ov107_HitShape_IntersectSegment(int shape, Ray *ray, VecFx32 *out);

static inline int FX_Mul(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov253_ThrownSpearTick(int *node)
{
    int *state = (int *)node[1];
    int hits[4];
    Segment seg;
    int i;
    int n;

    state[9] += FX_Mul(state[8], *(int *)(node[0] + 0x2c) * 30);
    ScaleVec3Fx12(state[8], (VecFx32 *)(state + 5), (VecFx32 *)(state + 2));
    if (state[0x11] == 0) {
        seg.p0 = *(VecFx32 *)state[1];
        seg.nLength = VEC_Normalize((VecFx32 *)(state + 2), &seg.dir);
        seg.nRadius = 0x200;
        n = Ov107_CollectSegmentOverlaps(*(int *)(*state + 0x388), &seg, hits);
        for (i = 0; i < n; i++) {
            VecFx32 push;

            if (Ov253_IdIsFree(*(int *)(*state + 0x388), hits[i]) == 0) {
                continue;
            }
            VEC_Normalize((VecFx32 *)(state + 2), &push);
            ScaleVec3Fx12(0x100, &push, &push);
            if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x388), 2, &push, 0) == 0) {
                continue;
            }
            *(VecFx32 *)(state + 2) = data_02041dc8;
            Ov107_BuildAndSendUpdate(*(int *)(state[0] + 0x388), 0x16b, 6, (void *)state[1]);
            *(u8 *)(*state + 0x1c7) = 2;
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
            return;
        }
    }
    if (state[9] >= 0x2000) {
        int *link;
        int obj;
        int grid;
        Ray ray;
        int *sub;

        grid = *(int *)(*state + 4);
        ray.p0 = *(VecFx32 *)state[1];
        ray.nLength = VEC_Normalize((VecFx32 *)(state + 2), &ray.dir);
        for (link = List_First((void *)(grid + 0x80)); link != 0; link = List_Next((void *)(grid + 0x80))) {
            obj = *link;
            if ((((Hw60 *)(obj + 0x60))->lo & 1) == 0 || (((Hw60 *)(obj + 0x60))->lo & 0x80) != 0
                || (*(u16 *)(obj + 0x1ac) & 4) != 0) {
                continue;
            }
            for (sub = List_First((void *)(obj + 0x144)); sub != 0; sub = List_Next((void *)(obj + 0x144))) {
                if (Ov107_HitShape_IntersectSegment(*sub, &ray, 0) != 0) {
                    *(VecFx32 *)(state + 2) = data_02041dc8;
                    Ov107_BuildAndSendUpdate(*(int *)(state[0] + 0x388), 0x16b, 6, (void *)state[1]);
                    *(u8 *)(*state + 0x1c7) = 2;
                    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
                    return;
                }
            }
        }
    }
    if (state[9] > 0x2d000 || ((struct Bits17a *)(*state + 0x17a))->b1 || ((struct Bits17a *)(*state + 0x17a))->b0
        || ((struct Bits17a *)(*state + 0x17a))->b3) {
        Ov107_BuildAndSendUpdate(*(int *)(state[0] + 0x388), 0x16b, 7, (void *)state[1]);
        *(u8 *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
    }
}
