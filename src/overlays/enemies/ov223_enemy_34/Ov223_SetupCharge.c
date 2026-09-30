/* Charge setup of the ov223 enemy's state: keeps the variant at +0x48, the caller's target
 * point (the second by-value vector; the first is unused) at +0x20, zeroes the +0x14 velocity, +0x44 and the +0x4c byte, sets the
 * +0x2c pose to face the target from data_02042258 (ed60) and requests sub-state 1. */

#include "nitro/fx_types.h"

typedef struct { int q[4]; } Quat;

extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042258;

void Ov223_SetupCharge(int *state, int nVariant, VecFx32 vFrom, VecFx32 vTarget)
{
    state[0x12] = nVariant;
    *(VecFx32 *)(state + 8) = vTarget;
    *(VecFx32 *)(state + 5) = data_02041dc8;
    state[0x11] = 0;
    *(unsigned char *)((char *)state + 0x4c) = 0;
    Quat_FromTwoVectors((Quat *)(state + 0xb), &data_02042258, &vTarget);
    *(unsigned char *)(*state + 0x1c7) = 1;
}
