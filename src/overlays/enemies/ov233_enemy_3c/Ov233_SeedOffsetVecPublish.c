/*
 * Ov233_SeedOffsetVecPublish -- x3 (ov228/...). Seed the local offset vector, then publish it to the owner.
 * state = self[1]. If the mode byte *(s8)(*state + 0x1c6) == 0, reset the local vec state[2..4] to the
 * const data_02041dc8. Then copy state[2..4] into the owner block at *state + 0xf0.
 */

#include "nitro/fx_types.h"

extern VecFx32 data_02041dc8;

void Ov233_SeedOffsetVecPublish(int *self) {
    int *state = (int *)self[1];

    if (*(signed char *)(*state + 0x1c6) == 0) {
        *(VecFx32 *)(state + 2) = data_02041dc8;
    }
    *(VecFx32 *)(*state + 0xf0) = *(VecFx32 *)(state + 2);
}
