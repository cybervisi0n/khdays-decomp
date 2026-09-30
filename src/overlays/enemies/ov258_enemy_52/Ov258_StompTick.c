/* Stomp tick of the ov258 actor: the +0x30 and +0x44 clocks run up at the frame rate with six step
 * cues (020cd6c8). At 2.99 (+0x50 = 3) and 3.98 (+0x50 = 2) a stomp arms the +0x458 / +0x45c hand
 * window, at 4.81 (+0x50 = 1) the +0x458 hand again. Each of the four stomps (+0x3c count, landing at
 * 2.29, 3.32, 4.22 and 4.98) drops its data_ov258_020d182c marker turned by the +0x28 heading at 15.6
 * height (effect 8 / 9 alternating, kept in +0x1c), and for 1/3 after landing an upright cylinder 0.5
 * above it (radius 12.0 / 6.0 alternating) hits once with push data_ov258_020d1820 (kind 1); between
 * stomps the +0x52 low mask clears. Each time the +4 rig finishes a remaining stomp (+0x52 high nibble)
 * replays pose 0xb (last) or 0xe with effect 3 or 4; with none left a follow-up (020cd2cc, without a
 * +0x38 delay) or move 2 follows. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;
typedef struct { VecFx32 pos; VecFx32 axis[3]; int radius; int flag; } Cylinder;
typedef struct { u8 lo : 4; u8 hi : 4; } NibblePair;

extern void Ov258_StepCue(int *node, int step, int phase, unsigned int variant);
extern void Ov258_ForwardEventIfStateOne(int partner, int from, int to, int d);
extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern int Ov258_AttackHitTest(int *node, void *sphere, void *box, void *capsule, void *cylinder, VecFx32 *push, int once, u16 effect, int kind);
extern void Ov107_PostTagUpdate(int a, int b, int c);
extern int Ov258_PickMove(int *node);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern const VecFx32 data_ov258_020d1820;
extern const VecFx32 data_ov258_020d182c;
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042258;
extern const VecFx32 data_02042264;
extern const VecFx32 data_02042270;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov258_StompTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 marker;
    Mtx33 rot;
    Cylinder cyl;
    VecFx32 push;

    state[0xc] += *(int *)(node[0] + 0x2c);
    state[0x11] += *(int *)(node[0] + 0x2c);
    Ov258_StepCue(node, 0x3e, 6, (u16)(*(int *)(*state + 0x460) != 0 ? 0x1b : 0x12));
    Ov258_StepCue(node, 0x40, 5, (u16)3);
    Ov258_StepCue(node, 0x60, 4, (u16)5);
    Ov258_StepCue(node, 0x78, 3, (u16)(*(int *)(*state + 0x460) != 0 ? 0x1b : 0x11));
    Ov258_StepCue(node, 0x7a, 2, (u16)4);
    Ov258_StepCue(node, 0x92, 1, (u16)5);
    if ((state[0xc] >= 0x2fd0 && *(u16 *)(state + 0x14) == 3) || (state[0xc] >= 0x3fc0 && *(u16 *)(state + 0x14) == 2)) {
        (*(u16 *)(state + 0x14))--;
        if (state[0xc] < 0x3fc0) {
            Ov258_ForwardEventIfStateOne(*(int *)(*state + 0x458), 0x2a8, 0x550, 1);
        } else {
            Ov258_ForwardEventIfStateOne(*(int *)(*state + 0x45c), 0x2a8, 0x550, 1);
        }
    }
    if (state[0xc] >= 0x4d08 && *(u16 *)(state + 0x14) == 1) {
        (*(u16 *)(state + 0x14))--;
        Ov258_ForwardEventIfStateOne(*(int *)(*state + 0x458), 0x110, 0x550, 1);
    }
    if ((state[0xf] == 0 && state[0xc] >= 0x24a8) || (state[0xf] == 1 && state[0xc] >= 0x3520) ||
        (state[0xf] == 2 && state[0xc] >= 0x4378) || (state[0xf] == 3 && state[0xc] >= 0x4fb0)) {
        marker = data_ov258_020d182c;
        {
            int idx = ANG2IDX(state[0xa]) * 2;

            MTX_RotY33_(&rot, data_0203d210[idx], data_0203d210[idx + 1]);
        }
        MTX_MultVec33(&marker, &rot, &marker);
        marker.y = 0xfa00;
        if (++state[0xf] % 2 == 1) {
            func_ov107_020c0b90(*state, 8, marker, 0);
        } else {
            func_ov107_020c0b90(*state, 9, marker, 0);
        }
        *(VecFx32 *)(state + 7) = marker;
    }
    if ((state[0xc] >= 0x24a8 && state[0xc] < 0x24a8 + 0x550) || (state[0xc] >= 0x3520 && state[0xc] < 0x3520 + 0x550) ||
        (state[0xc] >= 0x4378 && state[0xc] < 0x4378 + 0x550) || (state[0xc] >= 0x4fb0 && state[0xc] < 0x5500)) {
        push = data_ov258_020d1820;
        cyl.pos = *(VecFx32 *)(state + 7);
        cyl.pos.y += 0x800;
        cyl.axis[0] = data_02042270;
        cyl.axis[1] = data_02042258;
        cyl.axis[2] = data_02042264;
        cyl.radius = state[0xf] % 2 == 0 ? 0xc000 : 0x6000;
        cyl.flag = 1;
        Ov258_AttackHitTest(node, 0, 0, 0, &cyl, &push, 1, 0, 1);
    } else {
        ((NibblePair *)((u8 *)state + 0x52))->lo = 0;
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if (((NibblePair *)((u8 *)state + 0x52))->hi != 0) {
        ((NibblePair *)((u8 *)state + 0x52))->hi--;
        Ov107_PostTagUpdate(*state, ((NibblePair *)((u8 *)state + 0x52))->hi == 1 ? 0xb : 0xe, 0);
        func_ov107_020c0b90(*state, ((NibblePair *)((u8 *)state + 0x52))->hi == 1 ? 3 : 4, data_02041dc8, 0);
        return;
    }
    if (state[0xe] == 0 && Ov258_PickMove(node) != 0) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    *(signed char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
