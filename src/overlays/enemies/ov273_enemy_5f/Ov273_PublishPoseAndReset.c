/* Save the current pose vector from (child)+8 to (*child)+0xf0, then reset (child)+8 from
 * the const offset vector. */

#include "nitro/fx_types.h"

extern VecFx32 data_02041dc8;
void Ov273_PublishPoseAndReset(int param_1) {
    int child = *(int *)(param_1 + 4);
    VecFx32 *p = (VecFx32 *)(child + 8);
    *(VecFx32 *)(*(int *)child + 0xf0) = *p;
    *p = data_02041dc8;
}
