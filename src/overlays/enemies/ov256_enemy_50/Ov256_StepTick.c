/* Step tick of the ov256 actor: the +0x10 velocity is the +0x450 owner's +0x2c vector turned by its
 * heading (020cd054); once the partner holds no queued move +0x54, +0x4c and the +0x69 charges clear,
 * the +0x70 turn direction is rolled (+1 or -1), pose 1 plays, the +0x450 part takes motion 0 and the
 * node moves on to 020cdc98. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern void Ov256_RotateByActorHeading(int *out, int param_2, int *vec);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_WalkTick(void);

void Ov256_StepTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    Ov256_RotateByActorHeading((int *)&v, (int)node, (int *)(*(int *)(*state + 0x450) + 0x2c));
    *(VecFx32 *)(state + 4) = v;
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    state[0x15] = 0;
    state[0x13] = 0;
    *((u8 *)state + 0x69) = 0;
    *((signed char *)state + 0x70) = RandNextScaled(2) == 0 ? 1 : -1;
    Ov107_PostTagUpdate((Actor *)(*state), 1, 0);
    Ov107_StartAnim(*(int *)(*state + 0x450), 0, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_WalkTick);
}
