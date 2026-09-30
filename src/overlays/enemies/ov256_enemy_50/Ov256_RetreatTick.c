/* Retreat tick of the ov256 actor: the +0x10 velocity is the +0x450 owner's +0x2c vector turned by
 * its heading (020cd054); once the partner holds no queued move, in retreat mode 2 (+0x6b) a fresh
 * pick (020ccdf0) other than move 9 just ends the node; otherwise mode 2 is set, the next move is the
 * +0x74 mode + 2 and the node ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void Ov256_RotateByActorHeading(int *out, int param_2, int *vec);
extern int Ov256_PickMove(int *node);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov256_RetreatTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    Ov256_RotateByActorHeading((int *)&v, (int)node, (int *)(*(int *)(*state + 0x450) + 0x2c));
    *(VecFx32 *)(state + 4) = v;
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if (*((u8 *)state + 0x6b) == 2 && Ov256_PickMove(node) && *(signed char *)(*state + 0x1c7) != 9) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    *((u8 *)state + 0x6b) = 2;
    *(signed char *)(*state + 0x1c7) = state[0x1d] + 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
