/* Raise flag 0x80 in the high byte at (*child)+0x60, reset +0x388, clear flag 0 in the high
 * byte at (*child)+0x60, copy the const offset vector into (child)+8, and register the handler. */

#include "nitro/fx_types.h"

extern const VecFx32 data_02041dc8;
extern int SetIndexedSlot(int a, int b, void *handler);
extern void Ov267_ReactionResetIdleStep(int);
struct node60_020d1168 { unsigned short lo : 8; unsigned short hi : 8; };
void Ov267_EnterReactionReset(int param_1) {
    int child = *(int *)(param_1 + 4);
    {
        unsigned short *p = (unsigned short *)(*(int *)child + 0x60);
        unsigned int hi = ((unsigned int)*p << 0x10) >> 0x18;
        hi |= 0x80;
        *p = (unsigned short)((*p & ~0xff00) | ((hi << 0x18) >> 16));
    }
    *(int *)(*(int *)child + 0x388) = 0;
    ((struct node60_020d1168 *)(*(int *)child + 0x60))->hi &= ~1;
    *(VecFx32 *)(child + 8) = data_02041dc8;
    SetIndexedSlot(param_1, *(signed char *)(param_1 + 0x20), (void *)&Ov267_ReactionResetIdleStep);
}
