
#include "nitro/fx_types.h"
#include "game/engine.h"

extern void VEC_Add(VecFx32 *dst, VecFx32 *a, VecFx32 *b);
extern void VEC_Subtract(VecFx32 *a, VecFx32 *b, VecFx32 *out);
extern void VEC_Normalize(VecFx32 *dst, VecFx32 *src);
extern void MTX_RotY33_(int *mtx, int a, int b);
extern void MTX_MultVec33(VecFx32 *a, int *mtx, VecFx32 *out);
extern int FX_Div(int a, int b);
extern void ScaleVec3Fx12(int scale, VecFx32 *src, VecFx32 *dst);
extern void VEC_MultAdd(int t, VecFx32 *a, VecFx32 *b, VecFx32 *out);

/* Approach-solver: advance the approach timer (+4 += param_4), take the target
 * offset, blend the current facing toward it with a rotation-limited step whose
 * magnitude eases in over the first 0x6000 of the timer, then scale by the entity's
 * base speed (1.5x in the alt mode) and write the resulting velocity to param_1. */
void Ov088_ComputeApproachVelocity(VecFx32 *param_1, int param_2, int param_3, int param_4) {
    struct { int mtx[9]; VecFx32 v24; VecFx32 v30; VecFx32 v3c; } f;
    int *iVar1 = *(int **)(param_3 + 0x138);
    int t;

    *(int *)(param_3 + 4) += param_4;
    f.v3c = *(VecFx32 *)(param_3 + 0xcc);
    VEC_Add(&f.v3c, (VecFx32 *)(param_3 + 0x1c), &f.v3c);
    VEC_Subtract(&f.v3c, (VecFx32 *)(param_3 + 0x10), &f.v24);
    VEC_Normalize(&f.v24, &f.v24);
    MTX_RotY33_(f.mtx, 0x1000, 0);
    MTX_MultVec33(&f.v24, f.mtx, &f.v30);
    if (*(int *)(param_3 + 4) < 0x6000) {
        int r = FX_Div((*(int *)(param_3 + 4) / 3) * 3, 0x6000);
        t = (r / 8) + 0x1000;
    } else {
        t = 0x1000;
    }
    ScaleVec3Fx12(t, &f.v30, &f.v30);
    VEC_MultAdd(0x1000 - t, &f.v24, &f.v30, &f.v24);
    VEC_Normalize(&f.v24, &f.v24);
    {
        int mode = GetFrameRateMode();
        int sc;
        if (mode == 1) {
            sc = (iVar1[4] * 3) / 2;
        } else {
            sc = iVar1[4];
        }
        ScaleVec3Fx12(sc, &f.v24, &f.v24);
    }
    *param_1 = f.v24;
}
