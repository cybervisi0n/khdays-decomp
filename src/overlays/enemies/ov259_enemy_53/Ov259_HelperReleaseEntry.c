/* Release entry of an ov259 helper: its +0x38c shape hides, the +0x394 owner is knocked back at the
 * helper's +0x74 position (mode 7) and the next move is 1. */

#include "nitro/fx_types.h"

typedef struct { unsigned f : 8; } B8;

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov259_HelperReleaseEntry(int *node)
{
    int *state = (int *)node[1];

    ((B8 *)(*(int *)(*state + 0x38c) + 8))->f &= ~1;
    func_ov107_020c0b90(*(int *)(*state + 0x394), 7, *(VecFx32 *)(*state + 0x74), 0);
    *(signed char *)(*state + 0x1c7) = 1;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
