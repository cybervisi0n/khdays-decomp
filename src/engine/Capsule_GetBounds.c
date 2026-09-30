/* Bounding box of a swept sphere: from the capsule's start point (+0) along its direction (+0xc)
 * times its length (+0x18), widened on every axis by the radius (+0x1c). The box is
 * min x/y/z then max x/y/z. */

#include "nitro/fx_types.h"

typedef struct {
    VecFx32 pos;        /* 0x00 */
    VecFx32 dir;        /* 0x0c */
    int length;         /* 0x18 */
    int radius;         /* 0x1c */
} Capsule;

typedef struct {
    VecFx32 min;
    VecFx32 max;
} Box;

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) >= (b) ? (a) : (b))

void Capsule_GetBounds(Box *box, Capsule *c)
{
    VecFx32 end;
    VecFx32 start;

    ScaleVec3Fx12(c->length, &c->dir, &end);
    VEC_Add(&c->pos, &end, &end);
    start = c->pos;
    box->min.x = MIN(start.x, end.x) - c->radius;
    box->max.x = MAX(start.x, end.x) + c->radius;
    box->min.y = MIN(start.y, end.y) - c->radius;
    box->max.y = MAX(start.y, end.y) + c->radius;
    box->min.z = MIN(start.z, end.z) - c->radius;
    box->max.z = MAX(start.z, end.z) + c->radius;
}
