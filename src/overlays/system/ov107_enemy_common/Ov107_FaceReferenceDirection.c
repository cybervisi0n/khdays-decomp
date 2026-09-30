/* Point the actor at the fixed reference direction: take the difference between
 * the two constant vectors into self+0x7c, normalise it in place, copy the
 * second constant straight into self+0x88, then hand off to
 * Ov107_Region_SyncChildVisibility. */

#include "nitro/fx_types.h"

extern VecFx32 data_020475c4;
extern VecFx32 data_020475ac;

extern void VEC_Subtract(VecFx32 *a, VecFx32 *b, VecFx32 *dst);
extern void VEC_Normalize(VecFx32 *src, VecFx32 *dst);
extern void Ov107_Region_SyncChildVisibility(void *self, int arg);

void Ov107_FaceReferenceDirection(char *self, int arg) {
    VEC_Subtract(&data_020475c4, &data_020475ac, (VecFx32 *)(self + 0x7c));
    VEC_Normalize((VecFx32 *)(self + 0x7c), (VecFx32 *)(self + 0x7c));
    *(VecFx32 *)(self + 0x88) = data_020475ac;
    Ov107_Region_SyncChildVisibility(self, arg);
}
