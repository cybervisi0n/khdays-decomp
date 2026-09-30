/* Transforms the offset through the owner's matrix and scales it. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern int Ov107_ActionResource_GetOffsetAndScale(void *src, void *out);
extern void ScaleVec3Fx12(int scale, void *src, void *dst);
extern void SetIndexedSlot(void *node, int idx, void *next);
extern void Ov121_CopyVecThenSetupSubActionAndAdvance(void);

void Ov121_AiTrackOffsetTick(void *node)
{
    int vec[3];
    int *state;
    int scale;

    state = *(int **)((char *)node + 4);
    scale = Ov107_ActionResource_GetOffsetAndScale(*(void **)(state[0] + 0x3a0), vec);
    Vec3TransformViaTempMtx(state + 7, (void *)(state[0] + 0xa0), vec);
    ScaleVec3Fx12(scale, state + 7, state + 7);

    if (**(unsigned char **)((char *)state + 0x48) != 0) {
        return;
    }

    *(VecFx32 *)((char *)state + 0x28) = *(VecFx32 *)((char *)state + 0x1c);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov121_CopyVecThenSetupSubActionAndAdvance);
}
