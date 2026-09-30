/* Copy the pose vector from (child)+0xc to (*child)+0xf0, then update the transform node
 * at (*child)+0xa0 from (child)+0x30. */

#include "nitro/fx_types.h"

extern void Srt_SetRotationQuat(int a, int b);
void Ov208_Item_ApplyOrientation(int param_1) {
    int child = *(int *)(param_1 + 4);
    *(VecFx32 *)(*(int *)child + 0xf0) = *(VecFx32 *)(child + 0xc);
    Srt_SetRotationQuat(*(int *)child + 0xa0, child + 0x30);
}
