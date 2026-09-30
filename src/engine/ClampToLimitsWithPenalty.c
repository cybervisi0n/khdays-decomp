
#include "nitro/fx/fx.h"


/* Clamp each axis of *pos against a limit vector at bounds+0x30, accumulating
 * a squared-overshoot penalty into *penalty for any axis pushed past its
 * limit. */
void ClampToLimitsWithPenalty(VecFx32 *pos, unsigned char *bounds, fx32 *penalty)
{
    fx32 *limit = (fx32 *)(bounds + 0x30);

    if (pos->x < -limit[0]) {
        fx32 over = pos->x + limit[0];
        *penalty += FX_Mul(over, over);
        pos->x = -limit[0];
    } else if (pos->x > limit[0]) {
        fx32 over = pos->x - limit[0];
        *penalty += FX_Mul(over, over);
        pos->x = limit[0];
    }

    if (pos->y < -limit[1]) {
        fx32 over = pos->y + limit[1];
        *penalty += FX_Mul(over, over);
        pos->y = -limit[1];
    } else if (pos->y > limit[1]) {
        fx32 over = pos->y - limit[1];
        *penalty += FX_Mul(over, over);
        pos->y = limit[1];
    }

    if (pos->z < -limit[2]) {
        fx32 over = pos->z + limit[2];
        *penalty += FX_Mul(over, over);
        pos->z = -limit[2];
    } else if (pos->z > limit[2]) {
        fx32 over = pos->z - limit[2];
        *penalty += FX_Mul(over, over);
        pos->z = limit[2];
    }
}
