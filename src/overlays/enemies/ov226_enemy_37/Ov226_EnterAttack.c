/* Attack entry of the ov221 enemy: animation 6 with a +0x78 target (7 without) plays, the
 * zero-vector message of mode 1 goes out with flag 1 only when there is no target, reaction
 * 0x14c mode 6 fires at the +8 point, the +0x75 flag and +0x5c timer clear and the tick hands
 * over to Ov226_SpawnWindupTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern VecFx32 data_02041dc8;
extern void Ov226_SpawnWindupTick(int *node);

void Ov226_EnterAttack(int *node)
{
    int *state = (int *)node[1];

    Ov107_PostTagUpdate((Actor *)(*state), state[0x1e] != 0 ? 6 : 7, 0);
    func_ov107_020c0b90(*state, 1, data_02041dc8, (unsigned char)(state[0x1e] == 0));
    Ov107_BuildAndSendUpdate(*state, 0x14c, 6, (void *)state[2]);
    *(unsigned char *)((char *)state + 0x75) = 0;
    state[0x17] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov226_SpawnWindupTick);
}
