/* Accumulate the owner rate (+0x2c) into the timer at (child)+0x2c; once it passes 0x700,
 * snapshot the pose vector (+0x54 -> +0x14) and step the height (+0x58 -= 0x80). Then, unless
 * the gate byte at *(child+0xc) is set, play the anim (ov107 mode 0x16,1), reset the timer and dispatch. */

#include "nitro/fx_types.h"

extern void Ov107_PostTagUpdate(int a, int b, int c);
extern int SetIndexedSlot(int a, int b, void *handler);
extern void Ov210_AdvanceLungeOrCommit(int);
void Ov210_AiRiseTick(int param_1) {
    int child = *(int *)(param_1 + 4);
    int t = *(int *)(child + 0x2c) + *(int *)(*(int *)param_1 + 0x2c);
    *(int *)(child + 0x2c) = t;
    if (t >= 0x700) {
        *(VecFx32 *)(child + 0x14) = *(VecFx32 *)(child + 0x54);
        *(int *)(child + 0x58) -= 0x80;
    }
    if (*(unsigned char *)*(int *)(child + 0xc) != 0) return;
    Ov107_PostTagUpdate(*(int *)child, 0x16, 1);
    *(int *)(child + 0x2c) = 0;
    SetIndexedSlot(param_1, *(signed char *)(param_1 + 0x20), (void *)&Ov210_AdvanceLungeOrCommit);
}
