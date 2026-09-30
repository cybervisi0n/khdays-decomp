/* Recover entry of the ov239 enemy: plays animation 0, raises bit 0 of the actor's +0x1ae,
 * spawns effect 0 at the +8 point, fires reaction 0x138 mode 9 at the actor's position and
 * hands off to ccf0c. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int d);
extern void Ov107_BuildAndSendUpdate(int actor, int a, int b, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov239_stClearReadyFlag(int *node);

void Ov239_RecoverEntry(int *node)
{
    int *state = (int *)node[1];

    Ov107_PostTagUpdate((Actor *)(*state), 0, 0);
    *(unsigned short *)(*state + 0x1ae) |= 1;
    func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[2], 0);
    Ov107_BuildAndSendUpdate(*state, 0x138, 9, (void *)(*state + 0x74));
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov239_stClearReadyFlag);
}
