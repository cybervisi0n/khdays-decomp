
/* Starts a two-point move: marks the owner busy, stores both endpoints and sets phase 1. */

#include "nitro/fx_types.h"

void Ov245_StartTwoPointMove(int *self, VecFx32 *from, VecFx32 *to) {
    *(int *)(*self + 0x38c) = 1;
    *(VecFx32 *)((char *)self + 0x18) = *from;
    *(VecFx32 *)((char *)self + 0x24) = *to;
    *(char *)(*self + 0x1c7) = 1;
}
