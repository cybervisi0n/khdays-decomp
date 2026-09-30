/* Blast tick of the ov260 actor: the +0x70 timer accumulates the frame rate and the blast steps fire
 * once each (+0x7b bits): at 0x1540 the actor is knocked back at the origin (mode 0xb), at 0x1870
 * pose 0x1f plays, at 0x1430 and 0x1ed0 the cue fires (020cd04c 2 / 3). Once the partner holds no
 * queued move the next move is 2 and the node ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov260_MapHeldItemKindToAnim(int actor, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;

void Ov260_BlastTick(int *node)
{
    int *state = (int *)node[1];

    state[0x1c] += *(int *)(node[0] + 0x2c);
    if ((*((u8 *)state + 0x7b) & 1) == 0 && state[0x1c] >= 0x1540) {
        *((u8 *)state + 0x7b) |= 1;
        func_ov107_020c0b90(*state, 0xb, data_02041dc8, 0);
    }
    if ((*((u8 *)state + 0x7b) & 2) == 0 && state[0x1c] >= 0x1870) {
        *((u8 *)state + 0x7b) |= 2;
        Ov107_PostTagUpdate((Actor *)(*state), 0x1f, 0);
    }
    if ((*((u8 *)state + 0x7b) & 4) == 0 && state[0x1c] >= 0x1430) {
        *((u8 *)state + 0x7b) |= 4;
        Ov260_MapHeldItemKindToAnim(*state, 2);
    }
    if ((*((u8 *)state + 0x7b) & 8) == 0 && state[0x1c] >= 0x1ed0) {
        *((u8 *)state + 0x7b) |= 8;
        Ov260_MapHeldItemKindToAnim(*state, 3);
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    *(signed char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
