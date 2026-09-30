/*
 * Ov268_KickFixedMotion -- x3. AI-state tick: kick off a fixed motion, then dispatch.
 * Call 020c0b90(state[0], mode 2, const vec data_02041dc8, flag 0) -- passes the vec3 by value.
 * Reset state[0xb] = 0 (elapsed) and the *(u8)(state+0x49) flag, then hand off to the 020d22e0 state.
 */

#include "nitro/fx_types.h"

extern VecFx32 data_02041dc8;
extern void func_ov107_020c0b90(int obj, int mode, VecFx32 v, int flag);
extern void SetIndexedSlot(int self, int idx, int cb);
extern void Ov268_AiStrikeWindup(void);

void Ov268_KickFixedMotion(int *self) {
    int *state = (int *)self[1];

    func_ov107_020c0b90(*state, 2, data_02041dc8, 0);
    state[0xb] = 0;
    *((char *)state + 0x49) = 0;
    SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), (int)&Ov268_AiStrikeWindup);
}
