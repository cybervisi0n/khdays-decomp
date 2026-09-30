/* Drift tick of an ov256 part: the +0x10 velocity is the +0x450 owner's +0x2c vector turned by the
 * part's heading (020cd054); once the partner holds no queued move pose 0x1c plays and the node moves
 * on to 020d01bc. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov256_RotateByActorHeading(int *out, int param_2, int *vec);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_PartWanderTick(void);

void Ov256_PartDriftTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    Ov256_RotateByActorHeading((int *)&v, (int)node, (int *)(*(int *)(*state + 0x450) + 0x2c));
    *(VecFx32 *)(state + 4) = v;
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x1c, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_PartWanderTick);
}
