/* Ground drop tick of the ov158 enemy. In owner mode 1 the +0x28 radius grows by the owner's
 * rate and, up to 0xe00, the entities inside 2.2 times that radius around the +0x10 anchor are
 * swept: each one whose +0x1b4 slot bit is still clear in the +0x30 hit mask is tested along
 * the unit direction from the anchor (scaled 0x800) and, once accepted, fires reaction 0 mode
 * 0x53 at the anchor and marks its bit. The state ends once the +0xc sub-object goes idle
 * (+0xad). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { VecFx32 pos; int nRadius; } Sphere;

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

extern int Ov107_CollectSphereOverlaps(int owner, Sphere *query, int *results);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int ent, int owner, int aux, int mode, VecFx32 *dir, int flag);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);

void Ov159_GroundDropTick(int *node)
{
    int *state = (int *)node[1];
    int results[4];
    Sphere query;
    VecFx32 dir;
    int i;
    int n;

    if (*(int *)(*state + 0x50) == 1) {
        state[0xa] += *(int *)(node[0] + 0x2c);
        if (state[0xa] <= 0xe00) {
            query.pos = *(VecFx32 *)(state + 4);
            query.nRadius = FX_Mul(state[0xa], 0x2333);
            n = Ov107_CollectSphereOverlaps(*state, &query, results);
            for (i = 0; i < n; i++) {
                VEC_Subtract((VecFx32 *)(results[i] + 0x74), &query.pos, &dir);
                VEC_Normalize(&dir, &dir);
                ScaleVec3Fx12(0x800, &dir, &dir);
                if (((*(u8 *)((char *)state + 0x30) >> *(u8 *)(results[i] + 0x1b4)) & 1) == 0
                    && Ov107_InvokeHitCallback(results[i], *state, *state, 0, &dir, 0) != 0) {
                    Ov107_BuildAndSendUpdate(*state, 0, 0x53, state + 4);
                    *(u8 *)((char *)state + 0x30) |= 1 << *(u8 *)(results[i] + 0x1b4);
                }
            }
        }
    }
    if (*(u8 *)(state[3] + 0xad) != 0) {
        return;
    }
    Task_MarkFinished(node);
}
