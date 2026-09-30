/* Spawns the ov045 enemy's next effect (x4: ov045/064/083/100) when the rig's countdown at
 * +0x2f00 runs out: the countdown is re-armed to 0x6000 plus a shrinking bonus (0x6000 minus
 * 0x4cd per spawn so far, floored at 0), the spawn count at +0x2efc advances, and one of the
 * offsets of the 16-entry table (10 entries while +0x2f04 is clear) is picked at random,
 * rotated by the actor's heading, scaled by 5, added to the actor origin and jittered on x/z;
 * the point is resolved against the ground and handed to the spawner with the rig. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int m[9]; } Mtx33;
typedef struct { VecFx32 v[16]; } OffsetTable;

extern void MTX_RotY33_(Mtx33 *m, int s, int c);
extern void MTX_MultVec33(const VecFx32 *v, const Mtx33 *m, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);         /* ScaleVec3Fx12 */
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Ov064_ResolveHitPositionOrFallback(VecFx32 *src, VecFx32 *out);
extern void Ov064_SpawnEffectWithFalloff(char *self, char *rig, VecFx32 *src);
extern short data_0203d210[];
extern OffsetTable data_ov064_020b72ac;

void Ov064_SpawnNextEffect(char *self, int dt)
{
    OffsetTable table;
    VecFx32 out;
    Mtx33 m;
    char *rig = self + 0xdf0 + 0x2000;
    int bonus;
    int count;
    int idx;
    int randomOffset;

    *(int *)(rig + 0x110) -= dt;
    if (*(int *)(rig + 0x110) > 0) {
        return;
    }
    bonus = 0x6000 - *(int *)(rig + 0x10c) * 0x4cd;
    if (bonus < 0) {
        bonus = 0;
    }
    *(int *)(rig + 0x110) = bonus + 0x6000;
    *(int *)(rig + 0x10c) += 1;
    table = data_ov064_020b72ac;
    count = 16;
    if (*(int *)(rig + 0x114) == 0) {
        count -= 6;
    }
    idx = (unsigned short)(*(unsigned short *)(*(char **)(self + 0x20) + 0x80) - 0x8000) >> 4;
    MTX_RotY33_(&m, -data_0203d210[idx * 2], -data_0203d210[idx * 2 + 1]);
    MTX_MultVec33(&table.v[Session_RandNextScaled(count)], &m, &out);
    ScaleVec3Fx12(0x5000, &out, &out);
    VEC_Add((VecFx32 *)(self + 0x8c + 0x400), &out, &out);
    randomOffset = Session_RandNext() - 0x800;
    out.x += (int)(((s64)randomOffset * 0x19a + 0x800) >> 12);
    out.y = *(int *)(self + 0x490);
    randomOffset = Session_RandNext() - 0x800;
    out.z += (int)(((s64)randomOffset * 0x19a + 0x800) >> 12);
    Ov064_ResolveHitPositionOrFallback(&out, &out);
    Ov064_SpawnEffectWithFalloff(self, rig, &out);
}
