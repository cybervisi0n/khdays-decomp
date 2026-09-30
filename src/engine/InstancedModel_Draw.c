/* Draws the model once per live instance with its scale, orientation, position, alpha and polygon
 * id. */

#include "nitro/fx_types.h"

typedef struct {
    fx32 w, x, y, z;
} Quat_0203c304;

typedef struct {
    int marker;                 /* 0x00 */
    int field_04;                /* 0x04 */
    Quat_0203c304 orientation;   /* 0x08 */
    char pad18[0x2c - 0x18];
    VecFx32 position;             /* 0x2c */
} Entry_0203c304;

typedef struct {
    char pad00[0x78];
    void *model;                  /* 0x78 */
} ModelHolder_0203c304;

typedef struct {
    char pad00[0x4c];
    VecFx32 baseVector;           /* 0x4c */
    char pad58[0x88 - 0x58];
    ModelHolder_0203c304 *modelHolder; /* 0x88 */
    int count;                    /* 0x8c */
    Entry_0203c304 *entries;      /* 0x90 */
} Obj_0203c304;

extern void ScaleVec3Fx12(int factor, int *src, int *dst);
extern void Mtx33_FromQuat(int *mtx, const Quat_0203c304 *q);
extern void NNS_G3dGlbSetBaseTrans(const VecFx32 *target);
extern void Gfx_ApplyBaseTransform(void);
extern void NNS_G3dMdlSetMdlCullMode(int obj, unsigned int idx, unsigned int val);
extern void NNS_G3dMdlSetMdlAlpha(int obj, unsigned int idx, unsigned int val);
extern void NNS_G3dMdlSetMdlPolygonIDAll(unsigned char *ptr, int arg);
extern void NNS_G3dDraw1Mat1Shp(void *model, unsigned int materialId, unsigned int shapeId, int sendMaterial);
extern int data_02047458[3];
extern int data_02047428[9];

void InstancedModel_Draw(Obj_0203c304 *this) {
    int i;
    for (i = 0; i < this->count; i++) {
        Entry_0203c304 *entry = &this->entries[i];
        if (entry->marker != 0) {
            ScaleVec3Fx12(entry->marker, (int *)&this->baseVector, data_02047458);
            Mtx33_FromQuat(data_02047428, &entry->orientation);
            NNS_G3dGlbSetBaseTrans(&entry->position);
            Gfx_ApplyBaseTransform();
            NNS_G3dMdlSetMdlCullMode((int)this->modelHolder->model, 0, 3);
            NNS_G3dMdlSetMdlAlpha((int)this->modelHolder->model, 0, (entry->field_04 * 31) >> 12);
            NNS_G3dMdlSetMdlPolygonIDAll((unsigned char *)this->modelHolder->model, i % 0x3f);
            NNS_G3dDraw1Mat1Shp(this->modelHolder->model, 0, 0, 1);
        }
    }
}
