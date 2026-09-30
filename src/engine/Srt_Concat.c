/* Srt_Concat -- concatenate two SRT transforms, MAIN: *out = parent * child. A transform is a
 * quaternion (+0x00), a translation (+0x10), a scale (+0x1c) and flags (+0x28: bit 0 identity,
 * bit 1 uniform scale, only scale.x used). An identity side just copies the other. A uniformly
 * scaled parent composes directly: rotations multiply (Quat_Multiply), the child's translation is
 * rotated by the parent (Vec3TransformViaTempMtx), scaled and offset, and the scales multiply. Otherwise
 * both sides become scaled rotation matrices, the product sets the rotation/scale (Node_SetRotationFromMtx)
 * and the child's translation goes through the parent matrix. */

#include "nitro/fx_types.h"

typedef struct { fx32 w, x, y, z; } Quat;
typedef struct { fx32 m[9]; } MtxFx33;

typedef struct SrtFlags {
    unsigned char identity : 1;
    unsigned char uniformScale : 1;
} SrtFlags;

typedef struct Srt {
    Quat rot;                           /* +0x00 */
    VecFx32 trans;                      /* +0x10 */
    VecFx32 scale;                      /* +0x1c */
    SrtFlags flags;                     /* +0x28 */
} Srt;

extern void Quat_Multiply(Srt *out, const Srt *a, const Srt *b);          /* Quat_Mul */
extern void Vec3TransformViaTempMtx(VecFx32 *out, const Srt *rot, const VecFx32 *in);  /* Quat_RotateVec */
extern void ScaleVec3Fx12(fx32 scale, const VecFx32 *src, VecFx32 *dst);  /* VEC_Scale */
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *ab);
extern void Srt_SetTranslation(Srt *srt, const VecFx32 *trans);                /* Srt_SetTrans */
extern void Srt_SetScaleUniform(Srt *srt, fx32 scale);                          /* Srt_SetUniformScale */
extern void Srt_SetScaleVec(Srt *srt, const VecFx32 *scale);                /* Srt_SetScale */
extern void Mtx33_FromQuat(MtxFx33 *mtx, const Srt *rot);                  /* Quat_ToMtx33 */
extern void Mtx33_ScaleColumns(MtxFx33 *dst, const MtxFx33 *mtx, const VecFx32 *scale);
extern void MTX_Concat33(const MtxFx33 *a, const MtxFx33 *b, MtxFx33 *ab);
extern void Node_SetRotationFromMtx(Srt *srt, const MtxFx33 *mtx);                  /* Srt_SetFromMtx33 */
extern void MTX_MultVec33(const VecFx32 *vec, const MtxFx33 *m, VecFx32 *dst);

#define FX_MUL(a, b) ((fx32)(((long long)(a) * (b) + 0x800) >> 12))

void Srt_Concat(Srt *out, const Srt *parent, const Srt *child)
{
    VecFx32 v;
    MtxFx33 parentMtx;
    MtxFx33 childMtx;
    MtxFx33 prod;

    if (parent->flags.identity) {
        *out = *child;
        return;
    }
    if (child->flags.identity) {
        *out = *parent;
        return;
    }
    if (parent->flags.uniformScale) {
        Quat_Multiply(out, parent, child);
        Vec3TransformViaTempMtx(&v, parent, &child->trans);
        ScaleVec3Fx12(parent->scale.x, &v, &v);
        VEC_Add(&v, &parent->trans, &v);
        Srt_SetTranslation(out, &v);
        if (child->flags.uniformScale) {
            Srt_SetScaleUniform(out, FX_MUL(parent->scale.x, child->scale.x));
        } else {
            ScaleVec3Fx12(parent->scale.x, &child->scale, &v);
            Srt_SetScaleVec(out, &v);
        }
        return;
    }
    Mtx33_FromQuat(&parentMtx, parent);
    Mtx33_ScaleColumns(&parentMtx, &parentMtx, &parent->scale);
    Mtx33_FromQuat(&childMtx, child);
    Mtx33_ScaleColumns(&childMtx, &childMtx, &child->scale);
    MTX_Concat33(&parentMtx, &childMtx, &prod);
    Node_SetRotationFromMtx(out, &prod);
    MTX_MultVec33(&child->trans, &parentMtx, &v);
    VEC_Add(&v, &parent->trans, &v);
    Srt_SetTranslation(out, &v);
}
