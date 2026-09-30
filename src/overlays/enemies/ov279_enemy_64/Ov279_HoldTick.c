/* Hold tick of the ov279 enemy. While the owner's +0x3ac grab partner is
 * missing or not flagged 0x8000 in its +0x464 state, the +0x50 timer runs and past 0x2a8 sub-state
 * 8 is requested. Otherwise the owner is placed (c5c54) 0.94 behind the +8 target (its rig yaw
 * turned half a circle), the +0x1c facing aims from the +0x4c point at the target and is committed
 * to +0xc, and once the +4 animator is free animation 6 plays (looping), the timer restarts, +0x54
 * becomes 0.5, the target's height is kept at +0x6c and the tick hands over to
 * Ov279_CarryTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[4]; } Quat;
struct Partner { char pad[0x464]; unsigned long long flags; };

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

extern void Mtx33_LookAt(int *dst, const VecFx32 *a, const VecFx32 *b, const void *c);
extern void Quat_FromMtx33(void *dst, const int *src);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern const VecFx32 data_02042264;
extern void Ov279_CarryTick(int *node);

void Ov279_HoldTick(int *node)
{
    int target;
    int *state = (int *)node[1];
    int partner = *(int *)(*state + 0x3ac);
    int mtx[9];
    VecFx32 pos;
    int yaw;
    int rad;

    if (partner == 0 || (((struct Partner *)partner)->flags & 0x8000) == 0) {
        state[0x14] += *(int *)(*node + 0x2c);
        if (state[0x14] < 0x2a8) {
            return;
        }
        *(unsigned char *)(*state + 0x1c7) = 8;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    target = state[2];
    yaw = (*(unsigned short *)(*(int *)(*(int *)(target + 0x18c) + 0x20) + 0x80) - 0x8000) & 0xffff;
    rad = (int)(((long long)yaw * 0x6487fLL + 0x80000LL) >> 20);
    pos.x = *(int *)(target + 0x74) + (int)(((long long)data_0203d210[ANG2IDX(rad) * 2] * 0x1e00LL + 0x800LL) >> 12);
    pos.y = *(int *)(target + 0x78);
    pos.z = *(int *)(target + 0x7c) + (int)(((long long)data_0203d210[ANG2IDX(rad) * 2 + 1] * 0x1e00LL + 0x800LL) >> 12);
    Ov107_MoveNodeAndRelayout((Actor *)(*state), &pos);
    Mtx33_LookAt(mtx, (VecFx32 *)(target + 0x74), (VecFx32 *)state[0x13], &data_02042264);
    Quat_FromMtx33(state + 7, mtx);
    *(Quat *)(state + 3) = *(Quat *)(state + 7);
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 6, 1);
    state[0x14] = 0;
    state[0x15] = 0x800;
    state[0x1b] = *(int *)(target + 0x78);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov279_CarryTick);
}
