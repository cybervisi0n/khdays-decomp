/* Beam tick: once, while the +4 part's rig is busy, the actor spawns effect 1 (flag 2). The +0x18
 * timer accumulates the frame rate; past 1/3 a 5.0-long segment from the +8 point along
 * data_02042264, as thick as the actor's +0x80 radius, sweeps the actor list on behalf of the +0x384
 * owner. The first entity that accepts a 0.5 horizontal push away from the actor (kind 3) ends the
 * beam: effect 1 (flag 3), the owner's effect 8 and the actor's effect 0 (flag 1) at its +0x74
 * point, reaction 0 mode 0x53 at the +8 point, and the tick hands over to Ov248_AiStep_QueueAction0OnAnimEnd. Past
 * 3.0 without a hit the beam ends the same way without the hit effects. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 p0; VecFx32 dir; int nLength; int nRadius; } Segment;

extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042264;
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern int Ov107_CollectSegmentOverlaps(int owner, Segment *seg, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov248_AiStep_QueueAction0OnAnimEnd(int *node);

void Ov248_BeamTick(int *node)
{
    int *state = (int *)node[1];
    Segment seg;
    int hits[4];
    VecFx32 push;
    int i;
    int n;

    if (*((u8 *)state + 0x1c) == 0 && *(u8 *)(state[1] + 0xad) == 0) {
        func_ov107_020c0b90(*state, 1, data_02041dc8, 2);
        *((u8 *)state + 0x1c) = 1;
    }
    state[6] += *(int *)(node[0] + 0x2c);
    if (state[6] >= 0x550) {
        seg.p0 = *(VecFx32 *)state[2];
        seg.dir = data_02042264;
        seg.nLength = 0x5000;
        seg.nRadius = *(int *)(*state + 0x80);
        n = Ov107_CollectSegmentOverlaps(*(int *)(*state + 0x384), &seg, hits);
        for (i = 0; i < n; i++) {
            VEC_Subtract((void *)(hits[i] + 0x74), (void *)(*state + 0x74), &push);
            push.y = 0;
            VEC_Normalize(&push, &push);
            ScaleVec3Fx12(0x800, &push, &push);
            if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x384), 3, &push, 0) == 0) {
                continue;
            }
            func_ov107_020c0b90(*state, 1, data_02041dc8, 3);
            func_ov107_020c0b90(*(int *)(*state + 0x384), 8, *(VecFx32 *)(*state + 0x74), 0);
            func_ov107_020c0b90(*state, 0, *(VecFx32 *)(*state + 0x74), 1);
            Ov107_BuildAndSendUpdate(*state, 0, 0x53, (void *)state[2]);
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov248_AiStep_QueueAction0OnAnimEnd);
            return;
        }
    }
    if (state[6] < 0x3000) {
        return;
    }
    func_ov107_020c0b90(*state, 1, data_02041dc8, 3);
    func_ov107_020c0b90(*state, 0, *(VecFx32 *)(*state + 0x74), 1);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov248_AiStep_QueueAction0OnAnimEnd);
}
