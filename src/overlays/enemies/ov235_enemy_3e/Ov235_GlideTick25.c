/* Glide tick of an ov235 state: the +0x5c path point is resolved (Ov235_SteerToTarget) into the
 * +0x10 step. Once the +0xc idle byte clears, animation 0x25 and the +0x3a8 part's motion 0x1b play
 * looped, bit 6 of the owner's +0x60 high byte clears and the tick hands over to
 * Ov235_GlideToLandTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct hw60 { unsigned short lo : 8, hi : 8; };

extern void Ov235_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov235_GlideToLandTick(int *node);

void Ov235_GlideTick25(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    Ov235_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x25, 1);
    Ov107_StartAnim(*(int *)(*state + 0x3a8), 0x1b, 1);
    ((struct hw60 *)(*state + 0x60))->hi &= ~0x40;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov235_GlideToLandTick);
}
