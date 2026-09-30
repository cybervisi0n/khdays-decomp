/* Apply the ov259 helper's +0xc step: the owner moves to its +0xb0 position plus the step
 * (020c5c54) and the step resets. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern const VecFx32 data_02041dc8;

void Ov259_ApplyHelperStep(int *node)
{
    int *state = (int *)node[1];
    VecFx32 at;

    VEC_Add((VecFx32 *)(*state + 0xb0), (VecFx32 *)(state + 3), &at);
    Ov107_MoveNodeAndRelayout((Actor *)(*state), &at);
    *(VecFx32 *)(state + 3) = data_02041dc8;
}
