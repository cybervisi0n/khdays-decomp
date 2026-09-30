/* Recoil entry of the ov260 actor: pose 0x1b plays, its +0x428 part takes motion 0x10, the +0x2c
 * push is normalised and scaled to 1/16, +0x30 = -0x3d2b and the node moves on to 020cf90c. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov260_TickRecoilFall(void);

void Ov260_RecoilEntry(int *node)
{
    int *state = (int *)node[1];

    Ov107_PostTagUpdate((Actor *)(*state), 0x1b, 0);
    Ov107_StartAnim(*(int *)(*state + 0x428), 0x10, 0);
    VEC_Normalize((VecFx32 *)(state + 0xb), (VecFx32 *)(state + 0xb));
    ScaleVec3Fx12(0x100, (VecFx32 *)(state + 0xb), (VecFx32 *)(state + 0xb));
    state[0xc] = -0x3d2b;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_TickRecoilFall);
}
