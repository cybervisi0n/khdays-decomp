
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_vec.h"

typedef struct Box {
    VecFx32 center;
    VecFx32 axis[3];
    fx32 halfExtent[3];
} Box;

extern void ScaleVec3Fx12(int factor, int *src, int *dst);

/* Squared distance from a point to an oriented box, with the closest point on
 * the box optionally written out through the three pointer outputs. */
fx32 OBB_DistSqToPoint(VecFx32 *point, Box *box, fx32 *outX, fx32 *outY, fx32 *outZ) {
    VecFx32 diff;
    fx32 local[3];
    VecFx32 closest;
    fx32 sum;
    int i;

    sum = 0;
    #ifdef SDK_BUILD_ARM
    //todo
    VEC_Subtract((int *)point, (int *)box, (int *)&diff);

    for (i = 0; i < 3; i++) {
        fx32 d = VEC_DotProduct(&diff, &box->axis[i]);
        fx32 halfExtent = box->halfExtent[i];
        local[i] = d;
        if (d < -halfExtent) {
            fx32 pen = d + halfExtent;
            sum += FX_Mul(pen, pen);
            local[i] = -halfExtent;
        } else if (d > halfExtent) {
            fx32 pen = d - halfExtent;
            sum += FX_Mul(pen, pen);
            local[i] = halfExtent;
        }
    }

    closest = box->center;
    for (i = 0; i < 3; i++) {
        VecFx32 scaled;
        ScaleVec3Fx12(local[i], (int *)&box->axis[i], (int *)&scaled);
        VEC_Add((int *)&closest, (int *)&scaled, (int *)&closest);
    }
    #endif

    if (outX) *outX = closest.x;
    if (outY) *outY = closest.y;
    if (outZ) *outZ = closest.z;

    return sum;
}
