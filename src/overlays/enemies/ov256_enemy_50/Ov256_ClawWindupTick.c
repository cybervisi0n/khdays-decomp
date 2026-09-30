/* Claw windup tick of the ov256 actor: it re-picks its target (020ccd54); each time the partner holds
 * no queued move the windup count +0x54 grows with pose 0xb, and at 2 +0x4c clears, a charge is armed
 * (+0x69 = 1), the actor is knocked back at the origin twice (modes 0xa and 0xb), both claws (+0x434,
 * +0x438) aim at the +0x34 direction (020d108c), pose 0xc plays and the node moves on to 020cf88c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov256_PickTarget(int *node);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov256_Claw_LaunchIfReady(int claw, VecFx32 *dir);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_RoarTick(void);
extern const VecFx32 data_02041dc8;

void Ov256_ClawWindupTick(int *node)
{
    int *state = (int *)node[1];

    Ov256_PickTarget(node);
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if (state[0x15] >= 2) {
        VecFx32 origin;

        state[0x13] = 0;
        *((u8 *)state + 0x69) = 1;
        origin = data_02041dc8;
        func_ov107_020c0b90(*state, 0xa, origin, 0);
        func_ov107_020c0b90(*state, 0xb, origin, 0);
        Ov256_Claw_LaunchIfReady(*(int *)(*state + 0x434), (VecFx32 *)(state + 0xd));
        Ov256_Claw_LaunchIfReady(*(int *)(*state + 0x438), (VecFx32 *)(state + 0xd));
        Ov107_PostTagUpdate((Actor *)(*state), 0xc, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_RoarTick);
        return;
    }
    state[0x15]++;
    Ov107_PostTagUpdate((Actor *)(*state), 0xb, 0);
}
