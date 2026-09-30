/* Moves the node to `pos` (Ov107_MoveNodeAndRelayout), then sets bit 0 of the high byte of its
 * flags (+0x60). */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

void Ov178_RunSetupThenSetHw60HighBit0(int this_, VecFx32 *pos) {
    unsigned short *p = (unsigned short *)(this_ + 0x60);
    unsigned int h;
    Ov107_MoveNodeAndRelayout((Actor *)this_, pos);
    h = *p;
    *p = h & ~0xff00 | (((((unsigned int)h << 0x10) >> 0x18 | 1) << 0x18) >> 0x10);
}
