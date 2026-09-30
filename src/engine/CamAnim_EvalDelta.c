/* Evaluates the animation and returns how far the camera moved (position, target, up, fovy) since
 * the last frame. */

#include "nitro/fx_types.h"

extern void MI_CpuCopy8(const void *src, void *dst, unsigned int size);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *ab);
extern void DispatchWithReentrantScratch(void *a, void *b);

void CamAnim_EvalDelta(char *a, char *b, void *c)
{
    char tmp[0x38];

    MI_CpuCopy8(a + 0x10, tmp, 0x38);
    DispatchWithReentrantScratch(a, c);
    VEC_Subtract((const VecFx32 *)(a + 0x30), (const VecFx32 *)(tmp + 0x20), (VecFx32 *)(b + 0x20));
    VEC_Subtract((const VecFx32 *)(a + 0x24), (const VecFx32 *)(tmp + 0x14), (VecFx32 *)(b + 0x14));
    VEC_Subtract((const VecFx32 *)(a + 0x3c), (const VecFx32 *)(tmp + 0x2c), (VecFx32 *)(b + 0x2c));
    *(int *)b = *(int *)(a + 0x10) - *(int *)tmp;
    *(int *)(b + 4) = *(int *)(a + 0x14) - *(int *)(tmp + 4);
}
