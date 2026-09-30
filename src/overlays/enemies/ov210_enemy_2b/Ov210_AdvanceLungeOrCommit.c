/*
 * Ov210_AdvanceLungeOrCommit -- x3 (ov210/211/282). AI-state tick: advance a lunge, then commit or wait.
 * Timer state[0xb] += owner_delta. Copy the working vec state[0x15..0x17] down to state[5..7]
 * (field-to-field) and ease state[0x16] -= 0x80. If the timer is still <= 0x800 and the facing bit0
 * at *state+0x17a is clear, keep waiting. Otherwise snap state[5..7] to the const vec data_02041dc8,
 * fire attack 0x17 (020c9264) and hand off to the 020d326c state.
 */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct S210 { char pad[0x14]; VecFx32 a; char pad2[0x34]; VecFx32 b; };
struct b17a { unsigned char b0 : 1; };
extern void SetIndexedSlot(int self, int idx, int cb);
extern void Ov210_AiQueue2OnFlagClearB(void);
extern VecFx32 data_02041dc8;

void Ov210_AdvanceLungeOrCommit(int *self) {
    int *state = (int *)self[1];

    state[0xb] += *(int *)(*self + 0x2c);
    ((struct S210 *)state)->a = ((struct S210 *)state)->b;
    state[0x16] -= 0x80;
    if (state[0xb] <= 0x800) {
        if (((struct b17a *)(*state + 0x17a))->b0 == 0) {
            return;
        }
    }
    ((struct S210 *)state)->a = data_02041dc8;
    Ov107_PostTagUpdate((Actor *)(*state), 0x17, 0);
    SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), (int)&Ov210_AiQueue2OnFlagClearB);
}
