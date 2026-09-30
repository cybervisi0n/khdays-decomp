/*
 * Ov282_TrackLeaderRangeCheck -- x3. AI-state tick: track the leader, aim below it, then range-check the
 * acquired target. Copy the leader position (**(state[0]+0x3d4))+0xb0 into state[0xd..0xf]; if the
 * leader's sub-node (+0x190) exists, cache state[0xe] = *(that+0x44). Aim point w = state[0xd..0xf]
 * with Y lowered by 0x6000, fed to 020c5c54. Acquire a target -> state[4]; if found, dist =
 * VEC_Mag(flatten_y(target(+0x74) - state[0]+0x74)) and pick next-state 7 (in range, dist>0x4000) or
 * 0x10 (too far); if no target, next-state 0x10. Hand off via 0203c634 (cb=0).
 */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int  Ov107_FindNearestObject(int obj, int *out);
extern void VEC_Subtract(void *a, void *b, void *c);
extern int  VEC_Mag(int *v);
extern void SetIndexedSlot(int self, int idx, int cb);

void Ov282_TrackLeaderRangeCheck(int *self) {
    int *state = (int *)self[1];
    VecFx32 w;
    int v[3];
    int dist;
    int target;
    int c;

    *(VecFx32 *)(state + 0xd) = *(VecFx32 *)(*(int *)(*(int *)(*state + 0x3d4)) + 0xb0);
    c = *(int *)(*(int *)(*(int *)(*state + 0x3d4)) + 0x190);
    if (c != 0) {
        state[0xe] = *(int *)(c + 0x44);
    }
    w = *(VecFx32 *)(state + 0xd);
    w.y -= 0x6000;
    Ov107_MoveNodeAndRelayout((Actor *)(*state), &w);
    target = Ov107_FindNearestObject(*state, &dist);
    state[4] = target;
    if (target != 0) {
        VEC_Subtract((void *)(target + 0x74), (void *)(*state + 0x74), v);
        v[1] = 0;
        dist = VEC_Mag(v);
        if (dist > 0x4000) {
            *(char *)(*state + 0x1c7) = 7;
        } else {
            *(char *)(*state + 0x1c7) = 0x10;
        }
    } else {
        *(char *)(*state + 0x1c7) = 0x10;
    }
    SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), 0);
}
