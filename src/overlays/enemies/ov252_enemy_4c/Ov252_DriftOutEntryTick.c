/* Drift-out entry tick of the ov252 actor: the +0xc velocity follows the +0x574 part's +0x2c vector
 * turned by the +0x54 heading; once the partner holds no queued move pose 0x17 plays, the part takes
 * motion 0x16, the +0x88 flag and +0x9c timer clear and the node moves on to 020d0cc0. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_SinkTick(void);

void Ov252_DriftOutEntryTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    v = Ov252_TurnVecY(state[0x15], (VecFx32 *)(*(int *)(*state + 0x574) + 0x2c));
    *(VecFx32 *)(state + 3) = v;
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x17, 0);
    Ov107_StartAnim(*(int *)(*state + 0x574), 0x16, 0);
    *((unsigned char *)state + 0x88) = 0;
    state[0x27] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_SinkTick);
}
