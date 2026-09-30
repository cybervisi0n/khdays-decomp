/* Path probe of the ov256 actor: `dir` is turned by the +0x40 heading; mode 0 probes from its +0x74
 * position along it with a 2.19 radius (01fff8e8), otherwise a floor probe runs there (01fff8b8).
 * Returns whether something was hit. */

#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;

extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern void *Collision_CastSphereEx(void *collision, VecFx32 *origin, VecFx32 *dir, int radius, void *ignore);
extern void *Collision_CastSimple(void *collision, VecFx32 *origin, VecFx32 *out, int flag);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

int Ov256_PathProbe(int *node, VecFx32 *dir, int mode)
{
    int *state = (int *)node[1];
    Mtx33 rot;
    VecFx32 floor;
    int scene;
    void *hit;

    {
        int idx = ANG2IDX(state[0x10]) * 2;

        scene = *(int *)(*state + 4);
        MTX_RotY33_(&rot, data_0203d210[idx], data_0203d210[idx + 1]);
    }
    MTX_MultVec33(dir, &rot, dir);
    if (mode == 0) {
        hit = Collision_CastSphereEx(*(void **)(scene + 0x7c), (VecFx32 *)(*state + 0x74), dir, 0x2300, 0);
    } else {
        hit = Collision_CastSimple(*(void **)(scene + 0x7c), (VecFx32 *)(*state + 0x74), &floor, 0);
    }
    return hit != 0;
}
