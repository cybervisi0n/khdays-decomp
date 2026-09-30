/* Moves the node to `pos` (Ov107_MoveNodeAndRelayout), stores `src` at +0x390 and sets flag bit 0. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Obj {
    char _pad0[0x60];
    unsigned short flags : 8;   /* 0x60, bits [7:0] */
    unsigned short _bf : 8;     /* 0x60, bits [15:8] */
    char _pad1[0x390 - 0x62];
    VecFx32 vec;            /* 0x390 */
};

void Ov167_RelayoutAndStoreVec(struct Obj *this, VecFx32 *pos, VecFx32 *src) {
    Ov107_MoveNodeAndRelayout((Actor *)this, pos);
    this->vec = *src;
    this->_bf |= (unsigned short)1;
}
