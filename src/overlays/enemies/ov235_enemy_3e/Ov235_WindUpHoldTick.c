/* Wind-up hold tick of an ov235 state: the same three-phase trigger on the rig's channel-0 frame
 * (modes 1 and 0 at 1.0 and 11.0, restarting below 11.0); the +0x40 rate is the frame rate x 3.
 * Without a nearest target (020cab14, kept in +0x5c) sub-state 2 is requested; otherwise the +0x10
 * step heads for it (Ov235_SteerToTarget), and once the +0xc idle byte clears animation 3 plays,
 * the +0x3a8 part plays motion 2 and the tick hands over to Ov235_ApproachTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern int Ov107_FindNearestObject(int obj, int kind);
extern int Ov235_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov235_ApproachTick(int *node);

void Ov235_WindUpHoldTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    {
        unsigned char phase = *((unsigned char *)state + 0x65);

        if (phase == 0) {
            if (queryTableEntry(*(int *)(*state + 0x384), 0) >= 0x1000) {
                Ov107_BuildAndSendUpdate(state[0], (short)*(int *)(*state + 0x3c8), 1, (void *)state[2]);
                *((unsigned char *)state + 0x65) = 1;
            }
        } else if (phase == 1) {
            if (queryTableEntry(*(int *)(*state + 0x384), 0) >= 0xb000) {
                Ov107_BuildAndSendUpdate(state[0], (short)*(int *)(*state + 0x3c8), 0, (void *)state[2]);
                *((unsigned char *)state + 0x65) = 2;
            }
        } else if (phase == 2) {
            if (queryTableEntry(*(int *)(*state + 0x384), 0) < 0xb000) {
                *((unsigned char *)state + 0x65) = 0;
            }
        }
    }
    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 10;
    state[0x17] = Ov107_FindNearestObject(*state, 0);
    if (state[0x17] == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    Ov235_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 3, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3a8), 2, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov235_ApproachTick);
}
