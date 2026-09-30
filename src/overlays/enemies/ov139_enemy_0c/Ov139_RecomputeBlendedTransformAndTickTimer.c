
#include "nitro/fx_types.h"
#include "game/engine.h"

extern void Quat_FromTwoVectors(int *out, int *tbl, int v);
extern void Quat_Multiply(int *out, int *a, int *b);
extern void Srt_SetRotationQuat(int dst, int *src);
extern int data_02042264;
extern int data_02041dc8;

// Recompute the node scalar, build the combined transform (blend of the table
// vector and the object matrix at node[0]+0x124) into node[0]+0xa0, tick down the
// countdown at node[0x10], and shift/reload the vector triple at node[5..7].
void Ov139_RecomputeBlendedTransformAndTickTimer(int *this)
{
    int *node = (int *)this[1];
    int delta[4];
    int base[4];
    node[2] = Angle_TurnToward(node[2], node[3], node[4], 0);
    QuatFromAxisAngle(delta, &data_02042264, node[2]);
    Quat_FromTwoVectors(base, &data_02042264, node[0] + 0x124);
    Quat_Multiply(base, base, delta);
    Srt_SetRotationQuat(node[0] + 0xa0, base);
    if (node[0x10] >= 0) {
        node[0x10] = node[0x10] - *(int *)(*this + 0x2c);
    }
    {
        VecFx32 *triple = (VecFx32 *)(node + 5);
        *(VecFx32 *)(node[0] + 0xf0) = *triple;
        *triple = *(VecFx32 *)&data_02041dc8;
    }
}
