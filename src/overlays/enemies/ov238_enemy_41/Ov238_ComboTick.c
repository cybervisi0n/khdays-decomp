/* Combo tick of the ov238 enemy: the +0x20 clock runs up at the owner's rate. In four windows the
 * owner steers by the overlay's offset (020d07f0); in three strike windows a sphere (radius 1.125)
 * on the +0x3f0 hand (the +0x3f4 one in the second window) sweeps the target (020d0910), counting
 * hits in +0x24 with reaction 0x12e mode 0xc. At 3.83 / 4.9 / 5.99 a +0x31 charge is spent with
 * reactions 0x12e modes 6 and 0xf; cue 0xe plays at tick 0x4a. From 11.0 the overlay's lunge offset,
 * turned by the +0x18 yaw, places a growing box 1.4 below the owner (message 0xa the first time,
 * message 4 at the hand and reaction 0x12e mode 7 on a +0x2e request) that sweeps towards the +0x3e4
 * partner's +0x190 point, counting hits with reaction 0x50. Once the +4 rig is idle a hit round
 * before the third +0x2d strike hands over to 020d18a4, a missed one replays pose 0xb + strike and
 * restarts (020d1e74), and after the third strike sub-state 2 follows. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;
typedef struct { VecFx32 pos; int nRadius; } Sphere;
typedef struct { VecFx32 pos; VecFx32 axis[3]; VecFx32 half; } Box;

extern void Ov238_TurnVelocity(int *node, VecFx32 *vec);
extern int Ov238_AttackHitTest(int *node, Sphere *sphere, Box *box, VecFx32 *push, int b, int c, int d);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, int at);
extern void Ov238_TimedCue(int *node, int ticks, int cue, int variant);
extern void MTX_RotY33_(Mtx33 *m, int sin, int cos);
extern void MTX_MultVec33(VecFx32 *v, Mtx33 *m, VecFx32 *d);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void VEC_Subtract(void *a, void *b, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void Ov107_PostTagUpdate(int owner, int pose, int loop);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov238_AiComboStart(void);
extern void Ov238_AiStep_QueueAction2OnAnimEnd(void);
extern const short data_0203d210[];
extern const VecFx32 data_ov238_020d36c0;
extern const VecFx32 data_ov238_020d36b4;
extern const VecFx32 data_ov238_020d36cc;
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042264;
extern const VecFx32 data_02042258;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov238_ComboTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 push = data_ov238_020d36c0;
    Sphere sphere;
    VecFx32 steer;
    Mtx33 mtx;
    Box box;
    VecFx32 off;
    int t;
    u8 busy;

    sphere.nRadius = 0x1200;
    state[8] += *(int *)(node[0] + 0x2c);
    t = state[8];
    if ((t >= 0xc38 && t < 0xc38 + 0x220) || (t >= 0x1188 && t < 0x1188 + 0x220) ||
        (t >= 0x16d8 && t < 0x16d8 + 0x220) || (t >= 0x2640 && t < 0x28e8)) {
        steer = data_ov238_020d36b4;
        Ov238_TurnVelocity(node, &steer);
    }
    t = state[8];
    if ((t >= 0xdd0 && t < 0xff0) || (t >= 0x1320 && t < 0x1540) || (t >= 0x16d8 && t < 0x16d8 + 0x198)) {
        if (t < 0x1320 || t >= 0x1540) {
            sphere.pos = *(VecFx32 *)(*(int *)(*state + 0x3f0) + 0x14);
        } else {
            sphere.pos = *(VecFx32 *)(*(int *)(*state + 0x3f4) + 0x14);
        }
        if (Ov238_AttackHitTest(node, &sphere, 0, &push, 1, 3, 0) != 0) {
            state[9]++;
            Ov107_BuildAndSendUpdate(*state, 0x12e, 0xc, state[2]);
        }
    }
    t = state[8];
    if ((t >= 0xf68 && *((u8 *)state + 0x31) == 4) || (t >= 0x13a8 && *((u8 *)state + 0x31) == 3) ||
        (t >= 0x17e8 && *((u8 *)state + 0x31) == 2)) {
        *((u8 *)state + 0x31) -= 1;
        Ov107_BuildAndSendUpdate(*state, 0x12e, 6, state[2]);
        Ov107_BuildAndSendUpdate(*state, 0x12e, 0xf, state[2]);
    }
    Ov238_TimedCue(node, 0x4a, 1, 0xe);
    if (state[8] >= 0x2c18) {
        off = data_ov238_020d36cc;
        {
            unsigned int idx = ANG2IDX(state[6]);

            MTX_RotY33_(&mtx, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
        }
        MTX_MultVec33(&off, &mtx, &off);
        if (*((u8 *)state + 0x2d) == 3) {
            *((u8 *)state + 0x2d) += 1;
            func_ov107_020c0b90(*state, 0xa, *(VecFx32 *)state[2], 0);
        }
        if (*((u8 *)state + 0x2e) != 0) {
            int owner;

            *((u8 *)state + 0x2e) = 0;
            owner = *state;
            func_ov107_020c0b90(owner, 4, *(VecFx32 *)(*(int *)(owner + 0x3f0) + 0x14), 0);
            Ov107_BuildAndSendUpdate(*state, 0x12e, 7, state[2]);
        }
        box.pos = *(VecFx32 *)(*state + 0x74);
        box.pos.y -= 0x1680;
        box.pos.x += off.x;
        box.pos.z += off.z;
        box.axis[0] = data_02042270;
        box.axis[1] = data_02042264;
        box.axis[2] = data_02042258;
        box.half.x = ((state[8] - 0x2e38) << 3) + 0xc00;
        box.half.y = 0x300;
        box.half.z = ((state[8] - 0x2e38) << 3) + 0xc00;
        VEC_Subtract((void *)(*(int *)(*state + 0x3e4) + 0x190), &box.pos, &push);
        VEC_Normalize(&push, &push);
        if (Ov238_AttackHitTest(node, 0, &box, &push, 1, 3, 0) != 0) {
            Ov107_BuildAndSendUpdate(*state, 0, 0x50, *state + 0x74);
            state[9]++;
        }
    }
    busy = *(u8 *)(state[1] + 0xad);
    if (busy == 0 && state[9] != 0 && *((u8 *)state + 0x2d) < 3) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov238_AiComboStart);
        return;
    }
    if (busy == 0 && state[9] == 0 && *((u8 *)state + 0x2d) < 3) {
        Ov107_PostTagUpdate(*state, *((u8 *)state + 0x2d) + 0xb, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov238_AiStep_QueueAction2OnAnimEnd);
        return;
    }
    if (busy != 0) {
        return;
    }
    if (*((u8 *)state + 0x2d) < 3) {
        return;
    }
    *(u8 *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
