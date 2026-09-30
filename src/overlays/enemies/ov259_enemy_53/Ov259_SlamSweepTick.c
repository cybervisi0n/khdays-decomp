/* Sweep tick of the ov259 actor's slam for 0xaa0 of its +0x68 timer: a box under it (centre 0.375
 * below its +0x74 position, axes from the rest frame and the +0x124 ground normal, extent 7 x the
 * elapsed fraction capped at 0x2b00) collects the entities it touches. Each one not yet struck (bit
 * `1 << id` of the +0xaf mask) that takes the hit (020ca918 kind 7, push 1.0 up) knocks the actor
 * back at its position (mode 0xb), queues pose 7 (020cd628) and is marked; entities no longer
 * touched are forgotten. Past 0xaa0, once the partner holds no queued move, +0x54 clears, the next
 * move is 2 when grounded (+0x17a bit 0) else 0xc, and the node ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Flag17a { u8 b0 : 1; };
struct BoxQuery {
    VecFx32 vCenter;
    VecFx32 vAxisX;
    VecFx32 vAxisZ;
    VecFx32 vAxisY;
    int nExtent;
    int bFlag;
};

extern int FX_Div(int num, int den);
extern int Ov107_CollectEntitiesTouchingDisc(int actor, struct BoxQuery *query, int *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov259_ArmPartnerCue(int *node, int pose, int delay);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042258;

void Ov259_SlamSweepTick(int *node)
{
    int *state = (int *)node[1];
    u16 i;

    state[0x1a] += *(int *)(node[0] + 0x2c);
    if (state[0x1a] < 0xaa0) {
        struct BoxQuery box;
        VecFx32 push;
        int hits[4];
        u8 seen;
        int extent;
        int count;

        seen = 0;
        extent = FX_Div(state[0x1a], 0x800);
        if (extent > 0x2b00) {
            extent = 0x2b00;
        }
        box.vCenter = *(VecFx32 *)(*state + 0x74);
        box.vCenter.y -= 0x600;
        box.vAxisX = data_02042270;
        box.vAxisZ = data_02042258;
        box.vAxisY = *(VecFx32 *)(*state + 0x124);
        box.nExtent = extent * 7;
        box.bFlag = 0;
        push.x = 0;
        push.y = 0x1000;
        push.z = 0;
        count = Ov107_CollectEntitiesTouchingDisc(*state, &box, hits);
        for (i = 0; i < count; i++) {
            u8 bit = 1 << *(u16 *)(hits[i] + 2);

            seen |= bit;
            if ((*((u8 *)state + 0xaf) & bit) == 0 &&
                Ov107_InvokeHitCallback(hits[i], *state, *state, 7, &push, 0)) {
                func_ov107_020c0b90(*state, 0xb, *(VecFx32 *)(hits[i] + 0x74), 0);
                Ov259_ArmPartnerCue(node, 7, 0);
                *((u8 *)state + 0xaf) |= bit;
            }
        }
        *((u8 *)state + 0xaf) &= seen;
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if (state[0x1a] <= 0xaa0) {
        return;
    }
    state[0x15] = 0;
    *(signed char *)(*state + 0x1c7) = ((struct Flag17a *)(*state + 0x17a))->b0 ? 2 : 0xc;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
