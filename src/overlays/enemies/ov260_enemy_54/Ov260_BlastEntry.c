/* Blast entry of the ov260 actor: pose 0xc plays, the actor is knocked back at the origin (mode 0xa),
 * effects 0x16 and 0x1d start at the +0x10 point, +0x70 and the +0x7b flag clear and the node moves
 * on to 020d0834. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov260_PlaySound(int owner, int mode, int arg);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov260_BlastTick(void);
extern const VecFx32 data_02041dc8;

void Ov260_BlastEntry(int *node)
{
    int *state = (int *)node[1];

    Ov107_PostTagUpdate((Actor *)(*state), 0xc, 0);
    func_ov107_020c0b90(*state, 0xa, data_02041dc8, 0);
    Ov260_PlaySound(*state, 0x16, state[4]);
    Ov260_PlaySound(*state, 0x1d, state[4]);
    state[0x1c] = 0;
    *((u8 *)state + 0x7b) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_BlastTick);
}
