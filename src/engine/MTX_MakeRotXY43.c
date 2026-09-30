#pragma thumb on
/* MTX_MakeRotXY43 -- build a rotation matrix from X and Y angles, MAIN (THUMB). Starts from identity
 * and concatenates a rotation about X by `angleX`, then one about Y by `angleY` (16-bit angles, sine
 * and cosine from the SDK table). The angles arrive as ints and are cut to 16 bits on entry. */

#include "nitro/types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_mtx.h"
#include "nitro/fx/fx_trig.h"

void MTX_MakeRotXY43(MtxFx43 *m, int angleX, int angleY)
{
    MtxFx43 tmp;

    angleX = (u16)angleX;
    angleY = (u16)angleY;

    MTX_Identity43_(m);
    MTX_Identity43_(&tmp);
    MTX_RotX43_(&tmp, FX_SinIdx(angleX), FX_CosIdx(angleX));
    MTX_Concat43(m, &tmp, m);
    MTX_Identity43_(&tmp);
    MTX_RotY43_(&tmp, FX_SinIdx(angleY), FX_CosIdx(angleY));
    MTX_Concat43(m, &tmp, m);
}
#pragma thumb off
