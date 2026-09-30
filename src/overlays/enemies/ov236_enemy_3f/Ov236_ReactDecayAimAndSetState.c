
#include "nitro/fx_types.h"

extern void ScaleVec3Fx12(int scale, VecFx32 *dst, VecFx32 *src);
extern int SetIndexedSlot(int self, int idx, void *handler);

/* Follow-up reaction: same aim-vector decay as 020d1114, then (unless locked) clear the retry
 * counter, set the actor's reaction state to 0xa, and re-arm the slot with no handler. */
void Ov236_ReactDecayAimAndSetState(int self) {
    int *node = *(int **)(self + 4);
    node[0x10] = 0;
    ScaleVec3Fx12(0xf00, (VecFx32 *)(node + 0xf), (VecFx32 *)(node + 0xf));
    *(VecFx32 *)(node + 6) = *(VecFx32 *)(node + 0xf);
    if (*(unsigned char *)(node[1] + 0xad) != 0) {
        return;
    }
    node[0x15] = 0;
    *(unsigned char *)(*node + 0x1c7) = 0xa;
    SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
}
