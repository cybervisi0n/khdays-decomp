/* Quadratic curve through three keys: for t in [0, 1] the parabola through p0 (t = 0), p1 (t = 1)
 * and p2 (t = 2) evaluated at t, for t beyond 1 the one through p1 at t = 1 continued with the p0..p2
 * slope and curvature (all fx32, FX_Mul = FX_Mul). Codegen: the curvature term of the second
 * branch is written inline in the final sum; as a named local the last add swaps its operands. */
#pragma thumb on

#include "nitro/fx_types.h"

extern fx32 FX_Mul(fx32 a, fx32 b);   /* FX_Mul */

fx32 Anim_InterpEased(fx32 t, fx32 p0, fx32 p1, fx32 p2)
{
    if (t <= 0x1000) {
        fx32 a = FX_Mul(-3 * p0 + p1 * 4 - p2, t);
        fx32 b = FX_Mul(p2 + (p0 - (p1 << 1)), FX_Mul(t, t));

        return FX_Mul(0x800, p0 + p0 + a + b);
    } else {
        fx32 two = p1 << 1;
        fx32 c = FX_Mul(p2 - p0, t - 0x1000);

        return FX_Mul(0x800, two + c + FX_Mul(p2 + (p0 - two), FX_Mul(t - 0x1000, t - 0x1000)));
    }
}
