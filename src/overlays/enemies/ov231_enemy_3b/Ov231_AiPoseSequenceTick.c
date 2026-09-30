/* Compute the offset vector (020cca74) into (child)+0x30; unless the gate byte at
 * *(child+0x10) is set, advance the step counter (+0x28): steps 0-1 pose both nodes with
 * anim 1, step 2 with anim 0xc, and from step 3 mark sub-state 2 and dispatch. */

#include "nitro/fx_types.h"

extern void Ov231_rotateVecByOwnerYaw(void *out, int a, int b);
extern void Ov107_PostTagUpdate(int a, int b, int c);
extern void Ov107_StartAnim(int a, int b, int c);
extern int SetIndexedSlot(int a, int b, void *handler);
void Ov231_AiPoseSequenceTick(int param_1) {
    int child = *(int *)(param_1 + 4);
    VecFx32 out;
    int counter;
    Ov231_rotateVecByOwnerYaw(&out, param_1, *(int *)(*(int *)child + 0x388) + 0x2c);
    *(VecFx32 *)(child + 0x30) = out;
    if (*(unsigned char *)*(int *)(child + 0x10) != 0) return;
    counter = *(int *)(child + 0x28) + 1;
    *(int *)(child + 0x28) = counter;
    if (counter < 2) {
        Ov107_PostTagUpdate(*(int *)child, 1, 0);
        Ov107_StartAnim(*(int *)(*(int *)child + 0x388), 1, 0);
    } else if (counter < 3) {
        Ov107_PostTagUpdate(*(int *)child, 0xc, 0);
        Ov107_StartAnim(*(int *)(*(int *)child + 0x388), 0xc, 0);
    } else {
        *(signed char *)(*(int *)child + 0x1c7) = 2;
        SetIndexedSlot(param_1, *(signed char *)(param_1 + 0x20), (void *)0);
    }
}
