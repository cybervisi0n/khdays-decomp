/* Store a 3-word vector (vec) into obj+0x15c0 and a 2-word pair (a,b) into obj+0x15d8. */

#include "nitro/fx_types.h"

void Ov023_StoreEntityTransform(int obj, void *vec, int a, int b) {
    *(VecFx32 *)(obj + 0x15c0) = *(VecFx32 *)vec;
    *(int *)(obj + 0x15d8) = a;
    *(int *)(obj + 0x15dc) = b;
}
