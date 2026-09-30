/* Glide tick of an ov255 state: the +0x5c path point is resolved (Ov255_SteerToTarget) into the
 * +0x10 step. Once the +0xc idle byte clears, animation 0x1e and the +0x3a4 part's motion 0x19 play
 * looped, bit 6 of the owner's +0x60 high byte clears and the tick hands over to
 * Ov255_GlideToLandTick3. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct hw60 { unsigned short lo : 8, hi : 8; };

extern void Ov255_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov255_GlideToLandTick3(int *node);

void Ov255_GlideTick2(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    Ov255_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x1e, 1);
    Ov107_StartAnim(*(int *)(*state + 0x3a4), 0x19, 1);
    ((struct hw60 *)(*state + 0x60))->hi &= ~0x40;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov255_GlideToLandTick3);
}
