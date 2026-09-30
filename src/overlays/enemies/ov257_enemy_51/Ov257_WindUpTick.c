/* Wind-up tick of an ov257 state: the +0x44 timer accumulates the frame rate and a three-phase
 * trigger on the rig's channel-0 frame (+0x78) fires reaction +0x408 mode 0 at 2.0 and mode 1 at
 * 11.0 at the +8 point, restarting below 11.0. The +0x40 rate follows the frame rate; without a
 * nearest target (020cab14, kept in +0x60) sub-state 2 is requested. Otherwise the +0x10 step heads
 * for it (Ov257_SteerToTarget); with the +0x4c cooldown out a target more than 2.0 above or below,
 * or 5.0 on the timer, picks 0xd; within 2.0 the rig's +0xa8 byte clears and the tick hands over to
 * Ov257_WindUpHoldTick. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern int Ov107_FindNearestObject(int obj, int kind);
extern int Ov257_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov257_WindUpHoldTick(int *node);

void Ov257_WindUpTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;
    int dist;

    state[0x11] += *(int *)(node[0] + 0x2c);
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
    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 30;
    state[0x18] = Ov107_FindNearestObject(*state, 0);
    if (state[0x18] == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    dist = Ov257_SteerToTarget(state, state[0x18], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (state[0x13] <= 0) {
        int dy = *(int *)(*state + 0xb4) - *(int *)(state[0x18] + 0x78);

        if (dy < 0) {
            dy = -dy;
        }
        if (dy > 0x2000) {
            *(unsigned char *)(*state + 0x1c7) = 0xd;
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
            return;
        }
    }
    if (state[0x11] >= 0x5000) {
        *(unsigned char *)(*state + 0x1c7) = 0xd;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    if (dist > 0x2000) {
        return;
    }
    *(unsigned char *)(*(int *)(*state + 0x384) + 0xa8) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov257_WindUpHoldTick);
}
