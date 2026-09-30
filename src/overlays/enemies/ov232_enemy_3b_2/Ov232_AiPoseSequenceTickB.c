/* Compute the offset vector (020cca74) into (child)+0x30; unless the gate byte at
 * *(child+0x10) is set, advance the step counter (+0x28): on the 3rd step mark sub-state
 * 2 and dispatch, otherwise re-pose both nodes (ov107 mode counter+0xd). */

#include "nitro/fx_types.h"

extern void Ov232_rotateVecByOwnerYaw(void *out, int a, int b);
extern int SetIndexedSlot(int a, int b, void *handler);
extern void Ov107_PostTagUpdate(int a, int b, int c);
extern void Ov107_StartAnim(int a, int b, int c);
void Ov232_AiPoseSequenceTickB(int param_1) {
    int child = *(int *)(param_1 + 4);
    VecFx32 out;
    int counter;
    Ov232_rotateVecByOwnerYaw(&out, param_1, *(int *)(*(int *)child + 0x388) + 0x2c);
    *(VecFx32 *)(child + 0x30) = out;
    if (*(unsigned char *)*(int *)(child + 0x10) != 0) return;
    counter = *(int *)(child + 0x28) + 1;
    *(int *)(child + 0x28) = counter;
    if (counter == 3) {
        *(signed char *)(*(int *)child + 0x1c7) = 2;
        SetIndexedSlot(param_1, *(signed char *)(param_1 + 0x20), (void *)0);
    } else {
        Ov107_PostTagUpdate(*(int *)child, counter + 0xd, 0);
        Ov107_StartAnim(*(int *)(*(int *)child + 0x388), *(int *)(child + 0x28) + 0xd, 0);
    }
}
