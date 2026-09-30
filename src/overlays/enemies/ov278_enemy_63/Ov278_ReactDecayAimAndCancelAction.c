
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void ScaleVec3Fx12(int scale, VecFx32 *dst, VecFx32 *src);
extern int SetIndexedSlot(int self, int idx, void *handler);
extern void Ov278_ReactDecayAimAndSetState(int self);

/* Reaction: kill vertical drift (Y=0), decay the aim vector to 3/4 scale, copy it into the
 * active velocity slot, then (unless the sub-target is locked) cancel action 0xf and re-arm the
 * follow-up reaction 020d1180. */
void Ov278_ReactDecayAimAndCancelAction(int self) {
    int *node = *(int **)(self + 4);
    node[0x10] = 0;
    ScaleVec3Fx12(0xf00, (VecFx32 *)(node + 0xf), (VecFx32 *)(node + 0xf));
    *(VecFx32 *)(node + 6) = *(VecFx32 *)(node + 0xf);
    if (*(unsigned char *)(node[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*node), 0xf, 0);
    SetIndexedSlot(self, *(signed char *)(self + 0x20), &Ov278_ReactDecayAimAndSetState);
}
