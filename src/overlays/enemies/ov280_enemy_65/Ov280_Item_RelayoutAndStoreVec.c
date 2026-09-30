/* Move the shot to `pos` (its translation; Ov107_MoveNodeAndRelayout), store its direction
 * (-> +0x390) and spin (-> +0x38c), and raise flag 0 in the high byte at +0x60.
 *
 * The callers (Ov280_AiFireVolleyTick) pass a fifth argument, a point, which the ROM puts on
 * the stack (`str r0, [sp]` before the call) and this function never reads. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

void Ov280_Item_RelayoutAndStoreVec(int muzzle, const VecFx32 *pos, const VecFx32 *dir,
                                    signed char spin, int unusedPoint) {
    Ov107_MoveNodeAndRelayout((Actor *)muzzle, pos);
    *(VecFx32 *)(muzzle + 0x390) = *dir;
    *(signed char *)(muzzle + 0x38c) = spin;
    {
        unsigned short *p = (unsigned short *)(muzzle + 0x60);
        unsigned int hi = ((unsigned int)*p << 0x10) >> 0x18;
        hi |= 1;
        *p = (unsigned short)((*p & ~0xff00) | ((hi << 0x18) >> 16));
    }
}
