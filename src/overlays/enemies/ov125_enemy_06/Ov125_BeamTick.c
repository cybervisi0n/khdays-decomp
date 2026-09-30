/* Beam tick of the ov125 enemy (aimed at the player). The aim point is the midpoint of the two
 * anchors (+0x394 / +0x398 at +0x14) pulled 0x100 towards the player's +0x7c; the +0x30 timer
 * accumulates the owner's rate and, capped at 1.0 of 0x800, gives the beam length (x30). The
 * state[2] sub-node faces the aim point and state[1] takes the owner's +0xa0 basis turned by
 * data_02042258 (ed60 by data_02042240 + normalise) placed at the aim point. The beam (forward x
 * length) is cast against the world twice -- a swept sphere of 0x100 and a plain ray -- and the
 * nearer plane hit (squared plane distance of the aim point) clips it: the end point, its length
 * and the plane normal are kept, else the beam is normalised and the length is 30.0. state[1]
 * is scaled 2.0 x (length x 0.2) x 2.0; with a hit state[5] is placed 0x200 along the normal from
 * the end point and state[3] at the end point (both unflagged), otherwise both are flagged. In
 * owner mode 1 the beam capsule (aim, end, length, 0x200) is swept over the actor list and the
 * first candidate accepted along the unit vector from its closest point (0x400) gets the
 * overlay's 14-byte command with that point packed in, phase 2 is queued and reaction 0x11b/6
 * fires at the point.
 *
 * Codegen: nLen is declared first and the hit flags before the two result pointers -- that is
 * the ROM's callee-saved assignment (nLen r6, nBest r7, bHit1 r8, r1/bHit2 r5, owner/r2 sb). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int value; } Fx32;
typedef struct { int q[4]; } Quat;
typedef struct { u16 h[7]; } Cmd14;

typedef struct {
    VecFx32 p0;
    VecFx32 dir;
    int nLength;
    int nRadius;
} Segment;

typedef struct {
    char pad00[0x14];
    short x, y, z;
    short pad1a;
    int d;
} PlaneS16;

typedef struct {
    VecFx32 n;
    int d;
} Plane;

struct CollisionResult {
    int pad00;
    PlaneS16 *pPlane;
    int field08;
    int nAlong;
};

struct Ov125Phase { int cur : 4, next : 4; };

extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int *Ov107_GetActorManager(void);
extern int FX_Div(int num, int den);
extern void Srt_SetTranslation(void *p, VecFx32 *v);
extern void Vec3TransformViaTempMtx(VecFx32 *out, void *pose, void *k);
extern void Quat_FromTwoVectors(Quat *dst, void *src, VecFx32 *m);
extern void Vec4_Normalize(Quat *out, Quat *in);
extern void Srt_SetRotationQuat(void *pose, Quat *q);
extern struct CollisionResult *Collision_CastSphereEx(void *collision, VecFx32 *position, VecFx32 *direction, int radius, void *ignore);
extern void ScaleVec3Fixed27(int scale, VecFx32 *in, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern struct CollisionResult *Collision_CastRay(void *collision, VecFx32 *position, VecFx32 *direction);
extern void Srt_SetScaleXYZ(void *placement, int x, int y, int z);
extern int Ov107_CollectSegmentOverlaps(int owner, Segment *query, int *results);
extern int Segment_ClosestPoint(VecFx32 *point, Segment *seg, fx64 *outDist);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int ent, int owner, int aux, int mode, VecFx32 *dir, int flag);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int a, VecFx32 *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int data_02042258;
extern int data_02042240;
extern Cmd14 data_ov125_020d03dc;

static inline int FX_Mul(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

#define PACK(cmd, dead, src, at)                                              \
    (dead) = (src);                                                           \
    ((u8 *)&(cmd))[at] = (u8)(((unsigned int)(dead).value >> 0x10 & 0x7f)     \
                              | ((unsigned int)(dead).value >> 0x18 & 0x80)); \
    ((u8 *)&(cmd))[(at) + 1] = (u8)((unsigned int)(dead).value >> 8);         \
    ((u8 *)&(cmd))[(at) + 2] = (u8)(dead).value

void Ov125_BeamTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 aim;
    Quat q;
    VecFx32 fwd;
    VecFx32 end;
    VecFx32 beam;
    VecFx32 normal;
    VecFx32 sum;
    Plane plane;
    VecFx32 dir;
    VecFx32 pull;
    VecFx32 side;
    Segment axis;
    int results[4];
    VecFx32 hit;
    VecFx32 dir2;
    Cmd14 cmd;
    fx64 along;
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;
    int n;
    int nHiY;
    int nHiZ;
    int nLen;
    int nBest = 0x7fffffff;
    int bHit1;
    int bHit2;
    struct CollisionResult *r1;
    struct CollisionResult *r2;
    char *collisionOwner = *(char **)(*state + 4);
    int nDist;
    int t;
    int i;

    VEC_Add((VecFx32 *)(*(int *)(*state + 0x394) + 0x14), (VecFx32 *)(*(int *)(*state + 0x398) + 0x14), &aim);
    ScaleVec3Fx12(0x800, &aim, &aim);
    ScaleVec3Fx12(-0x100, (VecFx32 *)(*Ov107_GetActorManager() + 0x7c), &pull);
    VEC_Add(&aim, &pull, &dir);
    state[0xc] += *(int *)(node[0] + 0x2c);
    t = FX_Div(state[0xc], 0x800);
    if (t > 0x1000) t = 0x1000;
    nLen = t * 30;
    Srt_SetTranslation((void *)(state[2] + 4), &dir);
    Vec3TransformViaTempMtx(&fwd, (char *)*state + 0xa0, &data_02042258);
    Quat_FromTwoVectors(&q, &data_02042240, &fwd);
    Vec4_Normalize(&q, &q);
    Srt_SetRotationQuat((void *)(state[1] + 4), &q);
    Srt_SetTranslation((void *)(state[1] + 4), &aim);
    ScaleVec3Fx12(nLen, &fwd, &beam);
    r1 = Collision_CastSphereEx(*(void **)(collisionOwner + 0x7c), &aim, &beam, 0x100, 0);
    if (r1 != 0 && r1->field08 == 0) {
        ScaleVec3Fixed27(r1->nAlong, &beam, &end);
        VEC_Add(&aim, &end, &sum);
        nLen = VEC_Normalize(&end, &end);
        bHit1 = 1;
        normal.x = r1->pPlane->x;
        normal.y = r1->pPlane->y;
        normal.z = r1->pPlane->z;
        plane.n = normal;
        plane.d = r1->pPlane->d;
        nDist = VEC_DotProduct(&plane.n, &aim) - plane.d;
        nBest = FX_Mul(nDist, nDist);
    } else {
        bHit1 = 0;
    }
    r2 = Collision_CastRay(*(void **)(collisionOwner + 0x7c), &aim, &beam);
    if (r2 != 0 && r2->field08 == 0) {
        normal.x = r2->pPlane->x;
        normal.y = r2->pPlane->y;
        normal.z = r2->pPlane->z;
        plane.n = normal;
        plane.d = r2->pPlane->d;
        bHit2 = 0;
        if (bHit1 != 0) {
            nDist = VEC_DotProduct(&plane.n, &aim) - plane.d;
            bHit2 = 0;
            if (nBest < FX_Mul(nDist, nDist)) {
                goto scaled;
            }
        }
        {
            ScaleVec3Fixed27(r2->nAlong, &beam, &end);
            VEC_Add(&aim, &end, &sum);
            nLen = VEC_Normalize(&end, &end);
            bHit2 = 1;
        }
    } else {
        VEC_Normalize(&beam, &end);
        nLen = 0x1e000;
        bHit2 = 0;
    }
scaled:
    Srt_SetScaleXYZ((void *)(state[1] + 4), 0x2000, FX_Mul(nLen, 0x3333), 0x2000);
    if (bHit2 != 0 || bHit1 != 0) {
        ScaleVec3Fx12(0x200, &normal, &side);
        VEC_Add(&side, &sum, &side);
        *(int *)(state[5] + 0x5c) &= ~2;
        Srt_SetTranslation((void *)(state[5] + 4), &side);
        *(int *)(state[3] + 0x5c) &= ~2;
        Srt_SetTranslation((void *)(state[3] + 4), &sum);
    } else {
        *(int *)(state[5] + 0x5c) |= 2;
        *(int *)(state[3] + 0x5c) |= 2;
    }
    if (*(int *)(*state + 0x50) != 1) {
        return;
    }
    axis.p0 = aim;
    axis.dir = end;
    axis.nLength = nLen;
    axis.nRadius = 0x200;
    n = Ov107_CollectSegmentOverlaps(*state, &axis, results);
    for (i = 0; i < n; i++) {
        Segment_ClosestPoint((VecFx32 *)(results[i] + 0x74), &axis, &along);
        hit.x = (fx32)((along * axis.dir.x + 0x80000000LL) >> 32);
        hit.y = (fx32)((along * axis.dir.y + 0x80000000LL) >> 32);
        hit.z = (fx32)((along * axis.dir.z + 0x80000000LL) >> 32);
        VEC_Add(&axis.p0, &hit, &hit);
        VEC_Subtract((VecFx32 *)(results[i] + 0x74), &hit, &dir2);
        VEC_Normalize(&dir2, &dir2);
        ScaleVec3Fx12(0x400, &dir2, &dir2);
        if (Ov107_InvokeHitCallback(results[i], *state, *state, 1, &dir2, 0) != 0) {
            cmd = data_ov125_020d03dc;
            PACK(cmd, scratchX, *(Fx32 *)&hit.x, 5);
            PACK(cmd, scratchY, *(Fx32 *)&hit.y, 8);
            PACK(cmd, scratchZ, *(Fx32 *)&hit.z, 11);
            if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
                (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &cmd, 0xe);
            }
            ((struct Ov125Phase *)(state + 0xd))->next = 2;
            Ov107_BuildAndSendUpdate(*state, 0x11b, 6, &hit);
            SetIndexedSlot(node, *(signed char *)((int)node + 0x20), 0);
            return;
        }
    }
}
