/* Ov245_LaunchTick -- launch tick: takes the +0x3a0 item's forward vector (020c9f48, speed
 * returned), rotates it by the actor's +0xa0 placement into the state's +0x1c direction and
 * scales it by the speed; once the +4 item's animation is no longer busy (+0xad) the direction
 * is copied to +0x28 and the node moves to 020d6e00. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov245_CopyVecThenSetupSubActionAndAdvance(void);

void Ov245_LaunchTick(int *node) {
    int *state = (int *)node[1];
    VecFx32 fwd;
    int speed;

    speed = Ov107_ActionResource_GetOffsetAndScale(*(int *)(*state + 0x3a0), &fwd);
    Vec3TransformViaTempMtx((VecFx32 *)(state + 7), (void *)(*state + 0xa0), &fwd);
    ScaleVec3Fx12(speed, (VecFx32 *)(state + 7), (VecFx32 *)(state + 7));
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    *(VecFx32 *)(state + 10) = *(VecFx32 *)(state + 7);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_CopyVecThenSetupSubActionAndAdvance);
}
