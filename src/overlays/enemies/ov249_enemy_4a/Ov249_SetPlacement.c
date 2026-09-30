/* Placement setter of the ov249 actor: its position is set (020c5c54) to the given point raised by the
 * +0x70 height, the +0x398 goal to the second point, and bit 0 of the +0x60 high byte is raised. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

void Ov249_SetPlacement(int self, VecFx32 pos, VecFx32 goal)
{
    VecFx32 at = pos;

    at.y += *(int *)(self + 0x70);
    Ov107_MoveNodeAndRelayout((Actor *)self, &at);
    *(VecFx32 *)(self + 0x398) = goal;
    {
        unsigned short hv = *(unsigned short *)(self + 0x60);

        *(unsigned short *)(self + 0x60) =
            (unsigned short)((hv & ~0xff00) | (((((unsigned int)hv << 0x10) >> 0x18 | 1) << 0x18) >> 0x10));
    }
}
