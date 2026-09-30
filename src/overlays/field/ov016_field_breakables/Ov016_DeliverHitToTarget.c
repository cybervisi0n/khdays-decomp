#include "nitro/fx_types.h"

extern int Ov002_GetCtxTableByte(int slot);

typedef struct {
    int x;
    int y;
    int z;
    int strength;
    void *source;
} Ov016Hit;

/* Hands a hit descriptor to the target's damage handler, but only when both belong to the same
 * model and the target is in state 2. */
int Ov016_DeliverHitToTarget(char *self, char *target) {
    Ov016Hit hit;
    if (*(unsigned short *)(*(char **)(target + 8) + 0x4c) != 2) {
        return 0;
    }
    if (Ov002_GetCtxTableByte((unsigned char)target[0x10])
        != Ov002_GetCtxTableByte((unsigned char)self[0x10])) {
        return 0;
    }
    *(VecFx32 *)&hit = *(VecFx32 *)(self + 0x1c);
    hit.strength = *(int *)(self + 0x28);
    hit.source = self;
    return (*(int (**)(void *, int, void *))(*(char **)(target + 8) + 0x44))(
               target, 0, &hit) != 0;
}
