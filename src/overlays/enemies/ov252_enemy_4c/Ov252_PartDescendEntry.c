/* Descend entry of an ov252 part: +0x6c clears, +0x89 = 5, +0x88 = 1, the owner plays effects 0xe and
 * 0xf at the origin and the node moves on to 020d2848. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_PartDescendTick(void);
extern const VecFx32 data_02041dc8;

void Ov252_PartDescendEntry(int *node)
{
    int *state = (int *)node[1];
    VecFx32 at;

    state[0x1b] = 0;
    *((u8 *)state + 0x89) = 5;
    *((u8 *)state + 0x88) = 1;
    at = data_02041dc8;
    func_ov107_020c0b90(*state, 0xe, at, 0);
    func_ov107_020c0b90(*state, 0xf, at, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_PartDescendTick);
}
