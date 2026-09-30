/* Allocates the 0x20-byte animation player from the default heap allocator. Returns the new player,
 * or NULL when the allocation fails. */

#include "nitro/os.h"
#include "nnsys/fnd/allocator.h"

extern NNSFndAllocator * NNSi_FndGetAllocatorForDefaultHeap();

void *CamAnim_AllocPlayer(void) {
    return NNS_FndAllocFromAllocator(NNSi_FndGetAllocatorForDefaultHeap(0), 0x20);
}
