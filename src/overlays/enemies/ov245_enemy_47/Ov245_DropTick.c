/* Ov245_DropTick -- dive tick (dropping variant): while the actor is still falling
 * (020cce48) the +0x14 height follows the +0x20 speed, decayed once per 0x88 of the frame step;
 * once landed the +0x28 timer runs up and past 0.75, with +0x30 drops left, the first of the nine
 * +0x3fc slots without a +0x38c child is thrown (020ce818, base data_0204227c) from the +0x44c
 * item's +0x14 anchor along data_0204227c, both shifted -/+ (2.0 / 0.4375) in x for the +0x40
 * side 0 / 2; the drop count falls, the timer restarts and the side cycles through 0..2.
 * With no drops left the node moves to 020cdbfc. */

#include "nitro/fx_types.h"

struct Ov245Actor { char pad[0x3fc]; int slots[9]; };

extern int Ov245_AnimGate(int actor);
extern int FX_Div(int num, int den);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov245_HitReact2(int self, VecFx32 *at, VecFx32 *dir, const VecFx32 *base);
extern void Ov245_WaitPartsSettled(void);
extern const VecFx32 data_0204227c;

void Ov245_DropTick(int *node) {
    int *state = (int *)node[1];
    VecFx32 at;
    VecFx32 dir;
    int rest;
    int i;

    if (Ov245_AnimGate(*state) != 0) {
        state[5] = state[8];
        rest = *(int *)(node[0] + 0x2c);
        while (rest > 0) {
            int ratio = FX_Div(rest <= 0x88 ? rest : 0x88, 0x88);
            int t = (int)(((long long)ratio * 0x80 + 0x800) >> 12);
            state[8] = (int)(((long long)state[8] * (0x1000 - t) + 0x800) >> 12);
            rest -= 0x88;
        }
        return;
    }
    state[10] += *(int *)(node[0] + 0x2c);
    if (state[10] < 0xc00) {
        return;
    }
    if (state[0xc] <= 0) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_WaitPartsSettled);
        return;
    }
    for (i = 0; i < 9; i++) {
        if (*(int *)(((struct Ov245Actor *)*state)->slots[i] + 0x38c) == 0) {
            dir = data_0204227c;
            at = *(VecFx32 *)(*(int *)(*state + 0x44c) + 0x14);
            switch (*((unsigned char *)state + 0x40)) {
            case 0:
                at.x -= 0x2000;
                dir.x -= 0x700;
                break;
            case 2:
                at.x += 0x2000;
                dir.x += 0x700;
                break;
            }
            Ov245_HitReact2(((struct Ov245Actor *)*state)->slots[i], &at, &dir, &data_0204227c);
            state[0xc]--;
            state[10] = 0;
            *((unsigned char *)state + 0x40) = (*((unsigned char *)state + 0x40) + 1) % 3;
            return;
        }
    }
}
