/* Transforms the hit shape (sphere, capsule or box) into world space and refreshes its bounds. */

#include "nitro/fx_types.h"
#include "game/engine.h"

static inline fx32 FX_Mul(fx32 a, fx32 b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

typedef struct {
    VecFx32 offset;
    fx32 scale_x;
    fx32 scale_y;
    fx32 scale_z;
    unsigned char flags : 1;
} XformObj;

typedef struct {
    unsigned char sel : 4;
    unsigned char rest : 4;
} Sel;

extern void Obj_LocalToWorld(VecFx32 *out, XformObj *obj, VecFx32 *in);
extern void SphereToAABB(int *dst, int *src);
extern void ScaleVec3Fx12(int factor, VecFx32 *src, VecFx32 *dst);
extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *out);
extern void Capsule_GetBounds(int *dst, VecFx32 *src);
extern void OrientedBox_GetBounds(int *dst, VecFx32 *src);

void Ov107_HitShape_UpdateWorld(unsigned char *self)
{
    VecFx32 *work = (VecFx32 *)(self + 4);

    switch (((Sel *)self)->sel) {
    case 0:
        Obj_LocalToWorld((VecFx32 *)(self + 0x68), (XformObj *)(self + 0x10), (VecFx32 *)(self + 0x58));
        *(fx32 *)(self + 0x74) = FX_Mul(*(fx32 *)(self + 0x64), *(fx32 *)(self + 0x2c));
        *work = *(VecFx32 *)(self + 0x68);
        SphereToAABB((int *)(self + 0x3c), (int *)(self + 0x68));
        return;
    case 1: {
        fx32 half;

        Obj_LocalToWorld((VecFx32 *)(self + 0x78), (XformObj *)(self + 0x10), (VecFx32 *)(self + 0x58));
        Vec3TransformViaTempMtx((VecFx32 *)(self + 0x84), (XformObj *)(self + 0x10), (VecFx32 *)(self + 0x64));
        *(fx32 *)(self + 0x90) = FX_Mul(*(fx32 *)(self + 0x70), *(fx32 *)(self + 0x2c));
        *(fx32 *)(self + 0x94) = FX_Mul(*(fx32 *)(self + 0x74), *(fx32 *)(self + 0x2c));
        half = *(fx32 *)(self + 0x90) / 2;
        ScaleVec3Fx12(half, (VecFx32 *)(self + 0x84), work);
        VEC_Add((VecFx32 *)(self + 0x78), work, work);
        Capsule_GetBounds((int *)(self + 0x3c), (VecFx32 *)(self + 0x78));
        return;
    }
    case 2: {
        int i;
        VecFx32 *dstv;
        VecFx32 *srcv;

        Obj_LocalToWorld((VecFx32 *)(self + 0x94), (XformObj *)(self + 0x10), (VecFx32 *)(self + 0x58));
        i = 0;
        dstv = (VecFx32 *)(self + 0x64);
        srcv = (VecFx32 *)(self + 0xa0);
        for (; i < 3; i++) {
            Vec3TransformViaTempMtx(srcv, (XformObj *)(self + 0x10), dstv);
            ((fx32 *)(self + 0xc4))[i] = FX_Mul(((fx32 *)(self + 0x88))[i], *(fx32 *)(self + 0x2c));
            dstv++;
            srcv++;
        }
        *work = *(VecFx32 *)(self + 0x94);
        OrientedBox_GetBounds((int *)(self + 0x3c), (VecFx32 *)(self + 0x94));
        return;
    }
    default:
        return;
    }
}
