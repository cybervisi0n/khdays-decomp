/* Turn tick of the ov256 actor: the +0x10 velocity is the +0x450 owner's +0x2c vector turned by its
 * heading (020cd054); once the partner holds no queued move a side is rolled (0 or 2) and the target
 * re-picked (020ccd54); a heading change of more than 35 degrees forces side 2. Pose 0x1f + side plays,
 * the +0x450 part takes motion 0x10 + side and the node moves on to 020ce650. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Ov256_RotateByActorHeading(int *out, int param_2, int *vec);
extern int Ov256_PickTarget(int *node);
extern void Ov107_PostTagUpdate(int actor, int pose, int loop);
extern int Ov107_StartAnim(int part, int motion, int mode);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_RetreatTick(void);

void Ov256_TurnTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;
    u8 side;
    int heading;

    Ov256_RotateByActorHeading((int *)&v, (int)node, (int *)(*(int *)(*state + 0x450) + 0x2c));
    *(VecFx32 *)(state + 4) = v;
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    side = RandNextScaled(2) == 0 ? 2 : 0;
    heading = state[0x10];
    Ov256_PickTarget(node);
    if (heading + 0x1922 < state[0x10] || heading - 0x1922 > state[0x10]) {
        side = 2;
    }
    Ov107_PostTagUpdate(*state, side + 0x1f, 0);
    Ov107_StartAnim(*(int *)(*state + 0x450), side + 0x10, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_RetreatTick);
}
