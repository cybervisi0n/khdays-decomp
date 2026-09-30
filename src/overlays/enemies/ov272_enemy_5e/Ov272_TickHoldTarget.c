/* Ov272_TickHoldTarget -- hold tick of the ov279 enemy: keeps itself at the point 1.875 in front
 * of the held target (along the target model's yaw), faces it, and drops the hold (owner +0x1c7 =
 * 2) once the target is gone, belongs to another world or is no longer active. If the target
 * drifted more than 1.5 away, is flagged 0x20 at +0x1e4, or its rider is busy or dead, the enemy
 * is pushed out of the world at the target's height (+0x1c7 = 8). Otherwise, once the +0x50 timer
 * reaches 1.0, it releases the target: plays animation 5, clears the grab bits, spawns effect
 * 0x167 at the anchor, hands the rider to 020ad8e0 and moves on to Ov272_HoldTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[4]; } Quat;
struct hw60 { unsigned short lo : 8, hi : 8; };
struct lo8 { unsigned int lo : 8; };
typedef struct { char pad[0x464]; unsigned long long flags; char pad2[0x12]; } Rider;

extern void Mtx33_LookAt(int *dst, const VecFx32 *a, const VecFx32 *b, const void *c);
extern void Quat_FromMtx33(void *dst, const int *src);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int  VEC_Mag(const VecFx32 *v);
extern void Ov272_PushOutOfWorld(int world, VecFx32 *pos, int rad, int height);
extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void Ov022_ToggleBit13ByMode(int rider, int n);
extern void Ov272_HoldTick(int *node);
extern const short data_0203d210[];
extern const VecFx32 data_02042264;
extern const VecFx32 data_02041dc8;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)
#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

void Ov272_TickHoldTarget(int *node)
{
    int *state = (int *)node[1];
    int mtx[9];
    VecFx32 pos;
    VecFx32 d;
    int target = state[2];
    int yaw = (*(unsigned short *)(*(int *)(*(int *)(target + 0x18c) + 0x20) + 0x80) - 0x8000) & 0xffff;
    int rad = (int)(((long long)yaw * 0x6487f + 0x80000) >> 20);
    VecFx32 *tpos = (VecFx32 *)(target + 0x74);
    int rider;

    pos.x = *(int *)(target + 0x74) + FX_MUL(data_0203d210[ANG2IDX(rad) * 2], 0x1680);
    pos.y = tpos->y;
    pos.z = tpos->z + FX_MUL(data_0203d210[ANG2IDX(rad) * 2 + 1], 0x1680);
    Ov107_MoveNodeAndRelayout((Actor *)(*state), &pos);
    Mtx33_LookAt(mtx, tpos, (VecFx32 *)state[0x13], &data_02042264);
    Quat_FromMtx33(state + 7, mtx);
    *(Quat *)(state + 3) = *(Quat *)(state + 7);
    if (state[2] == 0 || *(int *)(state[2] + 4) != *(int *)(*state + 4)
        || !(((struct hw60 *)(state[2] + 0x60))->lo & 1)) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    VEC_Subtract((VecFx32 *)(state + 0x1d), tpos, &d);
    if (VEC_Mag(&d) > 0x1800 || (*(unsigned int *)(state[2] + 0x1e4) & 0x20)
        || ((rider = *(int *)(state[2] + 0x18c)) != 0
            && ((((Rider *)rider)->flags & 0x8000) != 0 || *(unsigned short *)(rider + 0x12) == 0))) {
        pos = *tpos;
        pos.y += *(int *)(state[2] + 0x80) + *(int *)(*state + 0x80);
        Ov272_PushOutOfWorld(*(int *)(*state + 4), &pos, rad, *(int *)(*state + 0x80));
        Ov107_MoveNodeAndRelayout((Actor *)(*state), &pos);
        *(unsigned char *)(*state + 0x1c7) = 8;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    state[0x14] += *(int *)(*node + 0x2c);
    if (!(state[0x14] < 0x1000)) {
        func_ov107_020c0b90(*state, 4, data_02041dc8, 0);
        Ov107_PostTagUpdate((Actor *)(*state), 5, 0);
        ((struct hw60 *)(*state + 0x60))->hi &= ~4;
        *(unsigned short *)(*state + 0x1ae) &= ~1;
        ((struct lo8 *)(*(int *)(*state + 0x388) + 8))->lo |= 1;
        Ov107_BuildAndSendUpdate(*state, 0x167, 7, (void *)state[0x13]);
        *(int *)(*state + 0x3ac) = *(int *)(state[2] + 0x18c);
        Ov022_ToggleBit13ByMode(*(int *)(*state + 0x3ac), 1);
        state[0x14] = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov272_HoldTick);
        return;
    }
}
