/* Approach-start tick of an ov255 state: the +0x40 rate is the frame rate x 3 and the nearest
 * target (020cab14) becomes +0x5c; without one sub-state 2 is requested. Otherwise the +0x10 step
 * heads for it (Ov255_SteerToTarget) and, once the +0xc idle byte clears, animation 3 plays, the
 * +0x3a4 part plays motion 2 and the tick hands over to Ov255_ApproachTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov107_FindNearestObject(int obj, int kind);
extern void Ov255_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov255_ApproachTick(int *node);

void Ov255_ApproachStartTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 10;
    state[0x17] = Ov107_FindNearestObject(*state, 0);
    if (state[0x17] == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    Ov255_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 3, 0);
    Ov107_StartAnim(*(int *)(*state + 0x3a4), 2, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov255_ApproachTick);
}
