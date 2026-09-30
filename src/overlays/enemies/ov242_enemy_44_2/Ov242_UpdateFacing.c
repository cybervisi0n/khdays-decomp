/* Facing update of the ov241 enemy (x3: ov241/242/243): eases the +4 heading towards the +8
 * target at the +0x28 rate, turns it into a quaternion about the world Y axis for the actor's
 * +0xa0 orientation, shifts the +0x10 offset into the actor's +0xf0 and reloads the zero vector
 * into it (the Ov120_RecomputeNodeVectorAndReloadTriple shape). */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Srt_SetRotationQuat(int dst, int *src);
extern int data_02042264;
extern int data_02041dc8;

void Ov242_UpdateFacing(int *this)
{
    int *node = (int *)this[1];
    int scratch[4];
    node[1] = Angle_TurnToward(node[1], node[2], node[10], 0);
    QuatFromAxisAngle(scratch, &data_02042264, node[1]);
    Srt_SetRotationQuat(node[0] + 0xa0, scratch);
    {
        VecFx32 *triple = (VecFx32 *)(node + 4);
        *(VecFx32 *)(node[0] + 0xf0) = *triple;
        *triple = *(VecFx32 *)&data_02041dc8;
    }
}
