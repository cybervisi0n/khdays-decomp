/* Sweep of the ov221 enemy's strike: the entities inside the given sphere are tested one by
 * one -- each whose +2 id bit is clear in the +0x64 mask is pushed 1.0 along the flattened
 * unit direction from the owner's +0x74 (kind +0x58 byte) and, on acceptance, the sphere's
 * centre goes out as the mode-0 message (Ov224_PushVector), reaction 0x14a
 * mode 8 fires at the +8 point and the bit is set. Returns the entity count. */

#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; int nRadius; } Sphere;

extern int Ov107_CollectSphereOverlaps(int owner, Sphere *query, int *out);
extern void VEC_Subtract(void *a, void *b, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *a, VecFx32 *d);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void Ov224_PushVector(int *state, VecFx32 v, int flag);
struct Ov221Byte8 { unsigned int lo : 8, rest : 24; };
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);

int Ov224_SweepStrike(int *node, Sphere *sphere)
{
    int *state = (int *)node[1];
    int hits[4];
    VecFx32 push;
    long i;
    long n;
    unsigned char bit;

    n = Ov107_CollectSphereOverlaps(*state, sphere, hits);
    i = 0;
    if (n > 0) {
        do {
            bit = 1 << *(unsigned short *)(hits[i] + 2);
            if ((*(unsigned char *)((char *)state + 0x64) & bit) == 0) {
                VEC_Subtract((void *)(hits[i] + 0x74), (void *)(*state + 0x74), &push);
                push.y = 0;
                VEC_Normalize(&push, &push);
                ScaleVec3Fx12(0x1000, &push, &push);
                if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x390), state[0x16] & 0xff, &push, 0) != 0) {
                    Ov224_PushVector(state, sphere->pos, 0);
                    Ov107_BuildAndSendUpdate(*state, 0x14a, 8, (void *)state[2]);
                    *(unsigned char *)((char *)state + 0x64) |= bit;
                }
            }
        } while (++i < n);
    }
    return n;
}
