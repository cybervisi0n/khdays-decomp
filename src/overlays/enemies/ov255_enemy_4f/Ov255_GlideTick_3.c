/* Glide tick of an ov255 state: the +0x50 timer accumulates the owner's rate, the +0x40 rate
 * follows the frame rate and the +0x5c path point is resolved (Ov255_SteerToTarget) into the +0x10
 * step. Once the +0xc idle byte clears, animation 0x14 plays, the +0x3a4 part plays motion 0x10,
 * +0x63, the +0x44 timer, +0x62 and +0x65 clear and the tick hands over to Ov255_LungeTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov255_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov255_LungeTick(int *node);

void Ov255_GlideTick_3(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    state[0x14] += *(int *)(node[0] + 0x2c);
    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 30;
    Ov255_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x14, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3a4), 0x10, 0);
    *((unsigned char *)state + 0x63) = 0;
    state[0x11] = 0;
    *((unsigned char *)state + 0x62) = 0;
    *((unsigned char *)state + 0x65) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov255_LungeTick);
}
