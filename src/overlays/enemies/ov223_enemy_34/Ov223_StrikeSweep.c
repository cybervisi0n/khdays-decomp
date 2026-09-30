/* Strike sweep of the ov223 enemy's charge. With a segment the entities come from the +0x38c
 * item's segment query (ov107 c8f44) and the segment is cast against the world twice -- a
 * swept sphere of its radius and a plain ray -- and the first clean hit builds a 32-byte
 * kind-5 command-2 message (id, the pose facing the plane normal from data_02042264, the hit
 * point packed as 24-bit values) for the owner's +0x24 hook; with a sphere they come from the
 * sphere query (ov107 c8eb8). Every entity whose kind bit is clear in the +0x4c mask is
 * pushed along the flattened unit direction from the owner's +0x74 with the +0x48 variant as
 * the kind; on acceptance the entity's +0x74 (or the sphere's point) goes to ov223 43e4,
 * reaction 0x149 mode 8 fires at the +8 point and the bit is set. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int value; } Fx32;
typedef struct { int q[4]; } Quat;
typedef struct { VecFx32 pos; int nRadius; } Sphere;
typedef struct { VecFx32 p0; VecFx32 dir; int nLength; int nRadius; } Segment;

typedef struct {
    u16 id;
    u8 kind;
    u8 cmd;
    Quat pose;
    u8 pos[9];
    u8 pad[3];
} HitMsg;

typedef struct { char pad00[0x14]; short x, y, z; } PlaneS16;
struct CollisionHit { int pad00; PlaneS16 *pPlane; int nBlocked; int nAlong; };

struct Ov223Hit { char pad000[2]; u16 nKind; char pad004[0x70]; VecFx32 vOrigin74; };

#define PACK(msg, dead, src, at)                                              \
    (dead) = (src);                                                           \
    (msg).pos[at] = (u8)(((u32)(dead).value >> 0x10 & 0x7f)                   \
                         | ((u32)(dead).value >> 0x18 & 0x80));               \
    (msg).pos[(at) + 1] = (u8)((u32)(dead).value >> 8);                       \
    (msg).pos[(at) + 2] = (u8)(dead).value

extern int Ov107_CollectSegmentOverlaps(int item, Segment *query, struct Ov223Hit **results);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern struct CollisionHit *Collision_CastSphereEx(void *collision, VecFx32 *origin, VecFx32 *dir, int radius, void *ignore);
extern void ScaleVec3Fixed27(int scale, VecFx32 *in, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern struct CollisionHit *Collision_CastRay(void *collision, VecFx32 *origin, VecFx32 *dir);
extern int Ov107_CollectSphereOverlaps(int item, Sphere *query, struct Ov223Hit **results);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(struct Ov223Hit *hit, int owner, int item, int kind,
                                   VecFx32 *push, int z);
extern void Ov223_ForwardVecToOwner(int *state, VecFx32 v);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern const VecFx32 data_02042264;

void Ov223_StrikeSweep(int *node, Sphere *pSphere, Segment *pSeg)
{
    int *state = (int *)node[1];
    struct Ov223Hit *hits[4];
    VecFx32 end;
    VecFx32 normal;
    VecFx32 beam;
    int n;
    int i;
    u8 bit;
    u16 *p;
    char *coll;
    struct CollisionHit *hit;

    if (pSeg != 0) {
        coll = *(char **)(*state + 4);
        {
        HitMsg msg = {0};
        p = (u16 *)&msg;
        n = Ov107_CollectSegmentOverlaps(*(int *)(*state + 0x38c), pSeg, hits);
        p[0] = *(u16 *)(*state + 2);
        ((u8 *)p)[2] = 5;
        ((u8 *)p)[3] = 2;
        ScaleVec3Fx12(pSeg->nLength, &pSeg->dir, &beam);
        hit = Collision_CastSphereEx(*(void **)(coll + 0x7c), &pSeg->p0, &beam, pSeg->nRadius, 0);
        if (hit != 0 && hit->nBlocked == 0) {
            Fx32 scratchZ;
            Fx32 scratchY;
            Fx32 scratchX;
            normal.x = hit->pPlane->x;
            normal.y = hit->pPlane->y;
            normal.z = hit->pPlane->z;
            ScaleVec3Fixed27(hit->nAlong, &beam, &end);
            VEC_Add(&pSeg->p0, &end, &end);
            PACK(msg, scratchX, *(Fx32 *)&end.x, 0);
            PACK(msg, scratchY, *(Fx32 *)&end.y, 3);
            PACK(msg, scratchZ, *(Fx32 *)&end.z, 6);
            Quat_FromTwoVectors(&msg.pose, &data_02042264, &normal);
            if (*(void (**)(int, HitMsg *, int))(*state + 0x24) != 0) {
                (*(void (**)(int, HitMsg *, int))(*state + 0x24))(*state, &msg, 0x20);
            }
        } else {
            hit = Collision_CastRay(*(void **)(coll + 0x7c), &pSeg->p0, &beam);
            if (hit != 0 && hit->nBlocked == 0) {
                Fx32 scratchZ;
                Fx32 scratchY;
                Fx32 scratchX;
                normal.x = hit->pPlane->x;
                normal.y = hit->pPlane->y;
                normal.z = hit->pPlane->z;
                ScaleVec3Fixed27(hit->nAlong, &beam, &end);
                VEC_Add(&pSeg->p0, &end, &end);
                PACK(msg, scratchX, *(Fx32 *)&end.x, 0);
                PACK(msg, scratchY, *(Fx32 *)&end.y, 3);
                PACK(msg, scratchZ, *(Fx32 *)&end.z, 6);
                Quat_FromTwoVectors(&msg.pose, &data_02042264, &normal);
                if (*(void (**)(int, HitMsg *, int))(*state + 0x24) != 0) {
                    (*(void (**)(int, HitMsg *, int))(*state + 0x24))(*state, &msg, 0x20);
                }
            }
        }
        }
    } else if (pSphere != 0) {
        n = Ov107_CollectSphereOverlaps(*(int *)(*state + 0x38c), pSphere, hits);
    }
    for (i = 0; i < n; i++) {
        VecFx32 push;
        bit = (u8)(1 << hits[i]->nKind);
        if ((*(u8 *)((char *)state + 0x4c) & bit) != 0) {
            continue;
        }
        VEC_Subtract(&hits[i]->vOrigin74, (VecFx32 *)(*state + 0x74), &push);
        push.y = 0;
        VEC_Normalize(&push, &push);
        if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x38c), (u8)state[0x12], &push, 0) != 0) {
            if (pSeg != 0) {
                Ov223_ForwardVecToOwner(state, hits[i]->vOrigin74);
            } else if (pSphere != 0) {
                Ov223_ForwardVecToOwner(state, pSphere->pos);
            }
            Ov107_BuildAndSendUpdate(*state, 0x149, 8, (void *)state[1]);
            *(u8 *)((char *)state + 0x4c) |= bit;
        }
    }
}
