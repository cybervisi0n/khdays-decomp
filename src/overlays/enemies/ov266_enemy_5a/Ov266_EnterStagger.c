/* Stagger entry of the ov266 enemy: plays animation 9, spawns effect 2 at the zero vector
 * (data_02041dc8), fires reaction 0x15e mode 9 at the +8 point, clears the +0x40 timer and the
 * +0x5a byte and hands off to the stagger tick (020d1c0c). */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int d);
extern void Ov107_BuildAndSendUpdate(int actor, int a, int b, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern VecFx32 data_02041dc8;
extern void Ov266_StaggerTick(int *node);

void Ov266_EnterStagger(int *node)
{
    int *state = (int *)node[1];

    Ov107_PostTagUpdate((Actor *)(*state), 9, 0);
    func_ov107_020c0b90(*state, 2, data_02041dc8, 0);
    Ov107_BuildAndSendUpdate(*state, 0x15e, 9, (void *)state[2]);
    state[0x10] = 0;
    *(unsigned char *)((char *)state + 0x5a) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov266_StaggerTick);
}
