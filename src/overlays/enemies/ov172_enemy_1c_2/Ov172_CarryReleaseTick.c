/* Carry-release tick of the ov171 enemy (and its byte-identical twins). Needs a +0xc carried
 * target; while the target's 64-bit flags at +0x464 lack bit 15 the +0x4c timer accumulates the
 * frame-time and past 0x2a7 the state ends (sub-state 0). Once the bit is set, the +0x10 drop
 * point and the +0x38c item's +0x74 position are both settled on the ground by the sibling
 * settle routine (each with the other's radius holder), the target is moved to the settled item
 * position (its +0x4ec rider too, with the rider's +0x40 bit-1 hook), the target is released via
 * Ov022_ToggleBit13ByMode, the item is moved to the settled drop point (same hook), and the state
 * ends with sub-state 0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef void (*Hook)(int obj, int a);

extern void Ov172_SettleCarryPosition(int scene, VecFx32 *pos, int actor);
extern void Ov107_MoveNodeAndRelayout(int obj, VecFx32 *at);
extern void Ov022_ToggleBit13ByMode(int target, int a);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov172_CarryReleaseTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 drop;
    VecFx32 at;
    int scene;
    int rider;
    int item;

    if (state[3] == 0) {
        return;
    }
    if ((*(u64 *)((char *)state[3] + 0x464) & 0x8000) != 0) {
        scene = *(int *)(*state + 4);
        drop = *(VecFx32 *)(state + 4);
        at = *(VecFx32 *)(*(int *)(*state + 0x38c) + 0x74);
        Ov172_SettleCarryPosition(scene, &drop, *(int *)(*state + 0x38c) + 0x74);
        Ov172_SettleCarryPosition(scene, &at, (int)(state + 4));
        Actor_SetVecAndSyncChild(*(void **)(state[3] + 0x20), &at);
        rider = *(int *)(state[3] + 0x4ec);
        if (rider != 0) {
            *(VecFx32 *)(rider + 0x190) = at;
            Ov107_MoveNodeAndRelayout(rider, (VecFx32 *)(rider + 0x190));
            rider = *(int *)(state[3] + 0x4ec);
            if (((*(int *)(rider + 0x40) << 30) >> 31) != 0 && *(Hook *)(rider + 0xc) != 0) {
                (*(Hook *)(rider + 0xc))(rider, 0);
            }
        }
        Ov022_ToggleBit13ByMode(state[3], 0);
        Ov107_MoveNodeAndRelayout(*(int *)(*state + 0x38c), &drop);
        item = *(int *)(*state + 0x38c);
        if (((*(int *)(item + 0x40) << 30) >> 31) != 0 && *(Hook *)(item + 0xc) != 0) {
            (*(Hook *)(item + 0xc))(item, 0);
        }
        *(u8 *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
    } else {
        state[0x13] += *(int *)(*node + 0x2c);
        if (state[0x13] >= 0x2a8) {
            *(u8 *)(*state + 0x1c7) = 0;
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        }
    }
}
