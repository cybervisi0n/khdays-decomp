/* Hover tick of an ov252 part: the guard sweep runs (020ce370) and +0x64 accumulates the frame rate;
 * a pending +0x88 start plays pose 0x26, at 1.66 a pending +0x89 cue plays sound 0x148/0xd at the +8
 * point, and at 5.98 pose 0x30 plays, the owner plays effect 0xd at the origin and the node moves on to
 * 020d19b8. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov252_GuardSweep(int *node);
extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, void *at);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_AiStep_QueueAction13OnAnimEnd(void);
extern const VecFx32 data_02041dc8;

void Ov252_PartHoverTick(int *node)
{
    int *state = (int *)node[1];

    Ov252_GuardSweep(node);
    state[0x19] += *(int *)(node[0] + 0x2c);
    if (*((unsigned char *)state + 0x88) == 1) {
        *((unsigned char *)state + 0x88) -= 1;
        Ov107_PostTagUpdate((Actor *)(*state), 0x26, 0);
    }
    if (state[0x19] >= 0x1a90 && *((unsigned char *)state + 0x89) == 1) {
        *((unsigned char *)state + 0x89) = 0;
        Ov107_BuildAndSendUpdate(*state, 0x148, 0xd, (void *)state[2]);
    }
    if (state[0x19] < 0x5fa0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x30, 0);
    func_ov107_020c0b90(*state, 0xd, data_02041dc8, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_AiStep_QueueAction13OnAnimEnd);
}
