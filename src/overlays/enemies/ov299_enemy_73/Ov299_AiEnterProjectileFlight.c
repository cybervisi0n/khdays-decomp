/* Resets the flight state from the launch point, sets the part flag and installs the projectile
 * flight tick. */

#include "nitro/fx_types.h"

struct hw60 { unsigned short lo : 8, hi : 8; };
struct b8 { unsigned int f : 8; };

extern void SetIndexedSlot(int self, int index, void *cb);
extern void Ov299_ProjectileFlightTick(void);

void Ov299_AiEnterProjectileFlight(int self) {
    int *obj = *(int **)(self + 4);
    obj[9] = 0;
    *(VecFx32 *)((char *)obj + 0x18) = *(VecFx32 *)obj[1];
    obj[12] = 0;
    ((struct hw60 *)(*obj + 0x60))->hi &= ~0x80;
    ((struct b8 *)(*(int *)(*obj + 0x388) + 8))->f |= 1;
    SetIndexedSlot(self, *(signed char *)((char *)self + 0x20), (void *)&Ov299_ProjectileFlightTick);
}
