/* Hit sweep of an ov227 part: the given sphere is queried against the owner's +0x38c body; every
 * target whose kind bit (1 << +2) is not yet in the +0x28 mask is pushed along the flattened unit
 * direction from the owner (hit kind 1) and, when that lands, records its kind bit. After any hit
 * reaction 0x14d mode 0x10 fires at the +4 point. Returns whether anything was hit. */

#include "nitro/fx_types.h"

typedef struct { VecFx32 c; int r; } Sphere;

extern int Ov107_CollectSphereOverlaps(int body, Sphere *src, int *out);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int mode, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);

int Ov227_HitSweep(int *part, Sphere *sphere)
{
    int hits[4];
    VecFx32 dir;
    int n;
    int i;
    int hit;
    unsigned int mask;

    hit = 0;
    n = Ov107_CollectSphereOverlaps(*(int *)(*part + 0x38c), sphere, hits);
    for (i = 0; i < n; i++) {
        mask = (1 << *(unsigned short *)(hits[i] + 2)) & 0xff;
        if ((*(unsigned char *)((char *)part + 0x28) & mask) != 0) {
            continue;
        }
        VEC_Subtract((void *)(hits[i] + 0x74), (void *)(*part + 0x74), &dir);
        dir.y = 0;
        VEC_Normalize(&dir, &dir);
        if (Ov107_InvokeHitCallback(hits[i], *part, *(int *)(*part + 0x38c), 1, &dir, 0) != 0) {
            hit = 1;
            *(unsigned char *)((char *)part + 0x28) |= mask;
        }
    }
    if (hit != 0) {
        Ov107_BuildAndSendUpdate(part[0], 0x14d, 0x10, (void *)part[1]);
    }
    return hit;
}
