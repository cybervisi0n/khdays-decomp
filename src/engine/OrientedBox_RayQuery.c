/* OrientedBox_RayQuery -- ray against an oriented box, MAIN. The ray origin (relative to the box centre) and
 * its direction are taken into the box frame with dot products against the three axes; every axis
 * the direction points down is mirrored so the direction is non-negative. The octant then picks the
 * solver: all three components positive -> the dominant-axis projection (Shape_ProjectDominantAxis), two ->
 * Particle_SolvePairConstraint, one -> ClampAxesToBox (each told which axes to use), none -> ClampToLimitsWithPenalty (which
 * also clears the face). The solved local point is mirrored back, rebuilt in world space from the
 * scaled axes and written to the optional out components; the returned value is the solver's. */

#include "nitro/fx_types.h"

typedef struct OrientedBox {
    VecFx32 centre;                     /* +0x00 */
    VecFx32 axis[3];                    /* +0x0c */
    fx32 half[3];                       /* +0x30 */
} OrientedBox;

typedef struct Ray {
    VecFx32 origin;                     /* +0x00 */
    VecFx32 dir;                        /* +0x0c */
} Ray;

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *ab);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *ab);
extern fx32 VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern void ScaleVec3Fx12(fx32 scale, const VecFx32 *v, VecFx32 *out);  /* scale a vector */
extern void Shape_ProjectDominantAxis(VecFx32 *p, VecFx32 *n, const OrientedBox *box, int *pFace, fx32 *pResult);
extern void Particle_SolvePairConstraint(int ax0, int ax1, int ax2, VecFx32 *p, VecFx32 *n, const OrientedBox *box, int *pFace,
                          fx32 *pResult);
extern void ClampAxesToBox(int ax0, int ax1, int ax2, VecFx32 *p, VecFx32 *n, const OrientedBox *box, int *pFace,
                          fx32 *pResult);
extern void ClampToLimitsWithPenalty(VecFx32 *p, const OrientedBox *box, fx32 *pResult);

fx32 OrientedBox_RayQuery(const Ray *ray, const OrientedBox *box, int *pFace, fx32 *outX, fx32 *outY, fx32 *outZ)
{
    VecFx32 d;
    VecFx32 p;
    VecFx32 n;
    int flip[3];
    VecFx32 q;
    VecFx32 v;
    fx32 result;
    int i;
    fx32 *ln = (fx32 *)&n;
    fx32 *lp = (fx32 *)&p;

    VEC_Subtract(&ray->origin, &box->centre, &d);
    p.x = VEC_DotProduct(&d, &box->axis[0]);
    p.y = VEC_DotProduct(&d, &box->axis[1]);
    p.z = VEC_DotProduct(&d, &box->axis[2]);
    n.x = VEC_DotProduct(&ray->dir, &box->axis[0]);
    n.y = VEC_DotProduct(&ray->dir, &box->axis[1]);
    n.z = VEC_DotProduct(&ray->dir, &box->axis[2]);
    for (i = 0; i < 3; i++) {
        if (ln[i] < 0) {
            lp[i] = -lp[i];
            ln[i] = -ln[i];
            flip[i] = 1;
        } else {
            flip[i] = 0;
        }
    }
    result = 0;
    *pFace = 0;
    if (n.x > 0) {
        if (n.y > 0) {
            if (n.z > 0) {
                Shape_ProjectDominantAxis(&p, &n, box, pFace, &result);
            } else {
                Particle_SolvePairConstraint(0, 1, 2, &p, &n, box, pFace, &result);
            }
        } else {
            if (n.z > 0) {
                Particle_SolvePairConstraint(0, 2, 1, &p, &n, box, pFace, &result);
            } else {
                ClampAxesToBox(0, 1, 2, &p, &n, box, pFace, &result);
            }
        }
    } else {
        if (n.y > 0) {
            if (n.z > 0) {
                Particle_SolvePairConstraint(1, 2, 0, &p, &n, box, pFace, &result);
            } else {
                ClampAxesToBox(1, 0, 2, &p, &n, box, pFace, &result);
            }
        } else {
            if (n.z > 0) {
                ClampAxesToBox(2, 0, 1, &p, &n, box, pFace, &result);
            } else {
                ClampToLimitsWithPenalty(&p, box, &result);
                if (pFace != 0) {
                    *pFace = 0;
                }
            }
        }
    }
    q = box->centre;
    for (i = 0; i < 3; i++) {
        if (flip[i]) {
            lp[i] = -lp[i];
        }
        ScaleVec3Fx12(lp[i], &box->axis[i], &v);
        VEC_Add(&q, &v, &q);
    }
    if (outX != 0) {
        *outX = q.x;
    }
    if (outY != 0) {
        *outY = q.y;
    }
    if (outZ != 0) {
        *outZ = q.z;
    }
    return result;
}
