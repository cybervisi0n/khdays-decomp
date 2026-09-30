
/* Stores the caller's vector into the slot and records the mode; mode 0 also clears the owner's
 * pending-target field. */

#include "nitro/fx_types.h"

int Ov218_StoreTargetVector(char *self, int mode, char *src) {
    char *slot = *(char **)(self + 0x214);
    if (mode == 0) {
        *(int *)(*(int *)slot + 0x394) = 0;
    }
    *(VecFx32 *)(slot + 0x1c) = *(VecFx32 *)(src + 4);
    *(int *)(slot + 0xc) = mode;
    return 1;
}
