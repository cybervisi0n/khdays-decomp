/* Wind-up hold tick of an ov257 state: the same three-phase trigger on the rig's channel-0 frame
 * (+0x78: modes 0 and 1 at 2.0 and 11.0, restarting below 11.0); the +0x40 rate is the frame rate x
 * 3 and the +0x10 step heads for the +0x60 target (Ov257_SteerToTarget). Once the +0xc idle byte
 * clears, animation 3 plays, the +0x3d0 part plays motion 2, +0x78 clears and the tick hands over
 * to Ov257_BiteChargeTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern int Ov257_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov257_BiteChargeTick(int *node);

void Ov257_WindUpHoldTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    {
        unsigned char phase = *((unsigned char *)state + 0x78);

        if (phase == 0) {
            if (queryTableEntry(*(int *)(*state + 0x384), 0) >= 0x2000) {
                Ov107_BuildAndSendUpdate(state[0], (short)*(int *)(*state + 0x408), 0, (void *)state[2]);
                *((unsigned char *)state + 0x78) = 1;
            }
        } else if (phase == 1) {
            if (queryTableEntry(*(int *)(*state + 0x384), 0) >= 0xb000) {
                Ov107_BuildAndSendUpdate(state[0], (short)*(int *)(*state + 0x408), 1, (void *)state[2]);
                *((unsigned char *)state + 0x78) = 2;
            }
        } else if (phase == 2) {
            if (queryTableEntry(*(int *)(*state + 0x384), 0) < 0xb000) {
                *((unsigned char *)state + 0x78) = 0;
            }
        }
    }
    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 10;
    Ov257_SteerToTarget(state, state[0x18], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 3, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3d0), 2, 0);
    *((unsigned char *)state + 0x78) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov257_BiteChargeTick);
}
