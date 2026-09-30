/* Grab release of the ov227 enemy (shape of ov283 Ov283_Item_AiEnterRest): the owner's +0x388 grab is
 * dropped, bit 7 of its +0x60 high byte raised and bit 0 cleared, the +0xc step zeroed
 * (data_02041dc8), and the tick hands over to Ov227_ReleaseGrabIdleStep. */

#include "nitro/fx_types.h"

extern const VecFx32 data_02041dc8;
extern void SetIndexedSlot(int *a, int i, int v);
extern void Ov227_ReleaseGrabIdleStep(void);

struct node60_020cf740 { unsigned short lo : 8; unsigned short hi : 8; };

void Ov227_ReleaseGrab(int param_1) {
    int child = *(int *)(param_1 + 4);

    *(int *)(*(int *)child + 0x388) = 0;

    {
        unsigned short *p = (unsigned short *)(*(int *)child + 0x60);
        unsigned int hi = ((unsigned int)*p << 0x10) >> 0x18;
        hi |= 0x80;
        *p = (unsigned short)((*p & ~0xff00) | ((hi << 0x18) >> 16));
    }
    ((struct node60_020cf740 *)(*(int *)child + 0x60))->hi &= ~1;

    *(VecFx32 *)(child + 0xc) = data_02041dc8;

    SetIndexedSlot((int *)param_1, *(signed char *)(param_1 + 0x20), (int)&Ov227_ReleaseGrabIdleStep);
}
