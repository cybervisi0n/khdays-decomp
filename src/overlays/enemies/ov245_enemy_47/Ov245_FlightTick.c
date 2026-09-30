/* Ov245_FlightTick -- flight tick while the actor is aloft (020cce48 == 0): counts the
 * actor's +0x43c parts still active (bit 0 of +0x60); the +0x30 timer runs down by the frame
 * step and, once out while the +0x42c owner holds no +0x390 target, the actor is released
 * (020d1290), every active part detached (020d54d4), sub-state 2 set and the node slot freed.
 * Otherwise, with no part active and no target either, a random heading around +0x24
 * (RandNextScaled(0x595) - 0x2ca) becomes the launch direction handed to the owner (020d1234
 * with the +0x44c item's +0x14 anchor), +0x28 is cleared, the actor reset (020cce08) and the
 * node moves to 020ce0d8. */

#include "nitro/fx_types.h"

struct hw60 { unsigned short lo : 8, hi : 8; };
struct Ov245Actor { char pad[0x43c]; int parts[3]; };

extern int Ov245_AnimGate(int actor);
extern int Ov245_ForwardToAiTaskWhenReady(int owner);
extern int Ov245_Mounted_ForceState9IfReady(int part);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int RandNextScaled(int scale);
extern void Ov245_InvokeHookAndRearm_2(int owner, void *anchor, VecFx32 *dir);
extern void Ov245_SetNodeMode3(int actor);
extern void Ov245_AiDelayThenFlight(void);
extern const short data_0203d210[];

void Ov245_FlightTick(int *node) {
    int *state = (int *)node[1];
    VecFx32 dir;
    int j;
    int active;
    int i;
    int idx;

    if (Ov245_AnimGate(*state) != 0) {
        return;
    }
    j = 0;
    active = 0;
    for (; j < 3; j++) {
        if ((((struct hw60 *)(((struct Ov245Actor *)*state)->parts[j] + 0x60))->lo & 1) != 0) {
            active++;
        }
    }
    state[0xc] -= *(int *)(node[0] + 0x2c);
    if (state[0xc] <= 0) {
        state[0xc] = 0;
        if (*(int *)(*(int *)(*state + 0x42c) + 0x390) == 0) {
            Ov245_ForwardToAiTaskWhenReady(*(int *)(*state + 0x42c));
            for (i = 0; i < 3; i++) {
                if ((((struct hw60 *)(((struct Ov245Actor *)*state)->parts[i] + 0x60))->lo & 1) != 0) {
                    Ov245_Mounted_ForceState9IfReady(((struct Ov245Actor *)*state)->parts[i]);
                }
            }
            *(unsigned char *)(*state + 0x1c7) = 2;
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
            return;
        }
    }
    if (active > 0) {
        return;
    }
    if (*(int *)(*(int *)(*state + 0x42c) + 0x390) != 0) {
        return;
    }
    idx = (unsigned short)((0x28BE60DB9391LL * (state[9] + (RandNextScaled(0x595) - 0x2ca)) + 0x80000000000LL) >> 44);   /* FX_RAD_TO_IDX */
    dir.x = data_0203d210[(idx >> 4) << 1];
    dir.y = 0;
    dir.z = data_0203d210[((idx >> 4) << 1) + 1];
    Ov245_InvokeHookAndRearm_2(*(int *)(*state + 0x42c), (void *)(*(int *)(*state + 0x44c) + 0x14), &dir);
    state[10] = 0;
    Ov245_SetNodeMode3(*state);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_AiDelayThenFlight);
}
