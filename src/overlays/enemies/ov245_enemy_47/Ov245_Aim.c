/* Ov245_Aim -- aim: copies the given direction into the state's +0x18 vector, scales
 * it by 0.875 into the +0xc velocity and, unless the actor's kind byte is 1, raises bit 0 of the
 * +0x60 high byte and requests sub-state 1. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);

void Ov245_Aim(int *state, const VecFx32 *dir) {
    *(VecFx32 *)(state + 6) = *dir;
    ScaleVec3Fx12(0xe00, (VecFx32 *)(state + 6), (VecFx32 *)(state + 3));
    if (*(signed char *)(*state + 0x1c6) != 1) {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 1) << 0x18) >> 0x10);
        *(unsigned char *)(*state + 0x1c7) = 1;
    }
}
