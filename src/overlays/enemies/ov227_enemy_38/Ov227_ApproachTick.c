/* Approach tick of the ov227 enemy: the nearest target (020cab14) becomes the owner's +0x3e8; when
 * the tracking step (Ov227_MeasureTargetGap) reports it lost, or the move chooser
 * (Ov227_ChooseMove) queues a move, the tick ends. Otherwise the +0x38 goal is the +8 point
 * pushed 100 steps along the flat direction and the tick hands over to Ov227_WalkTick. */

#include "nitro/fx_types.h"

extern int Ov107_FindNearestObject(int obj, int kind);
extern int Ov227_MeasureTargetGap(int *node, VecFx32 *dir);
extern int Ov227_ChooseMove(int *node, int dist);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov227_WalkTick(int *node);

void Ov227_ApproachTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    int dist;

    *(int *)(*state + 0x3e8) = Ov107_FindNearestObject(*state, 0);
    dist = Ov227_MeasureTargetGap(node, &dir);
    if (dist < 0) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    if (Ov227_ChooseMove(node, dist) != 0) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    *(VecFx32 *)(state + 0xe) = *(VecFx32 *)state[2];
    state[0xe] += dir.x * 100;
    state[0x10] += dir.z * 100;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov227_WalkTick);
}
