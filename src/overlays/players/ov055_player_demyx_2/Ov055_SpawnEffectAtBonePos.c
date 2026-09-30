/* Spawns the attack effect at the character's weapon anchor through a placement message to the
 * battle module. */

#include "nitro/fx_types.h"

extern void func_ov022_020ad44c(void *out, int self);
extern void Ov022_SendPlacementMessage(int self, void *p);

typedef struct {
    VecFx32 pos;
    short f0c, f0e, f10, f12;
    int f14, f18, f1c, f20, f24, f28;
} Params;

void Ov055_SpawnEffectAtBonePos(int self) {
    Params p;
    VecFx32 v;
    func_ov022_020ad44c(&v, self);
    p.f10 = 0;
    p.f0e = 0;
    p.f0c = 0;
    p.pos = v;
    p.f14 = 2;
    p.f1c = 0;
    p.f20 = 0;
    p.f18 = 7;
    p.f24 = 0;
    p.f28 = 0;
    p.f12 = 0x1900;
    Ov022_SendPlacementMessage(self, &p);
}
