/* AI step: computes the velocity from the action resource and heading and, when the action ends,
 * records it and continues. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern int Ov107_ActionResource_GetOffsetAndScale(void *src, void *out);
extern void ScaleVec3Fx12(int scale, void *src, void *dst);
extern void SetIndexedSlot(void *node, int idx, void *next);
extern void Ov293_CopyVecThenSetupSubActionAndAdvance(void);

void Ov293_TransformScaleVecCopyThenAdvance(void *node)
{
    int vec[3];
    int *state;
    int scale;

    state = *(int **)((char *)node + 4);
    scale = Ov107_ActionResource_GetOffsetAndScale(*(void **)(state[0] + 0x39c), vec);
    Vec3TransformViaTempMtx(state + 7, (void *)(state[0] + 0xa0), vec);
    ScaleVec3Fx12(scale, state + 7, state + 7);

    if (**(unsigned char **)((char *)state + 0x4c) != 0) {
        return;
    }

    *(VecFx32 *)((char *)state + 0x28) = *(VecFx32 *)((char *)state + 0x1c);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov293_CopyVecThenSetupSubActionAndAdvance);
}
