/* Ov253_BoxSweepTick -- box sweep tick: every 0.5 (the +0x28 timer) the scene's +0xa8 list is
 * walked and each entity of the actor's +4 group that passes the +0x388 item's filter (020cceb0)
 * and lies inside the +0x2c..+0x40 box takes reaction 5 (020ca918, push data_02041dc8, 0x18)
 * from the item; the +0x24 timer runs up and past 6.0 sub-state 0 is requested and the node
 * slot released. */

#include "nitro/fx_types.h"

extern int List_First(int list);
extern int List_Next(int list);
extern int Ov253_IdIsFree(int item, int hit);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, const VecFx32 *push, int z);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;

void Ov253_BoxSweepTick(int *node) {
    int *state = (int *)node[1];
    int scene = *(int *)(*state + 4);
    int it;
    int hit;

    state[10] += *(int *)(node[0] + 0x2c);
    if (state[10] >= 0x800) {
        state[10] = 0;
        it = List_First(scene + 0xa8);
        hit = it == 0 ? 0 : *(int *)it;
        while (hit != 0) {
            if (*(int *)(hit + 4) == *(int *)(*state + 4) && Ov253_IdIsFree(*(int *)(*state + 0x388), hit) != 0) {
                if (*(int *)(hit + 0x74) >= state[0xb] && *(int *)(hit + 0x78) >= state[0xc] && *(int *)(hit + 0x7c) >= state[0xd] &&
                    *(int *)(hit + 0x74) <= state[0xe] && *(int *)(hit + 0x78) <= state[0xf] && *(int *)(hit + 0x7c) <= state[0x10]) {
                    Ov107_InvokeHitCallback(hit, *state, *(int *)(*state + 0x388), 5, &data_02041dc8, 0x18);
                }
            }
            it = List_Next(scene + 0xa8);
            hit = it == 0 ? 0 : *(int *)it;
        }
    }
    state[9] += *(int *)(node[0] + 0x2c);
    if (state[9] < 0x6000) {
        return;
    }
    *(unsigned char *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
