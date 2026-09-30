/* Glide tick of the ov255 enemy: the +0x40 rate is the owner's frame rate x 15; the +0x5c path
 * point is resolved (Ov255_SteerToTarget) into a direction and a speed that give the +0x10 step.
 * Once the +0xc idle byte clears, animation 0xb plays, the +0x3a4 part plays motion 0xa and the tick
 * hands over to Ov255_GlideToLandTick2. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov255_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov255_GlideToLandTick2(int *node);

void Ov255_GlideTick_2(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 2;
    Ov255_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0xb, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3a4), 0xa, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov255_GlideToLandTick2);
}
