
/* The hit record the caller hands over. The leading byte picks the shape, and
 * what follows the source point is either a radius or a second point. */

#include "nitro/fx_types.h"

typedef struct {
    signed char nShape;             /* +0x00 */
    char pad01[3];
    VecFx32 vFrom;                  /* +0x04 */
    union {
        int nRadius;                /* +0x10, shape 0 */
        VecFx32 vTo;                /* +0x10, shape 1 */
    } u;
} Ov002HitShape;                   /* 0x1c */

extern int Ov002_IsWithinRadii(VecFx32 *pFrom, int nRadius, VecFx32 *pAt,
                               int nExtent);
extern int Ov002_PointWithinBoxRange(VecFx32 *pFrom, VecFx32 *pTo, VecFx32 *pAt,
                               int nExtent);
extern int Ov002_IsBeyondDistance(VecFx32 *pAt, int nExtent, VecFx32 *pFrom);

/* Test one hit record against a timed element.
 *
 * Only mode 1 is handled; anything else reports -1 so the caller knows the
 * record was not consumed. Shape 0 is a sphere, shape 1 a segment and shape 2
 * a point, each tested against the element's position at +0x1c with the extent
 * at +0x28. The answer is the plain hit/no-hit boolean.
 *
 * The handled mode has to be the taken branch and the three shapes an if/else
 * chain: writing the guard the other way round predicates the tail, and a
 * switch turns the chain into a dispatch.
 */
int Ov002_ElementTestHit(char *pElement, int nMode, Ov002HitShape *pShape)
{
    int nShape;
    int nHit;

    if (nMode == 1) {
        nShape = pShape->nShape;
        nHit = 0;

        if (nShape == 0) {
            nHit = Ov002_IsWithinRadii(&pShape->vFrom, pShape->u.nRadius,
                                       (VecFx32 *)(pElement + 0x1c),
                                       *(int *)(pElement + 0x28));
        } else if (nShape == 1) {
            nHit = Ov002_PointWithinBoxRange(&pShape->vFrom, &pShape->u.vTo,
                                       (VecFx32 *)(pElement + 0x1c),
                                       *(int *)(pElement + 0x28));
        } else if (nShape == 2) {
            nHit = Ov002_IsBeyondDistance((VecFx32 *)(pElement + 0x1c),
                                       *(int *)(pElement + 0x28),
                                       &pShape->vFrom);
        }

        return nHit != 0;
    }

    return -1;
}
