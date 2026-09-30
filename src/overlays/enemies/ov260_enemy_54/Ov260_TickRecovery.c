/* Recovery tick of the ov260 actor: with a target (+8) the +0x7c flag is raised and brain slot +0x20
 * runs 020d0360 at once. Otherwise, once the +4 rig is idle, pose 9 plays, effect 8 spawns in place,
 * move 0x15 starts (020cd148 with the +0x10 argument) and 020d0360 follows. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov260_PlaySound(int owner, int mode, int arg);
extern void Ov260_DashEntryTick(void);
extern const VecFx32 data_02041dc8;

void Ov260_TickRecovery(int *node)
{
    int *state = (int *)node[1];

    if (state[2] != 0) {
        state[0x1f] = 1;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_DashEntryTick);
        return;
    }
    if (*(unsigned char *)(state[1] + 0xad) == 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 9, 0);
        func_ov107_020c0b90(*state, 8, data_02041dc8, 0);
        Ov260_PlaySound(*state, 0x15, state[4]);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_DashEntryTick);
        return;
    }
}
