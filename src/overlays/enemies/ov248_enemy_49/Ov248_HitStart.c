/* Entry of the ov248 actor's hit move: bit 0 of the owner's +0x60 high byte is set and bit 7 cleared,
 * the owner plays effect 1 in place and cue 0x146 (13) on the +8 target, the +0x1c flag and the +0x18
 * clock reset, and brain slot +0x20 runs 020d08a0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov107_BuildAndSendUpdate(int owner, int cue, int kind, void *target);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov248_BeamTick(void);
extern const VecFx32 data_02041dc8;

void Ov248_HitStart(int *node)
{
    int *state = (int *)node[1];

    {
        u16 hw = *(u16 *)(*state + 0x60);

        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 1) << 0x18) >> 0x10);
    }
    {
        u16 hw = *(u16 *)(*state + 0x60);

        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~0x80) << 0x18) >> 0x10);
    }
    func_ov107_020c0b90(*state, 1, data_02041dc8, 1);
    Ov107_BuildAndSendUpdate(*state, 0x146, 0xd, (void *)state[2]);
    *((unsigned char *)state + 0x1c) = 0;
    state[6] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov248_BeamTick);
}
