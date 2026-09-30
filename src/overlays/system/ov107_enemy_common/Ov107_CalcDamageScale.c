/* Damage scale from the attacker and defender stats (defense floor of 1.0). */

#include "nitro/fx_types.h"
#include "game/actor.h"

static inline fx32 FX_Mul(fx32 a, fx32 b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

extern int FX_Div(int a, int b);

int Ov107_CalcDamageScale(Actor *obj, fx32 p1, fx32 p2)
{
    fx32 sum = FX_Mul(obj->field_370[2], p2) - FX_Mul(obj->field_370[1], p1) + (obj->field_370[0] << 12);
    if (sum < 0x1000) sum = 0x1000;
    return FX_Div(FX_Mul(obj->field_370[3], p1) + (obj->field_370[4] << 12), sum);
}
