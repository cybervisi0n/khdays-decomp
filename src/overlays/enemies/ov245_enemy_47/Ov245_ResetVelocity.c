/* Ov245_ResetVelocity -- reset the velocity vector at +0x3a0 to the shared zero constant, and if
 * the object is in mode 1 hand its context (+0x214) to the follow-up. */

#include "nitro/fx_types.h"

extern void Ov245_SetPendingState2(int a);
extern int data_02041dc8;

void Ov245_ResetVelocity(int self) {
    *(VecFx32 *)(self + 0x3a0) = *(VecFx32 *)&data_02041dc8;
    if (*(int *)(self + 0x50) != 1) {
        return;
    }
    Ov245_SetPendingState2(*(int *)(self + 0x214));
}
