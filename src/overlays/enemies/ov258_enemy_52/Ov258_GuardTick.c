/* Guard tick of the ov258 actor: the +0x30 and +0x44 clocks run up at the frame rate with step cues at
 * 1, 8 and 0x4a x 0x88. At 0.33 the guard opens (+0x50 2 -> 1) with effects 0x17 / 0x18 at the origin;
 * from 2.49 to 4.18 the pending counter plays effects 0x19 / 0x1a at both hands (+0x44c / +0x450) and the
 * guard sound (0x13 with a +0x460 partner, else 0x1a) at the +0x430 rig, and in move 6 it starts the
 * vortex (+0x54 bit 3, +0x1c point (0, 15.6, 8.0), sound 0x12/0x19, effect 0x2a, 8.0 of time). From
 * 2.52 for 0.8 an aim marker (020cede0, alternating sides) is dropped every 0x110 after re-acquiring
 * the target. Once the +4 rig is idle: before 3.98 pose 0xd replays; move 6 continues with 020ce5d8;
 * otherwise a follow-up (020cd2cc, without a +0x38 delay) or move 2. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov258_StepCue(int *node, int step, int phase, unsigned int variant);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int bank, u16 variant, int at);
extern int Ov258_AcquireTarget(int *node, int face);
extern void Ov258_AimMarker(int *node, int side);
extern int Ov258_PickMove(int *node);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov258_EnterBarrage(void);
extern const VecFx32 data_02041dc8;

void Ov258_GuardTick(int *node)
{
    int *state = (int *)node[1];

    state[0xc] += *(int *)(node[0] + 0x2c);
    state[0x11] += *(int *)(node[0] + 0x2c);
    Ov258_StepCue(node, 1, 3, (u16)9);
    Ov258_StepCue(node, 8, 2, (u16)(*(int *)(*state + 0x460) != 0 ? 0x1b : 0x14));
    Ov258_StepCue(node, 0x4a, 1, (u16)10);
    if (state[0xc] >= 0x550 && *(u16 *)(state + 0x14) == 2) {
        VecFx32 origin;

        (*(u16 *)(state + 0x14))--;
        origin = data_02041dc8;
        func_ov107_020c0b90(*state, 0x17, origin, 0);
        func_ov107_020c0b90(*state, 0x18, origin, 0);
    }
    if (state[0xc] >= 0x27d8 && state[0xc] < 0x42f0 && *(u16 *)(state + 0x14) != 0) {
        (*(u16 *)(state + 0x14))--;
        func_ov107_020c0b90(*state, 0x19, *(VecFx32 *)(*(int *)(*state + 0x44c) + 0x14), 0);
        func_ov107_020c0b90(*state, 0x1a, *(VecFx32 *)(*(int *)(*state + 0x450) + 0x14), 0);
        Ov107_BuildAndSendUpdate(*state, *(short *)(state + 0x16), *(int *)(*state + 0x460) != 0 ? 0x13 : 0x1a,
                            *(int *)(*state + 0x430) + 0x14);
        if (*(signed char *)(*state + 0x1c6) == 6) {
            *((u8 *)state + 0x54) |= 8;
            state[7] = 0;
            state[8] = 0xfa00;
            state[9] = 0x8000;
            Ov107_BuildAndSendUpdate(*state, *(short *)(state + 0x16), *(int *)(*state + 0x460) != 0 ? 0x12 : 0x19,
                                (int)(state + 7));
            func_ov107_020c0b90(*state, 0x2a, *(VecFx32 *)(state + 7), 0);
            *(int *)(*state + 0x428) = 0x8000;
        }
    }
    if (state[0xc] >= 0x2860 && state[0xc] < 0x2860 + 0xcc0) {
        state[0xd] += *(int *)(node[0] + 0x2c);
        if (state[0xd] >= 0x110) {
            Ov258_AcquireTarget(node, 0);
            state[0xd] = 0;
            Ov258_AimMarker(node, (u8)(state[0xf] % 2 != 0));
            state[0xf]++;
        }
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if (state[0xc] < 0x3fc0) {
        Ov107_PostTagUpdate((Actor *)(*state), 0xd, 0);
        return;
    }
    if (*(signed char *)(*state + 0x1c6) == 6) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov258_EnterBarrage);
        return;
    }
    if (state[0xe] == 0 && Ov258_PickMove(node) != 0) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    *(signed char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
