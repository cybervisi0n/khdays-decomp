/* Disc_DistanceSq -- squared distance from a point to a disc, MAIN. The point's offset from the disc
 * centre is split into its height along the normal (+0x24) and the radial part in the disc plane. A
 * filled disc (+0x34 non-zero) is at height^2 away when the radial part lies within the radius (+0x30);
 * otherwise -- and always for a bare ring -- the nearest point is on the rim: the radial direction is
 * scaled to the radius (FX_Sqrt, the fx64c divider, FX_Mul32x64c) and the squared distance to that
 * rim point is returned. A point on the ring's axis is radius^2 + height^2 away. */

#include "nitro/types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/fx/fx_vec.h"

typedef struct Disc {
    VecFx32 centre;                     /* +0x00 */
    char pad0c[0x24 - 0x0c];
    VecFx32 normal;                     /* +0x24 */
    fx32 radius;                        /* +0x30 */
    int filled;                         /* +0x34 */
} Disc;

#define VEC_MAG_SQ(v) ((fx32)(((fx64)(v).x * (v).x + (fx64)(v).y * (v).y + (fx64)(v).z * (v).z + 0x800) >> 12))

fx32 Disc_DistanceSq(const VecFx32 *p, const Disc *disc)
{
    VecFx32 d;
    VecFx32 r;
    VecFx32 c;
    VecFx32 e;
    VecFx32 diffA;
    VecFx32 diffB;
    fx32 h;
    fx32 rr;
    fx64c k;

    VEC_Subtract(p, &disc->centre, &d);
    h = VEC_DotProduct(&d, &disc->normal);
    VEC_MultAdd(-h, &disc->normal, &d, &r);
    rr = VEC_MAG_SQ(r);
    if (disc->filled) {
        if (rr <= FX_Mul(disc->radius, disc->radius)) {
            VEC_Add(&disc->centre, &r, &c);
            return FX_Mul(h, h);
        }
        k = FX_DivFx64c(disc->radius, FX_Sqrt(rr));
        r.x = FX_Mul32x64c(r.x, k);
        r.y = FX_Mul32x64c(r.y, k);
        r.z = FX_Mul32x64c(r.z, k);
        VEC_Add(&disc->centre, &r, &c);
        VEC_Subtract(p, &c, &diffA);
        return VEC_MAG_SQ(diffA);
    }
    if (rr >= 2) {
        k = FX_DivFx64c(disc->radius, FX_Sqrt(rr));
        e.x = FX_Mul32x64c(r.x, k);
        e.y = FX_Mul32x64c(r.y, k);
        e.z = FX_Mul32x64c(r.z, k);
        VEC_Add(&disc->centre, &e, &c);
        VEC_Subtract(p, &c, &diffB);
        return VEC_MAG_SQ(diffB);
    }
    return FX_Mul(disc->radius, disc->radius) + FX_Mul(h, h);
}
