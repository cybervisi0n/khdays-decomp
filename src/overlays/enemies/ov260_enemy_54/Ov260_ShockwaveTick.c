/* Shockwave tick of an ov260 part: the +0x40 timer accumulates the frame rate; for 0x440 a box at
 * the +0x34 point (rest axes, extent growing to 3.0 over the run, flagged) is swept for hits
 * (020d0e14); after that the next move is 0 and the node ends. */

#include "nitro/fx_types.h"

struct BoxQuery {
    VecFx32 vCenter;
    VecFx32 vAxisX;
    VecFx32 vAxisZ;
    VecFx32 vAxisY;
    int nExtent;
    int bFlag;
};

extern int Ov260_AttackHitTest(int *state, void *sphere, void *cyl);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042258;
extern const VecFx32 data_02042264;

void Ov260_ShockwaveTick(int *node)
{
    int *state = (int *)node[1];
    struct BoxQuery box;

    state[0x10] += *(int *)(node[0] + 0x2c);
    if (state[0x10] <= 0x440) {
        box.vCenter = *(VecFx32 *)(state + 0xd);
        box.nExtent = (state[0x10] * 3 << 12) / 0x440;
        box.vAxisX = data_02042270;
        box.vAxisZ = data_02042258;
        box.vAxisY = data_02042264;
        box.bFlag = 1;
        Ov260_AttackHitTest(state, 0, &box);
        return;
    }
    *(signed char *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
