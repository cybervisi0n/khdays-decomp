/* Glide tick of an ov235 state: the +0x40 rate is the owner's frame rate x 15, and the +0x5c path
 * point is resolved (Ov235_SteerToTarget) into the +0x10 step. Once the +0xc idle byte clears,
 * animation 5 plays, the +0x3a8 part plays motion 4, the +0x54 timer and +0x65 flag clear and the
 * tick hands over to Ov235_DiveTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov235_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov235_DiveTick(int *node);

void Ov235_GlideTick5(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 2;
    Ov235_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 5, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3a8), 4, 0);
    state[0x15] = 0;
    *((unsigned char *)state + 0x65) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov235_DiveTick);
}
