/* Collect up to four collision contact points for a fixed-point sphere. */

#include "nitro/fx_types.h"

typedef struct { VecFx32 center; int radius; } Sphere;
typedef struct { int minX, minZ, maxX, maxZ; } BBox;
typedef struct { int a, b; } Pair;

typedef struct {
    char pad0[4];
    void *field4;
} Ctx;

typedef struct {
    char pad0[0x84];
    int f84;
    int f88;
    int f8c;
    char pad90[0x9c - 0x90];
    int f9c;
    char pada0[0xa4 - 0xa0];
    int fa4;
} Inner;

extern int Ov107_CollectSphereContacts(Pair *pair, int f8c, int f9c, int fa4,
                                Sphere *b, BBox *bbox, int *count,
                                VecFx32 *outList, VecFx32 *outDir);

int Ov107_QuerySphereContacts(Ctx *a, Sphere *b, VecFx32 *outList, VecFx32 *outDir) {
    int count = 0;
    BBox bbox;
    Pair pair;
    Inner *inner;

    bbox.minX = b->center.x - b->radius;
    bbox.minZ = b->center.z - b->radius;
    bbox.maxX = b->center.x + b->radius;
    bbox.maxZ = b->center.z + b->radius;

    inner = *(Inner **)a->field4;
    pair.a = inner->f84;
    pair.b = inner->f88;

    Ov107_CollectSphereContacts(&pair, inner->f8c, inner->f9c, inner->fa4,
                         b, &bbox, &count, outList, outDir);
    return count;
}
