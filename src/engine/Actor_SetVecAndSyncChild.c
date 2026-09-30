/* Sets the actor's position (+0xa8), first notifying its node of the move unless the actor is
 * detached (flag 0x10). */

#include "nitro/fx_types.h"

extern void Node_SetPosAndNotify(void *ptr, const VecFx32 *src);

void Actor_SetVecAndSyncChild(void *pPtr, const VecFx32 *src) {
    int *ptr = (int *)pPtr;
    if ((ptr[0] & 0x10) == 0) {
        Node_SetPosAndNotify((char *)ptr + 0x110, src);
    }

    *(VecFx32 *)((char *)ptr + 0xa8) = *src;
}
