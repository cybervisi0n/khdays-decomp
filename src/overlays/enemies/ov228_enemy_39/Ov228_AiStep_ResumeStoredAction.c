/* Only while flag 0 (low byte at (*child)+0x60) is set: pick a landing point at
 * (child)+0x54 = base(+0x224) + rand(|+0x228 - +0x224| + 1), reset the child state
 * (+0x60=-1, offset vec at +0x28 from the const, +8=0, +0x64=0), copy the sub-state
 * byte +0x1c9 into +0x1c7, then dispatch with no handler. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern int SetIndexedSlot(int a, int b, void *handler);
extern const VecFx32 data_02041dc8;
struct hw60lo_020d34b8 { unsigned short lo : 8; unsigned short hi : 8; };
void Ov228_AiStep_ResumeStoredAction(int param_1) {
    int child = *(int *)(param_1 + 4);
    int obj = *(int *)child;
    int base, d;
    if ((((struct hw60lo_020d34b8 *)(obj + 0x60))->lo & 1) == 0) return;
    base = *(int *)(obj + 0x224);
    d = *(int *)(obj + 0x228) - base;
    if (d < 0) d = -d;
    *(int *)(child + 0x54) = base + RandNextScaled(d + 1);
    *(signed char *)(child + 0x60) = -1;
    *(VecFx32 *)(child + 0x28) = data_02041dc8;
    *(int *)(child + 8) = 0;
    *(signed char *)(child + 0x64) = 0;
    *(signed char *)(*(int *)child + 0x1c7) = *(signed char *)(*(int *)child + 0x1c9);
    SetIndexedSlot(param_1, *(signed char *)(param_1 + 0x20), (void *)0);
}
