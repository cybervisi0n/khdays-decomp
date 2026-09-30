/* Contact check of the ov230 actor: with a target (+8) and the hit window open (+0x68), the direction
 * from the owner's +0x494 point to the target is kept (normalised) and, flattened (world +z when
 * degenerate) and scaled to 0.5, offered as a kind-4 hit. On acceptance effect 1 spawns at the +0x494
 * point pushed out by the +0x4a0 radius along the direction plus that offset, and reaction 0x147 mode
 * 6 fires at the +0xc position. Returns whether the hit landed. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov107_BuildAndSendUpdate(int owner, int cue, int kind, void *target);
extern const VecFx32 data_02042258;

int Ov230_ContactCheck(int *state)
{
    VecFx32 dir;
    VecFx32 at;

    if (state[2] != 0 && state[0x1a] != 0) {
        VEC_Subtract((VecFx32 *)(state[2] + 0x74), (VecFx32 *)(*state + 0x494), &dir);
        VEC_Normalize(&dir, &at);
        dir.y = 0;
        if (VEC_Normalize(&dir, &dir) == 0) {
            dir = data_02042258;
        }
        ScaleVec3Fx12(0x800, &dir, &dir);
        if (Ov107_InvokeHitCallback(state[2], *state, *state, 4, &dir, 0) != 0) {
            ScaleVec3Fx12(*(int *)(*state + 0x4a0), &at, &at);
            VEC_Add(&at, &dir, &at);
            VEC_Add(&at, (VecFx32 *)(*state + 0x494), &at);
            func_ov107_020c0b90(*state, 1, at, 0);
            Ov107_BuildAndSendUpdate(*state, 0x147, 6, (void *)state[3]);
            return 1;
        }
    }
    return 0;
}
