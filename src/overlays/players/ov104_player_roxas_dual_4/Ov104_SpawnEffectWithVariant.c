/* Spawns the landing effect at the point through a placement message to the battle module, using
 * the stronger variant when flagged. */

#include "nitro/fx_types.h"

extern void Ov022_SendPlacementMessage(int self, void *p);

typedef struct {
    VecFx32 vec;
    short f0c, f0e, f10, f12;
    int f14, f18, f1c, f20, f24, f28;
} Params;

void Ov104_SpawnEffectWithVariant(int self, int *ctx, VecFx32 *src) {
    Params p;
    p.vec = *src;
    p.f14 = 0;
    p.f1c = 1;
    p.f20 = 0;
    p.f0c = 0;
    p.f0e = 0x1000;
    p.f10 = 0;
    p.f28 = 0;
    p.f24 = 0;
    p.f18 = 0;
    p.f12 = 0x2100;
    if (ctx[0x46] != 0) {
        p.f28 = 1;
        p.f24 = 1;
        p.f12 = 0x2480;
        p.f18 = 1;
    }
    Ov022_SendPlacementMessage(self, &p);
}
