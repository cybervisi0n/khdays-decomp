/* Glide tick of an ov235 state: the +0x54 timer accumulates the owner's rate and the +0x5c path
 * point is resolved (Ov235_SteerToTarget) into the +0x10 step. Once bit 1 of the +0x3a8 part's +4
 * byte is set, animation 0x23 plays, the part plays motion 0x19 and the tick hands over to
 * Ov235_GlideInTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Bits4 { unsigned char b0 : 1, b1 : 1; };

extern void Ov235_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov235_GlideInTick(int *node);

void Ov235_GlideTick23(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    state[0x15] += *(int *)(node[0] + 0x2c);
    Ov235_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (((struct Bits4 *)(*(int *)(*state + 0x3a8) + 4))->b1 == 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x23, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3a8), 0x19, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov235_GlideInTick);
}
