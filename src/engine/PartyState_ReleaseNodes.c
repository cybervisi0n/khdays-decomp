/* Releases the node of each of the two party records. */

#include "game/engine.h"

extern char data_0204c500;

void PartyState_ReleaseNodes(void) {
    #ifdef SDK_BUILD_ARM
    //TODO
    register int index asm("r4") = 0;
    register char *ptr asm("r5") = &data_0204c500;
    register int zero asm("r6") = index;

    for (; index < 2; index++) {
        if (*(int *)(ptr + 0x44) != 0) {
            DispatchByNodeKind(ptr + 0x44);
            *(int *)(ptr + 0x44) = zero;
        }
        ptr += 0x48;
    }
    #endif
}
