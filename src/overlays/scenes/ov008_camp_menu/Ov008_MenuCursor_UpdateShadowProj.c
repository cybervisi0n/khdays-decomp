/* Ov008_MenuCursor_UpdateShadowProj -- recompute the ov008 cursor's derived positions, ov008.
 * Copies the live cursor (obj+0x18/1c/20) into the shadow slot (obj+0x24/28/2c) with fixed
 * y/z biases (+0x1000, +0x28000), then stores at obj+0x424..: the quotients
 * FX_Div(x, 0x14cd) and FX_Div(y, 0x14cd) of the shadow x and y, and the shadow z minus 0x64000. */

#include "nitro/fx_types.h"

extern int FX_Div(int x, int k);

void Ov008_MenuCursor_UpdateShadowProj(int obj) {
    VecFx32 proj;
    *(int *)(obj + 0x24) = *(int *)(obj + 0x18);
    *(int *)(obj + 0x28) = *(int *)(obj + 0x1c) + 0x1000;
    *(int *)(obj + 0x2c) = *(int *)(obj + 0x20) + 0x28000;
    proj.x = FX_Div(*(int *)(obj + 0x24), 0x14cd);
    proj.y = FX_Div(*(int *)(obj + 0x28), 0x14cd);
    proj.z = *(int *)(obj + 0x2c) - 0x64000;
    *(VecFx32 *)(obj + 0x424) = proj;
}
