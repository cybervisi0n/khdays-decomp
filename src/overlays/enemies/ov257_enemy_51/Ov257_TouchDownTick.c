/* Touch-down tick of an ov257 state: the +0x40 rate clears and the +0x60 path point is resolved
 * (Ov257_SteerToTarget) into the +0x10 step. Once the +0xc idle byte clears and the owner is
 * grounded (+0x17a bit 0), animation 6 plays, the +0x3d0 part plays motion 5, reaction +0x408 mode
 * 3 fires at the +4 point and the tick hands over to Ov257_SettleTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Bits17a { unsigned char b0 : 1; };

extern void Ov257_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov257_SettleTick(int *node);

void Ov257_TouchDownTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    state[0x10] = 0;
    Ov257_SteerToTarget(state, state[0x18], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    if (((struct Bits17a *)(*state + 0x17a))->b0 == 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 6, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3d0), 5, 0);
    Ov107_BuildAndSendUpdate(state[0], (short)*(int *)(*state + 0x408), 3, (void *)state[1]);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov257_SettleTick);
}
