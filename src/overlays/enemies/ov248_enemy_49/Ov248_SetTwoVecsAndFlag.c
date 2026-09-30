/* Setter: hand the caller's vector (param v) to Ov107_MoveNodeAndRelayout, store the second
 * triple (param_5..7) into owner fields +0x398/+0x39c/+0x3a0, and set owner hw60 hi bit 1. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

void Ov248_SetTwoVecsAndFlag(int param_1, VecFx32 v, VecFx32 v2) {
    Ov107_MoveNodeAndRelayout((Actor *)param_1, &v);
    *(VecFx32 *)(param_1 + 0x398) = v2;
    {
        unsigned short hv = *(unsigned short *)(param_1 + 0x60);
        *(unsigned short *)(param_1 + 0x60) =
            (unsigned short)((hv & ~0xff00) | (((((unsigned int)hv << 0x10) >> 0x18 | 1) << 0x18) >> 0x10));
    }
}
