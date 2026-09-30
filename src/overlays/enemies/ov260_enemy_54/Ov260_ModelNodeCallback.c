/* Node callback of the ov260 model: when the node's key (its +0xae byte when flag bit 4 is set,
 * else -1) matches the actor's +0x414 slot, the +0x3e4 transform is rebuilt from the actor's +0xa0
 * rotation at the current matrix's translation; a match on the +0x410 slot does the same for the
 * +0x3b8 transform. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct MtxFx43 { int m[9]; VecFx32 t; };

extern void NNS_G3dGetCurrentMtx(struct MtxFx43 *dst, void *src);
extern void SrtTransform_SetIdentity(void *transform);
extern void Srt_SetRotationQuat(void *transform, void *quat);
extern void Srt_SetTranslation(void *transform, VecFx32 *pos);

void Ov260_ModelNodeCallback(char *node)
{
    char *actor = *(char **)(*(int *)(node + 4) + 0x2c);
    struct MtxFx43 mtx;

    if (*(u32 *)(actor + 0x414) == ((*(u32 *)(node + 8) & 0x10) ? *(u8 *)(node + 0xae) : 0xffffffff)) {
        NNS_G3dGetCurrentMtx(&mtx, 0);
        SrtTransform_SetIdentity(actor + 0x3e4);
        Srt_SetRotationQuat(actor + 0x3e4, actor + 0xa0);
        Srt_SetTranslation(actor + 0x3e4, &mtx.t);
        return;
    }
    if (*(u32 *)(actor + 0x410) != ((*(u32 *)(node + 8) & 0x10) ? *(u8 *)(node + 0xae) : 0xffffffff)) {
        return;
    }
    NNS_G3dGetCurrentMtx(&mtx, 0);
    SrtTransform_SetIdentity(actor + 0x3b8);
    Srt_SetRotationQuat(actor + 0x3b8, actor + 0xa0);
    Srt_SetTranslation(actor + 0x3b8, &mtx.t);
}
