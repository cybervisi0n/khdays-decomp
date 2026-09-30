/* Burrow-burst tick: the +0x68 timer accumulates the frame rate; at 1/15 the +0xac flag bit 0 is
 * set and Ov259_MapHeldItemKindToAnim plays animation 3. Until 0.5 a 2.0 sphere half a unit below the
 * actor's +0x74 point sweeps the actor list: every entity whose +2 id bit is clear in the +0xaf
 * mask is pushed 1.25 away horizontally (kind 3); on acceptance the actor spawns effect 0xb at its
 * +0x74 point and its bit is set. The mask then keeps only the entities still inside the sphere.
 * Once the +4 part's rig is idle (+0xad), pose 2 is requested. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 center; int nRadius; } Sphere;

extern void Ov259_MapHeldItemKindToAnim(int owner, int anim);
extern int Ov107_CollectSphereOverlaps(int owner, Sphere *sphere, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov259_BurrowBurstTick(int *node)
{
    int *state = (int *)node[1];
    Sphere sphere;
    VecFx32 push;
    int hits[4];
    int n;
    u16 i;
    u8 seen;

    state[0x1a] += *(int *)(node[0] + 0x2c);
    if ((*((u8 *)state + 0xac) & 1) == 0 && state[0x1a] >= 0x110) {
        *((u8 *)state + 0xac) |= 1;
        Ov259_MapHeldItemKindToAnim(*state, 3);
    }
    if (state[0x1a] < 0x7f8) {
        sphere.center = *(VecFx32 *)(*state + 0x74);
        sphere.center.y -= 0x800;
        sphere.nRadius = 0x2000;
        seen = 0;
        n = Ov107_CollectSphereOverlaps(*state, &sphere, hits);
        for (i = 0; i < n; i++) {
            u8 bit = 1 << *(u16 *)(hits[i] + 2);

            seen |= bit;
            if ((*((u8 *)state + 0xaf) & bit) != 0) {
                continue;
            }
            VEC_Subtract((void *)(hits[i] + 0x74), &sphere.center, &push);
            push.y = 0;
            VEC_Normalize(&push, &push);
            ScaleVec3Fx12(0x1400, &push, &push);
            if (Ov107_InvokeHitCallback(hits[i], *state, *state, 3, &push, 0) == 0) {
                continue;
            }
            func_ov107_020c0b90(*state, 0xb, *(VecFx32 *)(hits[i] + 0x74), 0);
            *((u8 *)state + 0xaf) |= bit;
        }
        *((u8 *)state + 0xaf) &= seen;
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    *(u8 *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
