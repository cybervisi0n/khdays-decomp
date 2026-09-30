/* Physics tick of the ov146 actor: with no current move the +0xc velocity rests; the velocity is
 * mirrored to the actor's +0xf0 and then damped for the frame in 0x88-sized slices, by 0.08 per slice
 * on the ground (+0x17a bit 0) and 0.02 in the air. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { u8 b0 : 1; } Bit0;

extern int FX_Div(int num, int den);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern const VecFx32 data_02041dc8;

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

void Ov146_PhysicsTick(int *node)
{
    int *state = (int *)node[1];
    int remaining;

    if (*(signed char *)(*state + 0x1c6) == -1) {
        *(VecFx32 *)(state + 3) = data_02041dc8;
    }
    *(VecFx32 *)(*state + 0xf0) = *(VecFx32 *)(state + 3);
    for (remaining = *(int *)(node[0] + 0x2c); remaining > 0; remaining -= 0x88) {
        if (((Bit0 *)(*state + 0x17a))->b0) {
            ScaleVec3Fx12(0x1000 - FX_MUL(FX_Div(remaining <= 0x88 ? remaining : 0x88, 0x88), 0x148),
                          (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
        } else {
            ScaleVec3Fx12(0x1000 - FX_MUL(FX_Div(remaining <= 0x88 ? remaining : 0x88, 0x88), 0x52),
                          (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
        }
    }
}
