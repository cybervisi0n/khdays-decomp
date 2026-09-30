/* Fall tick of the ov206 enemy: the +0x14 point is zeroed (data_02041dc8); until flagged
 * (+0x52) the +0x24 timer accumulates the owner's rate and past 0x666 fires reaction 0x116
 * mode 0xd at the +8 point and sets the flag. Once the +0xc idle byte is clear the owner plays
 * animation 4 (mode 1), +0x20 and the flag clear and the tick hands over to
 * Ov206_ChargeTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov206_ChargeTick(int *node);
extern const VecFx32 data_02041dc8;

void Ov206_FallTick(int *node)
{
    int *state = (int *)node[1];

    *(VecFx32 *)(state + 5) = data_02041dc8;
    if (*(unsigned char *)((char *)state + 0x52) == 0) {
        state[9] += *(int *)(node[0] + 0x2c);
        if (state[9] >= 0x666) {
            Ov107_BuildAndSendUpdate(*state, 0x116, 0xd, (void *)state[2]);
            *(unsigned char *)((char *)state + 0x52) = 1;
        }
    }
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 4, 1);
    state[8] = 0;
    *(unsigned char *)((char *)state + 0x52) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov206_ChargeTick);
}
