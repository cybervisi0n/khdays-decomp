/* Take-off tick of an ov235 state: the +0x40 rate follows the frame rate and the nearest target
 * (020cab14) becomes +0x5c; without one sub-state 2 is requested. Otherwise the path point is
 * resolved (Ov235_SteerToTarget) into the +0x10 step and, once the +0xc idle byte clears, animation
 * 2 and the +0x3a8 part's motion 1 play looped, +0x65 clears and the tick hands over to
 * func_ov235_020cde6c. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov107_FindNearestObject(int obj, int kind);
extern void Ov235_SteerToTarget(int *state, int point, VecFx32 *dir, int *speed);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void func_ov235_020cde6c(int *node);

void Ov235_TakeOffTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int speed;

    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 30;
    state[0x17] = Ov107_FindNearestObject(*state, 0);
    if (state[0x17] == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    Ov235_SteerToTarget(state, state[0x17], &dir, &speed);
    ScaleVec3Fx12(speed, &dir, (VecFx32 *)(state + 4));
    if (*(unsigned char *)state[3] != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 2, 1);
    Ov107_StartAnim(*(int *)(*state + 0x3a8), 1, 1);
    *((unsigned char *)state + 0x65) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)func_ov235_020cde6c);
}
