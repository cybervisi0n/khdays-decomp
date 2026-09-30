/* Aim marker of the ov258 actor: the flat direction from the +0xc point to a spot past the `side`
 * hand (+0x44c / +0x450) along the alternating +0x3f8 offset (+0x3c parity, 5.0 plus the +0x40 gap)
 * is normalised; within 5.0 a d100 roll under 50 marks the target's +0x190 point instead. Effect 0x1b
 * plays at that spot 15.6 high. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);

void Ov258_AimMarker(int *node, int side)
{
    int *state = (int *)node[1];
    VecFx32 from;
    VecFx32 dir;
    VecFx32 at;

    from = *(VecFx32 *)state[3];
    ScaleVec3Fx12(state[0x10] + 0x5000, &((VecFx32 *)(*state + 0x3f8))[state[0xf] % 2], &at);
    VEC_Add((VecFx32 *)((side == 0 ? *(int *)(*state + 0x44c) : *(int *)(*state + 0x450)) + 0x14), &at, &at);
    from.y = 0;
    at.y = 0;
    VEC_Subtract(&at, &from, &dir);
    VEC_Normalize(&dir, &dir);
    if (state[0x10] < 0x5000 && (unsigned int)RandNextScaled(100) < 0x32) {
        at = *(VecFx32 *)(*(int *)(*state + 0x454) + 0x190);
    }
    at.y = 0xfa00;
    func_ov107_020c0b90(*state, 0x1b, at, 0);
}
