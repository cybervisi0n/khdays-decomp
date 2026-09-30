#include "nitro/fx_types.h"

extern int ScriptVm_ReadOperandInt(int ctx, void *arg);
extern int ByteCode_ResolveOperand(int ctx, void *arg);
extern int ScriptVm_ResolveActorIndex(int ctx, int arg);
extern char *ArrayEntryPtrD0(int index);

extern void NNS_G3dMdlSetMdlAlphaAll(int anim, int a);
extern void NNS_G3dMdlSetMdlPolygonIDAll(int anim, int a);

/* Script command: zeroes the Y component of the entity node's offset vector and restarts its
 * animation. */
int Ov023_Cmd_ClearEntityOffsetY(int ctx, int args) {
    char *node = ArrayEntryPtrD0((unsigned short)ScriptVm_ResolveActorIndex(ctx, ScriptVm_ReadOperandInt(ctx, (void *)args)));
    VecFx32 v;
    v.x = *(int *)(node + 0xb4);
    v.y = 0;
    v.z = *(int *)(node + 0xbc);
    *(VecFx32 *)(node + 0xb4) = v;
    NNS_G3dMdlSetMdlAlphaAll(*(int *)(node + 0x7c), 8);
    NNS_G3dMdlSetMdlPolygonIDAll(*(int *)(node + 0x7c), 0x3f);
    return 1;
}
