/* Place the ov218 actor at `pos` for `owner`: +0x398 clears, it is registered there (020c5c54), the
 * spawn point is kept in +0x3ac and its guard flag (+0x60 high byte bit 0) is set. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void Ov107_MoveNodeAndRelayout(char *self, int owner, VecFx32 *pos);

void Ov218_PlaceAt(char *self, int owner, VecFx32 *pos)
{
    *(int *)(self + 0x398) = 0;
    Ov107_MoveNodeAndRelayout(self, owner, pos);
    *(VecFx32 *)(self + 0x3ac) = *pos;
    {
        u16 hw = *(u16 *)(self + 0x60);
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 1) << 0x18) >> 0x10);
    }
}
