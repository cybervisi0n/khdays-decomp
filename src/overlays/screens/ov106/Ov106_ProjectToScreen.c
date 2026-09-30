/* Project `point` to the ov106 screen through `cam`: its look-at view (eye +0x20, up +0x2c, target
 * +0x14) is concatenated with the scene's +0x8b84 projection and the result is scaled to 12.8 x 9.6
 * screen units into `out`. */

#include "nitro/fx_types.h"

typedef struct { int m[12]; } MtxFx43;
typedef struct { int m[16]; } MtxFx44;

extern char *data_ov106_020b8b60;
extern void MTX_LookAt(const VecFx32 *pos, const VecFx32 *up, const VecFx32 *target, MtxFx43 *viewOut);
extern void MTX_Copy43To44_(const MtxFx43 *src, MtxFx44 *dst);
extern void MTX_Concat44(const MtxFx44 *a, const void *b, MtxFx44 *out);
extern void MTX_Copy44To43_(const MtxFx44 *pSrc, MtxFx43 *pDst);
extern void MTX_MultVec43(const VecFx32 *pVec, const MtxFx43 *pMtx, VecFx32 *pOut);

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

void Ov106_ProjectToScreen(VecFx32 *out, const VecFx32 *point, char *cam)
{
    MtxFx43 view;
    MtxFx43 m43;
    MtxFx44 m;

    MTX_LookAt((VecFx32 *)(cam + 0x20), (VecFx32 *)(cam + 0x2c), (VecFx32 *)(cam + 0x14), &view);
    MTX_Copy43To44_(&view, &m);
    MTX_Concat44(&m, data_ov106_020b8b60 + 0x8b84, &m);
    MTX_Copy44To43_(&m, &m43);
    MTX_MultVec43(point, &m43, out);
    out->x = FX_MUL(out->x, 0xcccd);
    out->y = FX_MUL(out->y, 0x999a);
}
