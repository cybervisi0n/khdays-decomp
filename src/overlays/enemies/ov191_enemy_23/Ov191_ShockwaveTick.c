/* Shockwave tick of the ov191 enemy (x3: ov191/192/193): walks the owner's +0xa8 actor list and,
 * for every actor of the same team (+4) whose +0x74 position lies inside the state's box
 * (+0x28..+0x30 min, +0x34..+0x3c max), applies hit 0 with a 0x800 push along the normalised
 * direction from the state's +4 origin (flags 0x18). The +0x24 timer advances by the node's
 * +0x2c speed and at 0xa00 the sub-state drops to 0 with the slot cleared. */

#include "nitro/fx_types.h"

extern int *List_First(void *list);
extern int *List_Next(void *list);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int victim, int a, int b, int mode, VecFx32 *push, int flags);
extern void SetIndexedSlot(int node, int slot, void *cb);

void Ov191_ShockwaveTick(int node)
{
    int *state = *(int **)(node + 4);
    int owner = *(int *)(*state + 4);
    int *pNode;
    int actor;
    VecFx32 push;

    pNode = List_First((void *)(owner + 0xa8));
    actor = pNode == 0 ? 0 : *pNode;
    while (actor != 0) {
        if (*(int *)(actor + 4) == *(int *)(*state + 4)
            && *(int *)(actor + 0x74) >= state[10] && *(int *)(actor + 0x78) >= state[0xb]
            && *(int *)(actor + 0x7c) >= state[0xc]
            && *(int *)(actor + 0x74) <= state[0xd] && *(int *)(actor + 0x78) <= state[0xe]
            && *(int *)(actor + 0x7c) <= state[0xf]) {
            VEC_Subtract((VecFx32 *)(actor + 0x74), (VecFx32 *)state[1], &push);
            push.y = 0;
            VEC_Normalize(&push, &push);
            ScaleVec3Fx12(0x800, &push, &push);
            Ov107_InvokeHitCallback(actor, *state, *(int *)(*state + 0x38c), 0, &push, 0x18);
        }
        pNode = List_Next((void *)(owner + 0xa8));
        actor = pNode == 0 ? 0 : *pNode;
    }
    state[9] += *(int *)(*(int *)node + 0x2c);
    if (state[9] >= 0xa00) {
        *(unsigned char *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)(node + 0x20), 0);
    }
}
