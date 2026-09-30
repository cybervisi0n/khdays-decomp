/* Begin the ov166 enemy's dash (x3: ov166/167/168): fires reaction 0x13e/5 at the +8 position,
 * clears the +0x28 timer, sets bit 0 and clears bits 2/3/7 of the actor's +0x60 high byte,
 * raises bit 0 on the +0x388 list node's +8 flags, copies the actor's +0x390 direction into
 * +0x18, sets the +0x24 speed to 0.25 and scales the direction by it into the +0xc step, then
 * advances to the dash handler (020ce744). */

#include "nitro/fx_types.h"

struct hw60 { unsigned short lo : 8, hi : 8; };
struct bf { unsigned b : 8; };

extern void Ov107_BuildAndSendUpdate(int actor, int id, int mode, void *anchor);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int node, int slot, void *cb);
extern void Ov168_DashTick(void);

void Ov168_BeginDash(int node)
{
    int *state = *(int **)(node + 4);
    Ov107_BuildAndSendUpdate(*state, 0x13e, 5, (void *)state[2]);
    state[10] = 0;
    {
        unsigned short *p = (unsigned short *)(*state + 0x60);
        unsigned int u = *p;
        *p = (unsigned short)((u & ~0xff00) | ((((u << 0x10) >> 0x18 | 1) << 0x18) >> 0x10));
    }
    ((struct hw60 *)(*state + 0x60))->hi &= ~0x8c;
    ((struct bf *)(*(int *)(*state + 0x388) + 8))->b |= 1;
    *(VecFx32 *)(state + 6) = *(VecFx32 *)(*state + 0x390);
    state[9] = 0x400;
    ScaleVec3Fx12(state[9], (VecFx32 *)(state + 6), (VecFx32 *)(state + 3));
    SetIndexedSlot(node, *(signed char *)(node + 0x20), Ov168_DashTick);
}
