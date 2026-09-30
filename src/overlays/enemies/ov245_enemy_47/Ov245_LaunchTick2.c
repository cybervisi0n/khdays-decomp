/* Ov245_LaunchTick2 -- launch tick (variant): takes the +0x3a0 item's forward vector (020c9f48, speed
 * returned), rotates it by the actor's +0xa0 placement into the state's +0x1c direction and
 * scales it by the speed; once the +4 item's animation is no longer busy (+0xad) pose 9 plays
 * (flag 1), +0x40 clears, bit 6 of the +0x60 high byte clears and the node moves to 020d6f30. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
struct hw60 { unsigned short lo : 8, hi : 8; };
extern void Ov245_Rider_AiPauseTick(void);

void Ov245_LaunchTick2(int *node) {
    int *state = (int *)node[1];
    VecFx32 fwd;
    int speed;

    speed = Ov107_ActionResource_GetOffsetAndScale(*(int *)(*state + 0x3a0), &fwd);
    Vec3TransformViaTempMtx((VecFx32 *)(state + 7), (void *)(*state + 0xa0), &fwd);
    ScaleVec3Fx12(speed, (VecFx32 *)(state + 7), (VecFx32 *)(state + 7));
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 9, 1);
    state[0x10] = 0;
    ((struct hw60 *)(*state + 0x60))->hi &= ~0x40;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_Rider_AiPauseTick);
}
