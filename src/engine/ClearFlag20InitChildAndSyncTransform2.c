/* Binds an animation sequence to the node: clears flag 0x20, registers the sequence on the child
 * and, when flag 8 is set, restores the saved position; always returns 1. */

#include "nitro/fx_types.h"
#include "game/engine.h"

int ClearFlag20InitChildAndSyncTransform2(int this_, int arg1, int arg2, int arg3) {
    if (arg1 != 0) {
        *(int *)this_ &= ~0x20;
        StoreField74ThenForward(this_ + 4, arg1, arg2, arg3);
        if (*(int *)this_ & 8) {
            *(VecFx32 *)(this_ + 0xa8) = *(VecFx32 *)(this_ + 0x13c);
        }
    }
    return 1;
}
