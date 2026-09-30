/* Copies the owner's position into the sub-object at +0x58, takes the vector to the target
 * at +0x14 of the block at +0x3b8, stores its squared length at +0x70 (never 0), and closes
 * the update. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void VEC_Subtract(VecFx32 *a, VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *a, VecFx32 *b);

void Ov245_Variant_PostTickAim(char *self) {
    char *o = *(char **)(self + 0x388);
    VecFx32 *v = (VecFx32 *)(o + 0x58);
    *v = *(VecFx32 *)(self + 0xb0);
    VEC_Subtract((VecFx32 *)(*(char **)(self + 0x3b8) + 0x14), v, (VecFx32 *)(o + 0x64));
    *(int *)(o + 0x70) = VEC_Normalize((VecFx32 *)(o + 0x64), (VecFx32 *)(o + 0x64));
    if (*(int *)(o + 0x70) == 0) {
        *(int *)(o + 0x70) = 1;
    }
    Ov107_AiState_PostTickBase(self);
}
