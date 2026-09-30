/* Aim entry of an ov256 claw: bits 2-4 of the owner's +0x60 high byte are set, the nearest live entity
 * (020cab14) becomes the +4 target and the +0x1c spin is the unit direction from the +0xc anchor to
 * its +0x190 point, its height clamped to [-0.5, 0.25] (the flat part grows by the excess over 0.75)
 * and renormalised, then scaled to 2.0. The +0x390 part takes motion 0, +0x80 rests on the vertical
 * axis, the +0x6c flag and +0x60 clear and the node moves on to 020d21d0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov107_FindNearestObject(int obj, int kind);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_ClawSpinTick(void);
extern const VecFx32 data_02042264;

void Ov256_ClawAimEntry(int *node)
{
    int *state = (int *)node[1];

    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x1c) << 0x18) >> 0x10);
    }
    state[1] = Ov107_FindNearestObject(*state, 0);
    VEC_Subtract((VecFx32 *)(state[1] + 0x190), (VecFx32 *)state[3], (VecFx32 *)(state + 7));
    VEC_Normalize((VecFx32 *)(state + 7), (VecFx32 *)(state + 7));
    if (state[8] > 0x400) {
        int over = state[8] - 0x400;

        state[7] += over * (state[7] / 0xc00);
        state[9] += over * (state[9] / 0xc00);
        state[8] = 0x400;
        VEC_Normalize((VecFx32 *)(state + 7), (VecFx32 *)(state + 7));
    }
    if (state[8] < -0x400) {
        int over = state[8] + 0x400;

        state[7] -= over * (state[7] / 0xc00);
        state[9] -= over * (state[9] / 0xc00);
        state[8] = -0x800;
        VEC_Normalize((VecFx32 *)(state + 7), (VecFx32 *)(state + 7));
    }
    ScaleVec3Fx12(0x2000, (VecFx32 *)(state + 7), (VecFx32 *)(state + 7));
    Ov107_StartAnim(*(int *)(*state + 0x390), 0, 0);
    *(VecFx32 *)(state + 0x20) = data_02042264;
    *((u8 *)state + 0x6c) = 0;
    state[0x18] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_ClawSpinTick);
}
