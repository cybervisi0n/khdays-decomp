/* Drift tick of the ov252 actor: it faces the target (020cdfe8 0, 1), the +0xc velocity is the +0x574
 * part's +0x2c vector turned by the +0x54 heading (020cdafc); once the partner holds no queued move the
 * queued +0x90 move becomes next and the node ends. */

#include "nitro/fx_types.h"

extern int Ov252_CheckTarget(int *node, int a, int b);
extern VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov252_DriftTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    Ov252_CheckTarget(node, 0, 1);
    v = Ov252_TurnVecY(state[0x15], (VecFx32 *)(*(int *)(*state + 0x574) + 0x2c));
    *(VecFx32 *)(state + 3) = v;
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    *(signed char *)(*state + 0x1c7) = *((signed char *)state + 0x90);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
