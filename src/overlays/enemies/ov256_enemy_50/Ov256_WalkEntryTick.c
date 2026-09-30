/* Walk entry tick of the ov256 actor: the +0x10 velocity is the +0x450 owner's +0x2c vector turned by
 * its heading (020cd054); once the partner holds no queued move, coming from move 7 the +0x44 target
 * heading swings by 70 degrees (and +0x40 follows), a pending retreat (+0x6b) re-picks the target, the
 * step count +0x54 = 5, +0x4c and +0x74 clear, the +0x71 turn direction is rolled, pose 2 plays, the
 * +0x450 part takes motion 1 and the node moves on to 020ce050. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern void Ov256_RotateByActorHeading(int *out, int param_2, int *vec);
extern int Ov256_PickTarget(int *node);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_AiTakeOff(void);

void Ov256_WalkEntryTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    Ov256_RotateByActorHeading((int *)&v, (int)node, (int *)(*(int *)(*state + 0x450) + 0x2c));
    *(VecFx32 *)(state + 4) = v;
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if (*(signed char *)(*state + 0x1c8) == 7) {
        state[0x10] = state[0x11] += 0x3244;
    }
    if (*((u8 *)state + 0x6b) != 0) {
        Ov256_PickTarget(node);
    }
    state[0x15] = 5;
    state[0x13] = 0;
    *((signed char *)state + 0x71) = RandNextScaled(2) == 0 ? 1 : -1;
    state[0x1d] = 0;
    Ov107_PostTagUpdate((Actor *)(*state), 2, 0);
    Ov107_StartAnim(*(int *)(*state + 0x450), 1, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_AiTakeOff);
}
