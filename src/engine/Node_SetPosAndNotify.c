/* Stores the position, marks it dirty and calls the node's change hook. */

#include "nitro/fx_types.h"

typedef void (*func_020293fc_cb)(void *ptr);

void Node_SetPosAndNotify(unsigned char *ptr, const VecFx32 *src) {
    func_020293fc_cb cb;

    *(VecFx32 *)(ptr + 0x2c) = *src;
    ptr[0x20] |= 1;

    cb = *(func_020293fc_cb *)*(int *)(ptr + 0x1c);
    if (cb != 0) {
        cb(ptr);
    }
}
