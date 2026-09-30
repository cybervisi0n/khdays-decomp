/*
 * Ov002_RecordMoveDelta - record a move delta and apply it (ARM).
 *
 * Snapshots the object's current position (the VecFx32 at param_1+0xd0) and stores the delta from the
 * requested target position param_2 into param_1's pending-move fields (+0x1a4/+0x1a8/+0x1ac). Then
 * hands the target off to Actor_SetVecAndSyncChild against the object's transform block at param_1+0x28.
 */

#include "nitro/fx_types.h"

extern void Actor_SetVecAndSyncChild(int a, int b);

void Ov002_RecordMoveDelta(int param_1, int *param_2)
{
    VecFx32 tmp = *(VecFx32 *)(param_1 + 0xd0);
    *(int *)(param_1 + 0x1a4) = param_2[0] - tmp.x;
    *(int *)(param_1 + 0x1a8) = param_2[1] - tmp.y;
    *(int *)(param_1 + 0x1ac) = param_2[2] - tmp.z;
    Actor_SetVecAndSyncChild(param_1 + 0x28, (int)param_2);
}
