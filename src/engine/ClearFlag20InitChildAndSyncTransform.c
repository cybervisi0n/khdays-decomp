/* If arg1 is 0 returns 1; else clears bit 0x20 of *this, calls RegisterSeqAndInit(this+4, arg1,
 * arg2, arg3), and if bit 8 of *this is set copies the 3-word vec at this+0x13c into this+0xa8;
 * returns 1. */

#include "nitro/fx_types.h"

extern void RegisterSeqAndInit();

int ClearFlag20InitChildAndSyncTransform(int this_, int arg1, int arg2, int arg3) {
    if (arg1 != 0) {
        *(int *)this_ &= ~0x20;
        RegisterSeqAndInit(this_ + 4, arg1, arg2, arg3);
        if (*(int *)this_ & 8) {
            *(VecFx32 *)(this_ + 0xa8) = *(VecFx32 *)(this_ + 0x13c);
        }
    }
    return 1;
}
