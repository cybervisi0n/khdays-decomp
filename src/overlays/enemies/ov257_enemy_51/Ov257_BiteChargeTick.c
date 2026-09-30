/* Bite tick of an ov257 state: a two-phase trigger on the rig's channel-0 frame (+0x78) fires
 * reaction +0x408 mode 1 at the +8 point once it reaches 6.0 and rearms below it. The +0x40 rate is
 * the frame rate x 3; without a nearest target (020cab14, kept in +0x60) sub-state 2 is requested,
 * otherwise the +0x10 step heads for it (Ov257_SteerToTarget) and, once the +0xc idle byte clears,
 * sub-state 2 is requested as well. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern int Ov107_FindNearestObject(int obj, int kind);
extern int Ov257_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov257_BiteChargeTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    {
        unsigned char phase = *((unsigned char *)state + 0x78);

        if (phase == 0) {
            if (queryTableEntry(*(int *)(*state + 0x384), 0) >= 0x6000) {
                Ov107_BuildAndSendUpdate(state[0], (short)*(int *)(*state + 0x408), 1, (void *)state[2]);
                *((unsigned char *)state + 0x78) = 1;
            }
        } else if (phase == 1) {
            if (queryTableEntry(*(int *)(*state + 0x384), 0) < 0x6000) {
                *((unsigned char *)state + 0x78) = 0;
            }
        }
    }
    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 10;
    state[0x18] = Ov107_FindNearestObject(*state, 0);
    if (state[0x18] == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    Ov257_SteerToTarget(state, state[0x18], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    *(unsigned char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
