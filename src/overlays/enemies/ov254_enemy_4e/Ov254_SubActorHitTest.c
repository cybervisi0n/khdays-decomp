/* Sub-actor hit test (called by the ticks at 020d4a90 / 020d4c94): with a segment the owner's
 * (+0x38c) segment sweep is used, otherwise the sub-actor's sphere; every entity whose +2 id bit is
 * clear in the +0x2c mask is pushed 1.0 horizontally away from the sub-actor, lifted by 1.0 (kind 2,
 * on behalf of the owner); on acceptance the owner spawns effect 0 at the entity's +0x74 point (or
 * at the sphere centre), reaction 0x16d mode 8 fires at the +8 point and the bit is set. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern int Ov107_CollectSegmentOverlaps(int owner, void *seg, int *hits);
extern int Ov107_CollectSphereOverlaps(int owner, void *sphere, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);

void Ov254_SubActorHitTest(int *node, VecFx32 *sphere, void *seg)
{
    int *state = (int *)node[1];
    int hits[4];
    VecFx32 push;
    int n;
    int i;

    if (seg != 0) {
        n = Ov107_CollectSegmentOverlaps(*(int *)(*state + 0x38c), seg, hits);
    } else {
        if (sphere == 0) {
            return;
        }
        n = Ov107_CollectSphereOverlaps(*state, sphere, hits);
    }
    for (i = 0; i < n; i++) {
        u8 bit = 1 << *(u16 *)(hits[i] + 2);

        if ((*((u8 *)state + 0x2c) & bit) != 0) {
            continue;
        }
        VEC_Subtract((void *)(hits[i] + 0x74), (void *)(*state + 0x74), &push);
        push.y = 0;
        VEC_Normalize(&push, &push);
        push.y = 0x1000;
        if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x38c), 2, &push, 0) == 0) {
            continue;
        }
        if (seg != 0) {
            func_ov107_020c0b90(*(int *)(*state + 0x38c), 0, *(VecFx32 *)(hits[i] + 0x74), 0);
        } else if (sphere != 0) {
            func_ov107_020c0b90(*(int *)(*state + 0x38c), 0, *sphere, 0);
        }
        Ov107_BuildAndSendUpdate(*state, 0x16d, 8, (void *)state[2]);
        *((u8 *)state + 0x2c) |= bit;
    }
}
