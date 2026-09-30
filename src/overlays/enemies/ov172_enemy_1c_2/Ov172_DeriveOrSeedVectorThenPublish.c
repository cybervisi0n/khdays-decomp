
#include "nitro/fx_types.h"

extern void Srt_SetRotationQuat(int dst, int *src);
extern int data_02041dc8;

// If the object is in sub-state 1, derive the working vector at node+0x30 from
// node+0x20; otherwise seed it with the default (data_02041dc8). Either way,
// publish node+0x30 into the object's live vector (node[0]+0xf0).
void Ov172_DeriveOrSeedVectorThenPublish(int *this)
{
    int *node = (int *)this[1];
    if (*(signed char *)(node[0] + 0x1c6) == 1) {
        Srt_SetRotationQuat(node[0] + 0xa0, (int *)((int)node + 0x20));
    } else {
        *(VecFx32 *)((int)node + 0x30) = *(VecFx32 *)&data_02041dc8;
    }
    *(VecFx32 *)(node[0] + 0xf0) = *(VecFx32 *)((int)node + 0x30);
}
