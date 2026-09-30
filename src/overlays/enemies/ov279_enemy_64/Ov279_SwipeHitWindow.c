/* Swipe hit window of the ov279 enemy. The +0x50 timer advances by the
 * node's +0x2c speed; between 0.56 and 1.13 it queries a 0.28-radius sphere at the midpoint of the
 * two +0x394/+0x398 bone points, and every hit whose kind bit (1 << +2) is not yet in the +0x70
 * mask is pushed along the flattened unit direction from the +0x4c point (zero when degenerate):
 * a landed hit spawns effect 2 at the sphere, records the kind bit and fires reaction 0 mode 0x50
 * at the point. Once the owner's +0xad busy byte is clear, sub-state 8 is requested. */

#include "nitro/fx_types.h"

typedef struct { VecFx32 c; int r; } Sphere;

extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_CollectSphereOverlaps(int owner, Sphere *src, int *out);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int victim, int a, int b, int mode, VecFx32 *push, int flags);
extern void func_ov107_020c0b90(int obj, int cmd, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int obj, int effect, int kind, void *pos);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042258;

void Ov279_SwipeHitWindow(int *node)
{
    int *state = (int *)node[1];
    int hits[4];
    Sphere sphere;
    VecFx32 dir;
    int n;
    int i;

    state[0x14] += *(int *)(*node + 0x2c);
    if (state[0x14] > 0x900 && state[0x14] <= 0x1200) {
        VEC_Add((VecFx32 *)(*(int *)(*state + 0x398) + 0x14), (VecFx32 *)(*(int *)(*state + 0x394) + 0x14), &sphere.c);
        ScaleVec3Fx12(0x800, &sphere.c, &sphere.c);
        sphere.r = 0x900;
        n = Ov107_CollectSphereOverlaps(*state, &sphere, hits);
        for (i = 0; i < n; i++) {
            if ((*(unsigned char *)((char *)state + 0x70) & (1 << *(unsigned short *)(hits[i] + 2))) != 0) {
                continue;
            }
            VEC_Subtract((void *)(hits[i] + 0x74), (void *)state[0x13], &dir);
            dir.y = 0;
            if (VEC_Normalize(&dir, &dir) == 0) {
                dir = data_02042258;
            }
            ScaleVec3Fx12(0x1000, &dir, &dir);
            if (Ov107_InvokeHitCallback(hits[i], *state, *state, 0, &dir, 0) != 0) {
                func_ov107_020c0b90(*state, 2, sphere.c, 0);
                *(unsigned char *)((char *)state + 0x70) |= 1 << *(unsigned short *)(hits[i] + 2);
                Ov107_BuildAndSendUpdate(*state, 0, 0x50, (void *)state[0x13]);
            }
        }
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    *(unsigned char *)(*state + 0x1c7) = 8;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
