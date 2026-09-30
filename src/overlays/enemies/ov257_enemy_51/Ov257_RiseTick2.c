/* Glide tick of an ov257 state: the +0x50 timer accumulates the owner's rate and the +0x5c path
 * point is resolved (Ov257_SteerToTarget) into the +0x10 step. Once bit 1 of the +0x3d0 part's +4
 * byte is set, animation 0x19 plays, the part plays motion 0x16 and the tick hands over to
 * Ov257_HoverInTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Bits4 { unsigned char b0 : 1, b1 : 1; };

extern void Ov257_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov257_HoverInTick(int *node);

void Ov257_RiseTick2(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    state[0x15] += *(int *)(node[0] + 0x2c);
    Ov257_SteerToTarget(state, state[0x18], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (((struct Bits4 *)(*(int *)(*state + 0x3d0) + 4))->b1 == 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x19, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3d0), 0x16, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov257_HoverInTick);
}
