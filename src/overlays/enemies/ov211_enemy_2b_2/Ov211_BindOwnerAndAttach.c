/* Store the owner (param_2) at +4; if a spawn position (param_3) is given, copy it to
 * +0x20 and run the ov107 attach. */

#include "nitro/fx_types.h"

extern void Ov107_MoveNodeAndRelayout(int a, int b);
void Ov211_BindOwnerAndAttach(int param_1, int param_2, int param_3) {
    *(int *)(param_1 + 4) = param_2;
    if (param_3 == 0) return;
    *(VecFx32 *)(param_1 + 0x20) = *(VecFx32 *)param_3;
    Ov107_MoveNodeAndRelayout(*(int *)param_1, param_1 + 0x20);
}
