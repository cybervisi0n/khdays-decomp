/* Ov253_CarriedSetup -- carried item setup: the +4 item takes the +0x39c joint's +4 transform
 * (scale flag 1 at +0x30), bit 1 of its +0x5c cleared and +0x70 set to 1; the state keeps the
 * +0x39c and +0x398 joints' +0x14 anchors at +8 / +0x14, clears +0x20 and moves the node to
 * 020d1248. */

#include "nitro/fx_types.h"

typedef struct { int m[11]; } Pose44;
struct Ov253Item { char pad[0x30]; Pose44 pose; };
struct Ov253Joint { char pad[4]; Pose44 pose; };

extern void Srt_SetScaleUniform(void *srt, int scale);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov253_CarriedLiftTick(void);

void Ov253_CarriedSetup(int *node) {
    int *state = (int *)node[1];

    ((struct Ov253Item *)state[1])->pose = ((struct Ov253Joint *)*(int *)(*state + 0x39c))->pose;
    Srt_SetScaleUniform((void *)(state[1] + 0x30), 1);
    *(int *)(state[1] + 0x5c) &= ~2;
    *(unsigned short *)(state[1] + 0x70) = 1;
    *(VecFx32 *)(state + 2) = *(VecFx32 *)(*(int *)(*state + 0x39c) + 0x14);
    *(VecFx32 *)(state + 5) = *(VecFx32 *)(*(int *)(*state + 0x398) + 0x14);
    state[8] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov253_CarriedLiftTick);
}
