/* Glide-to-land tick of an ov255 state: the +0x5c path point is resolved (Ov255_SteerToTarget)
 * into the +0x10 step; once the owner is grounded (+0x17a bit 0) animation 0x17 plays, the
 * +0x3a4 part plays motion 0x12, reaction +0x3f8 (as a halfword) mode 3 fires at the +8 point and
 * the tick hands over to Ov255_AimAndLaunchHoming. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Bits17a { unsigned char b0 : 1; };

extern void Ov255_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov255_AimAndLaunchHoming(int *node);

void Ov255_GlideToLandTick3(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    Ov255_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (((struct Bits17a *)(*state + 0x17a))->b0 == 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x17, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3a4), 0x12, 0);
    Ov107_BuildAndSendUpdate(state[0], (short)*(int *)(*state + 0x3f8), 3, (void *)state[2]);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov255_AimAndLaunchHoming);
}
