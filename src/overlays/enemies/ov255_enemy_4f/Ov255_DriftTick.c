/* Drift tick of an ov255 spawned object: the +8 step is the forward axis turned by the +0x20
 * orientation and scaled by the +0x18 speed; the +0x1c timer accumulates the frame rate and past
 * 0.25 the owner's sub-state 0 is requested. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042258;

void Ov255_DriftTick(int *node)
{
    int *obj = (int *)node[1];
    VecFx32 fwd;

    Vec3TransformViaTempMtx(&fwd, (void *)obj[8], &data_02042258);
    ScaleVec3Fx12(obj[6], &fwd, (VecFx32 *)(obj + 2));
    obj[7] += *(int *)(node[0] + 0x2c);
    if (obj[7] <= 0x400) {
        return;
    }
    *(unsigned char *)(obj[0] + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
