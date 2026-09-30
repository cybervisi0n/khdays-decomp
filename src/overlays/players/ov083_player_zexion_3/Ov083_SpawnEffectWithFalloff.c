/* Spawns the effect at the point with a speed that grows with the effect's timer (capped), using
 * the stronger variant when flagged, through a placement message to the battle module. */

#include "nitro/fx_types.h"

extern void Ov022_SendPlacementMessage(int self, void *p);

typedef struct {
    VecFx32 vec;
    short f0c, f0e, f10, f12;
    int f14, f18, f1c, f20, f24, f28;
} Params;

void Ov083_SpawnEffectWithFalloff(int self, int *node, VecFx32 *src) {
    Params p;
    int t = 0x800;
    t += node[0x43] * 0xcd;
    if (t > 0x1800) t = 0x1800;
    p.vec = *src;
    p.f14 = 0;
    p.f1c = 1;
    p.f20 = 0;
    p.f18 = 7;
    p.f24 = 0;
    p.f28 = 0;
    p.f12 = 0x1a00;
    p.f0c = 0;
    p.f10 = 0;
    p.f0e = -t;
    if (node[0x45] != 0) {
        p.f24 = 1;
        p.f12 = 0x1e00;
        p.f28 = 1;
    }
    Ov022_SendPlacementMessage(self, &p);
}
