/* Drift entry tick of the ov252 actor: the +0xc velocity is the +0x574 part's +0x2c vector turned by
 * the +0x54 heading (020cdafc); once the partner holds no queued move pose 0xe plays, the part takes
 * motion 0x13 and the node moves on to 020d09cc. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_DriftLandTick(void);

void Ov252_DriftEntryTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    v = Ov252_TurnVecY(state[0x15], (VecFx32 *)(*(int *)(*state + 0x574) + 0x2c));
    *(VecFx32 *)(state + 3) = v;
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0xe, 0);
    Ov107_StartAnim(*(int *)(*state + 0x574), 0x13, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_DriftLandTick);
}
