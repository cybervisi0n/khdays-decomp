/* Closest approach between a swept sphere (capsule: start +0, direction +0xc, length +0x18) and
 * `other`: the segment test (OrientedBox_RayQuery) gives the parameter along the capsule; outside
 * [0, length] the end point (start or start + dir * length) is tested instead (OBB_DistSqToPoint) and
 * the parameter clamped. The parameter and the three results the tests report are stored through
 * the optional out pointers; the test's own result is returned. */

#include "nitro/fx_types.h"

typedef struct {
    VecFx32 pos;        /* 0x00 */
    VecFx32 dir;        /* 0x0c */
    int length;         /* 0x18 */
    int radius;         /* 0x1c */
} Capsule;

typedef struct {
    VecFx32 start;
    VecFx32 delta;
} Segment;

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int OrientedBox_RayQuery(Segment *seg, void *other, int *t, int *a, int *b, int *c);
extern int OBB_DistSqToPoint(VecFx32 *point, void *other, int *a, int *b, int *c);

int Capsule_ClosestToBox(Capsule *cap, void *other, int *outT, int *outA, int *outB, int *outC)
{
    Segment seg;
    VecFx32 end;
    int t;
    int a;
    int b;
    int c;
    int result;

    seg.start = cap->pos;
    ScaleVec3Fx12(cap->length, &cap->dir, &seg.delta);
    result = OrientedBox_RayQuery(&seg, other, &t, &a, &b, &c);
    if (t >= 0) {
        if (t <= cap->length) {
            if (outT) {
                *outT = t;
            }
        } else {
            ScaleVec3Fx12(cap->length, &cap->dir, &end);
            VEC_Add(&cap->pos, &end, &end);
            result = OBB_DistSqToPoint(&end, other, &a, &b, &c);
            if (outT) {
                *outT = cap->length;
            }
        }
    } else {
        result = OBB_DistSqToPoint(&cap->pos, other, &a, &b, &c);
        if (outT) {
            *outT = 0;
        }
    }
    if (outA) {
        *outA = a;
    }
    if (outB) {
        *outB = b;
    }
    if (outC) {
        *outC = c;
    }
    return result;
}
