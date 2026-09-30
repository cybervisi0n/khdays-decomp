/* Placement of one ov255 ring item (only while its +0x50 mode is 1): at the given angle about the
 * forward axis turned by q, the item's +0x390 point sits 0.5 out from the centre plus the forward
 * axis, its +0x3a8 orientation faces along the cross of the forward and outward axes, the angle is
 * kept at +0x3b8 and bit 0 of the +0x60 high byte is raised. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int w[4]; } Quat;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *a, const VecFx32 *b);
extern void VEC_Add(const void *a, const void *b, void *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern const VecFx32 data_02042258;
extern const short data_0203d210[];

void Ov255_PlaceRingItem(char *item, VecFx32 *at, void *q, int angle)
{
    VecFx32 fwd;
    VecFx32 side;
    VecFx32 up;
    u16 hw;

    if (*(int *)(item + 0x50) != 1) {
        return;
    }
    Vec3TransformViaTempMtx(&fwd, q, &data_02042258);
    side.x = data_0203d210[ANG2IDX(angle) * 2];
    side.y = data_0203d210[ANG2IDX(angle) * 2 + 1];
    side.z = 0;
    Vec3TransformViaTempMtx(&side, q, &side);
    VEC_Normalize(&side, &side);
    VEC_CrossProduct(&fwd, &side, &up);
    Quat_FromTwoVectors((Quat *)(item + 0x3a8), &data_02042258, &up);
    VEC_Add(at, &fwd, item + 0x390);
    ScaleVec3Fx12(0x800, &side, &side);
    VEC_Add(item + 0x390, &side, item + 0x390);
    *(int *)(item + 0x3b8) = angle;
    hw = *(u16 *)(item + 0x60);
    *(u16 *)(item + 0x60) = (hw & ~0xff00) |
        ((((((unsigned int)hw << 0x10) >> 0x18) | 1) << 0x18) >> 0x10);
}
