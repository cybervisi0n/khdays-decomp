/* Hide entry of the ov297 enemy: spawns effect 0 and fires reaction 0x176 mode 4 at the +8
 * point, picks the overlay's hide point for the set chosen by +0x91, sets the +0x80 flag, zeroes
 * the +0x10 velocity, places the actor at the point, resets the +0x38 and +0x7c timers, requests
 * sub-state 2 and ends the state. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct HideTable { VecFx32 p[2]; };

extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int d);
extern void Ov107_BuildAndSendUpdate(int actor, int a, int b, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const struct HideTable data_ov297_020d569c;
extern const VecFx32 data_02041dc8;

void Ov297_HideEntry(int *node)
{
    int *state = (int *)node[1];
    VecFx32 pick;
    struct HideTable pts;
    int set;

    pts = data_ov297_020d569c;
    set = 0;
    if (*(signed char *)((char *)state + 0x91) != 0 && *(signed char *)((char *)state + 0x91) == 1) {
        set = 1;
    }
    func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[2], 0);
    Ov107_BuildAndSendUpdate(*state, 0x176, 4, (void *)state[2]);
    pick.x = pts.p[set].x;
    pick.y = pts.p[set].y;
    pick.z = pts.p[set].z;
    state[0x20] = 1;
    *(VecFx32 *)(state + 4) = data_02041dc8;
    Ov107_MoveNodeAndRelayout((Actor *)(*state), &pick);
    state[0xe] = 0;
    state[0x1f] = 0;
    *(unsigned char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
