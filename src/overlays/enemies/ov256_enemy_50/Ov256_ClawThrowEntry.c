/* Claw throw entry of the ov256 actor: it re-picks its target (020ccd54), pose 7 plays, it is knocked
 * back at the +0xc point (mode 4, 2), the next claw of the +0x43c set (+0x54 counter) is thrown from
 * 2.19 above that point (020d0334), the counter grows and the node moves on to 020cfe2c. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov256_PickTarget(int *node);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov256_InvokeHookAndRearm2(int claw, VecFx32 *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_AiLobPickNext(void);

void Ov256_ClawThrowEntry(int *node)
{
    int *state = (int *)node[1];
    VecFx32 at;

    at = *(VecFx32 *)state[3];
    at.y += 0x2300;
    Ov256_PickTarget(node);
    Ov107_PostTagUpdate((Actor *)(*state), 7, 0);
    func_ov107_020c0b90(*state, 4, *(VecFx32 *)state[3], 2);
    Ov256_InvokeHookAndRearm2(*(int *)(*state + state[0x15] * 4 + 0x43c), &at);
    state[0x15]++;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_AiLobPickNext);
}
