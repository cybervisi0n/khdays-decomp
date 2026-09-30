/* Ov245_SpinAttackTick -- spin-attack tick: the +0x40 timer runs up by the frame step and, past
 * 0.333 (latched at +0x48), plays effect 2 at the +0xc anchor; from then on a 0.1875 sphere at
 * the +0x390 item's +0x14 anchor is swept for hits (020c8eb8) and each hit whose +2 slot bit is
 * not yet in the +0x49 mask is pushed 0.5 along the flattened direction from the anchor (Z when
 * degenerate) through the +0x3cc item (020ca918, mode 5): a landing hit gets effect 1 and
 * reaction 0/0x4e at the anchor and its bit set. Then the +8 target is refreshed (020cab14) and,
 * with none or once the scene's +0xad flag is clear, sub-state 2 and the node slot are released. */

#include "nitro/fx_types.h"

struct Sphere { VecFx32 centre; int radius; };

extern void func_ov107_020c0b90(int actor, int effect, VecFx32 v, int flag);
extern int Ov107_CollectSphereOverlaps(int actor, struct Sphere *sphere, int *out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern int Ov107_FindNearestObject(int actor, int a);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042258;

void Ov245_SpinAttackTick(int *node) {
    int *state = (int *)node[1];
    int hits[4];
    struct Sphere sphere;
    VecFx32 push;
    int nHits;
    int i;

    state[0x10] += *(int *)(node[0] + 0x2c);
    if (*((unsigned char *)state + 0x48) == 0 && state[0x10] >= 0x555) {
        func_ov107_020c0b90(*state, 2, *(VecFx32 *)state[3], 0);
        *((unsigned char *)state + 0x48) = 1;
    }
    if (state[0x10] >= 0x555) {
        sphere.centre = *(VecFx32 *)(*(int *)(*state + 0x390) + 0x14);
        sphere.radius = 0x300;
        nHits = Ov107_CollectSphereOverlaps(*state, &sphere, hits);
        {
            for (i = 0; i < nHits; i++) {
                if ((*((unsigned char *)state + 0x49) & (1 << *(unsigned short *)(hits[i] + 2))) == 0) {
                    VEC_Subtract((VecFx32 *)(hits[i] + 0x74), (VecFx32 *)state[3], &push);
                    push.y = 0;
                    if (VEC_Normalize(&push, &push) == 0) {
                        push = data_02042258;
                    }
                    ScaleVec3Fx12(0x800, &push, &push);
                    if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x3cc), 5, &push, 0) != 0) {
                        func_ov107_020c0b90(*state, 1, *(VecFx32 *)(*(int *)(*state + 0x390) + 0x14), 0);
                        *((unsigned char *)state + 0x49) |= 1 << *(unsigned short *)(hits[i] + 2);
                        Ov107_BuildAndSendUpdate(*state, 0, 0x4e, (void *)(*(int *)(*state + 0x390) + 0x14));
                    }
                }
            }
        }
    }
    state[2] = Ov107_FindNearestObject(*state, 0);
    if (state[2] == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    *(unsigned char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
