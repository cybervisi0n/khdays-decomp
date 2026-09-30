/* Glide-to-land tick of an ov235 state: the +0x40 rate is the frame rate x 6 and the +0x5c path
 * point is resolved (Ov235_SteerToTarget) into the +0x10 step. Once the +0xc idle byte clears and
 * the owner is grounded (+0x17a bit 0), animation 9 plays, the +0x3a8 part plays motion 8,
 * reaction +0x3c8 (as a halfword) mode 3 fires at the +8 point and the tick hands over to
 * Ov235_UpdateMotionAndFinish. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Bits17a { unsigned char b0 : 1; };

extern void Ov235_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov235_UpdateMotionAndFinish(int *node);

void Ov235_GlideToLandTickB(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 5;
    Ov235_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    if (((struct Bits17a *)(*state + 0x17a))->b0 == 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 9, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3a8), 8, 0);
    Ov107_BuildAndSendUpdate(state[0], (short)*(int *)(*state + 0x3c8), 3, (void *)state[2]);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov235_UpdateMotionAndFinish);
}
