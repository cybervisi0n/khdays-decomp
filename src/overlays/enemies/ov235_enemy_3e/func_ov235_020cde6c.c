/* Wind-up tick of an ov235 state: a three-phase trigger on the rig's channel-0 frame (0203bec0)
 * -- at 1.0 reaction +0x3c8 mode 1 fires at the +8 point, at 11.0 mode 0, and once the frame falls
 * back below 11.0 the cycle restarts (+0x65). The +0x40 rate follows the frame rate; without a
 * nearest target (020cab14, kept in +0x5c) sub-state 2 is requested. Otherwise the +0x10 step
 * heads for it (Ov235_SteerToTarget); once it is within 4.0 or the +0x4c cooldown has run out,
 * the rig's +0xa8 byte clears and the tick hands over to Ov235_WindUpHoldTick. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern int Ov107_FindNearestObject(int obj, int kind);
extern int Ov235_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov235_WindUpHoldTick(int *node);

void func_ov235_020cde6c(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;
    int dist;

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
    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 30;
    state[0x17] = Ov107_FindNearestObject(*state, 0);
    if (state[0x17] == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    dist = Ov235_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (dist > 0x4000 && state[0x13] > 0) {
        return;
    }
    *(unsigned char *)(*(int *)(*state + 0x384) + 0xa8) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov235_WindUpHoldTick);
}
