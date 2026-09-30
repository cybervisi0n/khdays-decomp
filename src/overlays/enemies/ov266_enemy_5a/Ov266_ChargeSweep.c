/* Charge sweep of the ov266 enemy: the +8 point to +4 target direction (normalised, 0x800)
 * and the actor's +0x74 sphere are swept over the actor list (020c8eb8); each candidate whose
 * kind bit (1 << its +2 kind) is not yet in the +0x14 mask is asked to accept a kind-0 hit at
 * 0x200 along the direction -- on success it becomes the +0x5ac order with its +0x190 point
 * copied to +0x5b4 -- and its bit is added. Then, unless the +0x5d8 flag is set, the segment is
 * cast against the world (a thin ray, then a swept sphere of 0x100) and a blocking hit sets it. */

#include "nitro/fx_types.h"

typedef struct { VecFx32 v; int r; } Sphere;

extern int VEC_Subtract(void *a, void *b, void *out);
extern int VEC_Normalize(void *in, void *out);
extern void ScaleVec3Fx12(int scale, void *in, void *out);
extern int Ov107_CollectSphereOverlaps(int owner, Sphere *sphere, int *results);
extern int Ov107_InvokeHitCallback(int ent, int owner, int aux, int mode, VecFx32 *dir, int flag);
extern int *Collision_CastRay(void *world, VecFx32 *origin, VecFx32 *dir);
extern int *Collision_CastSphereEx(void *world, VecFx32 *origin, VecFx32 *dir, int radius, void *extra);

void Ov266_ChargeSweep(int *node)
{
    char *state = (char *)node[1];
    VecFx32 dir;
    VecFx32 seg;
    Sphere sphere;
    int results[4];
    int i;
    int n;
    unsigned char bit;
    int *hit;
    char *owner;

    VEC_Subtract((void *)*(int *)(state + 4), state + 8, &dir);
    VEC_Normalize(&dir, &seg);
    ScaleVec3Fx12(0x800, &seg, &seg);
    sphere = *(Sphere *)(*(int *)state + 0x74);
    n = Ov107_CollectSphereOverlaps(*(int *)(*(int *)state + 0x384), &sphere, results);
    for (i = 0; i < n; i++) {
        bit = 1 << *(unsigned short *)(results[i] + 2);
        if ((*(unsigned char *)(state + 0x14) & bit) == 0) {
            if (Ov107_InvokeHitCallback(results[i], *(int *)state, *(int *)(*(int *)state + 0x384), 0, &seg, 0x200) != 0) {
                *(int *)(*(int *)(*(int *)state + 0x384) + 0x5ac) = results[i];
                *(VecFx32 *)(*(int *)(*(int *)state + 0x384) + 0x1b4 + 0x400) = *(VecFx32 *)(results[i] + 0x190);
            }
            *(unsigned char *)(state + 0x14) |= bit;
        }
    }
    if (*(int *)(*(int *)(*(int *)state + 0x384) + 0x5d8) != 0) {
        return;
    }
    owner = *(char **)(*(int *)state + 4);
    hit = Collision_CastRay(*(void **)(owner + 0x7c), (VecFx32 *)(state + 8), &dir);
    if (hit != 0) {
        *(int *)(*(int *)(*(int *)state + 0x384) + 0x5d8) = 1;
        return;
    }
    hit = Collision_CastSphereEx(*(void **)(owner + 0x7c), (VecFx32 *)(state + 8), &dir, 0x100, 0);
    if (hit != 0) {
        *(int *)(*(int *)(*(int *)state + 0x384) + 0x5d8) = 1;
    }
}
