/* Render callback of the ov218 actor's +0x3a8 joint: when the node being drawn is that joint (its
 * 0xae byte when flag bit 4 is set, else -1), the current matrix is read back (02016294) and its
 * translation stored in the actor's +0x39c point. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int m[9]; VecFx32 trans; } MtxFx43;

extern void NNS_G3dGetCurrentMtx(MtxFx43 *dst, void *src);

void Ov218_CaptureJoint(int node)
{
    int actor = *(int *)(*(int *)(node + 4) + 0x2c);
    MtxFx43 mtx;
    u32 key = (*(u32 *)(node + 8) & 0x10) ? *(u8 *)(node + 0xae) : 0xffffffff;

    if (key != *(u32 *)(actor + 0x3a8)) {
        return;
    }
    NNS_G3dGetCurrentMtx(&mtx, 0);
    *(VecFx32 *)(actor + 0x39c) = mtx.trans;
}
