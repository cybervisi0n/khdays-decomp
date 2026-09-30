/* Combo tick of the ov258 actor: the +0x30 and +0x44 clocks run up at the frame rate (the +0x30 clock
 * then restarts at 0.53 with a +0x460 partner, else 0) and six step cues fire along the combo
 * (020cd6c8). Each time the +4 rig finishes a remaining swing (+0x52 high nibble) is spent: the +0x50
 * counter drops, the clock clears, pose 10 (last swing) or 0xf plays with effect 2 (last) or 5 at the
 * origin and the +0x458 hand arms its 0.5 to 0.83 window; the final swing also arms the +0x45c hand.
 * With no swing left, without a +0x38 delay a follow-up (020cd2cc) may be picked, else the next move
 * is 2. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { u8 lo : 4; u8 hi : 4; } NibblePair;

extern void Ov258_StepCue(int *node, int step, int phase, unsigned int variant);
extern void Ov107_PostTagUpdate(int a, int b, int c);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov258_ForwardEventIfStateOne(int partner, int from, int to, int d);
extern int Ov258_PickMove(int *node);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;

void Ov258_ComboTick(int *node)
{
    int *state = (int *)node[1];

    state[0xc] += *(int *)(node[0] + 0x2c);
    state[0x11] += *(int *)(node[0] + 0x2c);
    state[0xc] = *(int *)(*state + 0x460) != 0 ? 0x880 : 0;
    Ov258_StepCue(node, 0x52, 6, (u16)(*(int *)(*state + 0x460) != 0 ? 0x1b : 0x11));
    Ov258_StepCue(node, 0x54, 5, (u16)0);
    Ov258_StepCue(node, 0x6e, 4, (u16)(*(int *)(*state + 0x460) != 0 ? 0x1b : 0x12));
    Ov258_StepCue(node, 0x70, 3, (u16)1);
    Ov258_StepCue(node, 0xb6, 2, (u16)(*(int *)(*state + 0x460) != 0 ? 0x1b : 0x13));
    Ov258_StepCue(node, 0xd2, 1, (u16)2);
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if (((NibblePair *)((u8 *)state + 0x52))->hi != 0) {
        ((NibblePair *)((u8 *)state + 0x52))->hi--;
        (*(u16 *)(state + 0x14))--;
        state[0xc] = 0;
        Ov107_PostTagUpdate(*state, ((NibblePair *)((u8 *)state + 0x52))->hi == 1 ? 10 : 0xf, 0);
        func_ov107_020c0b90(*state, *(u16 *)(state + 0x14) == 1 ? 2 : 5, data_02041dc8, 0);
        Ov258_ForwardEventIfStateOne(*(int *)(*state + 0x458), 0x7f8, 0xd48, 0);
        if (*(u16 *)(state + 0x14) != 0) {
            return;
        }
        Ov258_ForwardEventIfStateOne(*(int *)(*state + 0x45c), 0x7f8, 0xd48, 0);
        return;
    }
    if (state[0xe] == 0 && Ov258_PickMove(node) != 0) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    *(signed char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
