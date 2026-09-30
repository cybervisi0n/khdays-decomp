/* Returns an action resource's scale (+0x38), copying its offset out when asked. */

#include "nitro/fx_types.h"

/* Optionally copy +0x14 vector out; return +0x38. */
int Ov107_ActionResource_GetOffsetAndScale(int res, VecFx32 *pOffset) {
    if (pOffset != 0) *pOffset = *(VecFx32 *)(res + 0x14);
    return *(int *)(res + 0x38);
}
