/* Take-off tick of an ov257 state: the +0x40 rate follows the frame rate and the nearest target
 * (020cab14) becomes +0x60; without one sub-state 2 is requested. Otherwise the path point is
 * resolved (Ov257_SteerToTarget) into the +0x10 step and, once the +0xc idle byte clears, animation
 * 2 and the +0x3d0 part's motion 1 play looped, +0x78 and +0x44 clear and the tick hands over to
 * Ov257_WindUpTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov107_FindNearestObject(int obj, int kind);
extern void Ov257_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov257_WindUpTick(int *node);

void Ov257_TakeOffTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 30;
    state[0x18] = Ov107_FindNearestObject(*state, 0);
    if (state[0x18] == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    Ov257_SteerToTarget(state, state[0x18], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 2, 1);
    Ov107_StartAnim(*(int *)(*state + 0x3d0), 1, 1);
    *((unsigned char *)state + 0x78) = 0;
    state[0x11] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov257_WindUpTick);
}
