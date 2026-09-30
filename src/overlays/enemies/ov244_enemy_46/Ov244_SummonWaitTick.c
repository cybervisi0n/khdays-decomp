/* Summon wait tick: counts the +0x1c timer up by the scene step; past 1.5 the first of the four
 * +0x408 parts whose +0x60 low byte has bit 0 clear is launched (020d0c74) at the nearest mate's
 * +0x190 point raised by 10.0, the low half of +0x18 counts up and the timer resets. Once that
 * count reaches 8 the +0x384 item's +0xa8 flag clears and the node moves to 020d0c14. */

#include "nitro/fx_types.h"

struct Hw60 { unsigned short lo : 8; unsigned short hi : 8; };
struct Count18 { int lo : 16; int hi : 16; };
extern char *Ov244_PickNearestMate(char *actor);
extern void Ov244_RunSetupThenSetHw60HighBit0(int part, VecFx32 *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov244_AiQueue2OnFlagClearB(void);

void Ov244_SummonWaitTick(int *node) {
    int *state = (int *)node[1];
    VecFx32 at;
    int i;

    state[7] += *(int *)(*node + 0x2c);
    if (state[7] > 0x1800) {
        for (i = 0; i < 4; i++) {
            int part = ((int *)*(int *)(*state + 0x408))[i];
            if ((((struct Hw60 *)(part + 0x60))->lo & 1) == 0) {
                char *mate = Ov244_PickNearestMate((char *)*state);
                at.x = *(int *)(mate + 0x190);
                at.y = *(int *)(mate + 0x194) + 0xa000;
                at.z = *(int *)(mate + 0x198);
                Ov244_RunSetupThenSetHw60HighBit0(((int *)*(int *)(*state + 0x408))[i], &at);
                ((struct Count18 *)(state + 6))->lo += 1;
                state[7] = 0;
                break;
            }
        }
    }
    if (((struct Count18 *)(state + 6))->lo < 8) {
        return;
    }
    *(unsigned char *)(*(int *)(*state + 0x384) + 0xa8) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov244_AiQueue2OnFlagClearB);
}
