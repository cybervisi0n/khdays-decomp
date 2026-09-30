/* Hover tick of an ov256 part: the +0x10 velocity is the +0x450 owner's +0x2c vector turned by the
 * part's heading (020cd054); once the partner holds no queued move bit 6 of the +0x60 high byte is
 * set, the next move is 2 and the node ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void Ov256_RotateByActorHeading(int *out, int param_2, int *vec);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov256_PartHoverTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    Ov256_RotateByActorHeading((int *)&v, (int)node, (int *)(*(int *)(*state + 0x450) + 0x2c));
    *(VecFx32 *)(state + 4) = v;
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    {
        u16 hw = *(u16 *)(*state + 0x60);
        *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x40) << 0x18) >> 0x10);
    }
    *(signed char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
