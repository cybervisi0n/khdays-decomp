/* Hit reaction of an ov256 helper: its owner's +0x398 part is knocked back at the +8 point (mode 4),
 * the owner's next move clears and the hit is taken (1). */

#include "nitro/fx_types.h"

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);

int Ov256_HelperHitReaction(char *self)
{
    int *state = *(int **)(self + 0x214);

    func_ov107_020c0b90(*(int *)(*state + 0x398), 4, *(VecFx32 *)state[2], 0);
    *(signed char *)(*state + 0x1c7) = 0;
    return 1;
}
