/* Beam tick: the +0xc timer accumulates the frame rate; between 0.1 and 0.33 a 5.0-long segment
 * from the +8 point along data_02042264, as thick as the actor's +0x80 radius, is swept over the
 * actor list on behalf of the +0x38c owner: every entity whose +2 id bit is clear in the +0x10 mask
 * is pushed 0.5 horizontally away from the actor (kind 4); on acceptance the owner spawns effect 7 at
 * the +8 point and the bit is set. After any hit reaction 0 mode 0x53 fires there. Once the +4
 * part's rig is idle (+0xad), pose 0 is requested. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 p0; VecFx32 dir; int nLength; int nRadius; } Segment;

extern const VecFx32 data_02042264;
extern int Ov107_CollectSegmentOverlaps(int owner, Segment *seg, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov260_BeamTick(int *node)
{
    int *state = (int *)node[1];
    int hit = 0;
    Segment seg;
    int hits[4];
    VecFx32 push;
    int i;
    int n;

    state[3] += *(int *)(node[0] + 0x2c);
    if (state[3] >= 0x198 && state[3] <= 0x550) {
        seg.p0 = *(VecFx32 *)state[2];
        seg.dir = data_02042264;
        seg.nLength = 0x5000;
        seg.nRadius = *(int *)(*state + 0x80);
        n = Ov107_CollectSegmentOverlaps(*(int *)(*state + 0x38c), &seg, hits);
        for (i = 0; i < n; i++) {
            u8 bit = 1 << *(u16 *)(hits[i] + 2);

            if ((*((u8 *)state + 0x10) & bit) != 0) {
                continue;
            }
            VEC_Subtract((void *)(hits[i] + 0x74), (void *)(*state + 0x74), &push);
            push.y = 0;
            VEC_Normalize(&push, &push);
            ScaleVec3Fx12(0x800, &push, &push);
            if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x38c), 4, &push, 0) == 0) {
                continue;
            }
            func_ov107_020c0b90(*(int *)(*state + 0x38c), 7, *(VecFx32 *)state[2], 0);
            hit = 1;
            *((u8 *)state + 0x10) |= bit;
        }
        if (hit != 0) {
            Ov107_BuildAndSendUpdate(*state, 0, 0x53, (void *)state[2]);
        }
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    *(u8 *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
