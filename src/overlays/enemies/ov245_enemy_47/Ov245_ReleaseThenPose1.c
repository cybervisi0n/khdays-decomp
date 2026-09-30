/* Ov245_ReleaseThenPose1 -- clears bit 7 of the actor's +0x60 high byte; once the +4 item's
 * animation is idle (+0xad) plays pose 1, spawns effect 0 at the state's +8 position (020c0b90)
 * and moves the node to 020d51d8. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct hw60 { unsigned short lo : 8, hi : 8; };

extern void func_ov107_020c0b90(int actor, int effect, VecFx32 v, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov245_FireAttack1(void);

void Ov245_ReleaseThenPose1(int *node) {
    int *state = (int *)node[1];

    ((struct hw60 *)(*state + 0x60))->hi &= ~0x80;
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 1, 0);
    func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[2], 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_FireAttack1);
}
