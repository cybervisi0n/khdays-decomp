
#include "nitro/fx_types.h"

extern void Ov256_RotateByActorHeading(void *out, int self, int arg);
extern int SetIndexedSlot(int self, int idx, void *handler);

/* Reaction tick: compute a world position via 020cd054 (from pActor+0x450 chain +0x2c) into
 * node+0x10, and unless the sub-target is locked (node[1]+0xad), bump the reaction state to
 * node[0x1d]+2 and re-arm the slot with no handler. node=*(ReactionAiNode**)(self+4); *node=pActor. */
void Ov256_TickReactionAim(int self) {
    int *node = *(int **)(self + 4);
    VecFx32 local;
    Ov256_RotateByActorHeading(&local, self, *(int *)(*node + 0x450) + 0x2c);
    *(VecFx32 *)((char *)node + 0x10) = local;
    if (*(unsigned char *)(node[1] + 0xad) == 0) {
        *(char *)(*node + 0x1c7) = node[0x1d] + 2;
        SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
    }
}
