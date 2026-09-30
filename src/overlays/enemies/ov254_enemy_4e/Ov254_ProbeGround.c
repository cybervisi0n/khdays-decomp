/* Ground probe: cast a ray 200.0 down from `pos` raised to a height of 100.0 through the actor
 * list's +0x7c collision grid. Without a hit returns -1; otherwise, when `outY` is given, stores
 * the height of the hit point, and returns the hit face's +0x83 material byte. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern int Collision_CastRay(int grid, VecFx32 *pos, VecFx32 *ray);
extern void ScaleVec3Fixed27(int scale, VecFx32 *in, VecFx32 *out);
extern void VEC_Add(const void *a, const void *b, void *out);

int Ov254_ProbeGround(int *self, VecFx32 pos, int *outY)
{
    int grid = *(int *)(self[0] + 4);
    VecFx32 ray;
    int hit;

    pos.y = 0x64000;
    ray.x = 0;
    ray.y = -0xc8000;
    ray.z = 0;
    hit = Collision_CastRay(*(int *)(grid + 0x7c), &pos, &ray);
    if (hit != 0) {
        if (outY != 0) {
            ScaleVec3Fixed27(*(int *)(hit + 0xc), &ray, &ray);
            VEC_Add(&ray, &pos, &pos);
            *outY = pos.y;
        }
        return *(u8 *)(*(int *)(hit + 4) + 0x83);
    }
    return -1;
}
