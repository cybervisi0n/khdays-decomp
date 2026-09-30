/* Leap tick of the ov213 enemy (twin of ov273 d5ba0): the +0x14 clock runs up at the owner's rate and
 * t = clock / 2.0 (capped at 1.0) drives a cubic Hermite curve from +0x30 (tangent +0x18) to +0x3c
 * (tangent +0x24); the step from the +4 position is kept in +8. The owner's sphere (+0x74), moved by
 * the step, collects the entities around it; each one is pushed by 1/16 along the flattened direction
 * away from the sphere (kind 5) and, on acceptance, the overlay's 14-byte template carries the
 * entity's +0x74 position to the owner's +0x24 hook and reaction 0x4f fires at the owner. At the end of
 * the curve, on landing (+0x17a bit 1) or after a hit, the landing template carries the owner's
 * position to the hook, reaction 0x122 mode 0xd fires when nothing was hit, and sub-state 0 follows. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { u8 hi, mid, lo; } Fx24;
typedef struct { int value; } Fx32;
typedef struct { VecFx32 pos; int nRadius; } Sphere;
struct b2 { u8 b0 : 1, b1 : 1; };

typedef struct {
    u16 id;             /* +0x0 */
    u8 kind;            /* +0x2 */
    u8 cmd;             /* +0x3 */
    u8 flag;            /* +0x4 */
    Fx24 pos[3];        /* +0x5 */
} PosMsg;

struct Ov213State {
    int pOwner;                  /* 0x00 */
    VecFx32 *pPos;                  /* 0x04 */
    VecFx32 vStep;                  /* 0x08 */
    int nClock;                  /* 0x14 */
    VecFx32 vTan0;                  /* 0x18 */
    VecFx32 vTan1;                  /* 0x24 */
    VecFx32 vFrom;                  /* 0x30 */
    VecFx32 vTo;                    /* 0x3c */
};

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

extern int FX_Div(int num, int den);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void VEC_Add(void *a, void *b, VecFx32 *d);
extern void VEC_Subtract(void *a, void *b, VecFx32 *d);
extern int Ov107_CollectSphereOverlaps(int owner, Sphere *query, int *out);
extern int VEC_Normalize(VecFx32 *a, VecFx32 *d);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const PosMsg data_ov213_020d2f44;
extern const PosMsg data_ov213_020d2f60;

static inline void PackFx24(Fx24 *dst, int v) {
    dst->hi = ((u32)v >> 16 & 0x7f) | ((u32)v >> 24 & 0x80);
    dst->mid = (u32)v >> 8;
    dst->lo = v;
}

static inline void SendPos(struct Ov213State *state, PosMsg *msg, const VecFx32 *src)
{
    Fx32 px;
    Fx32 py;
    Fx32 pz;
    px = *(Fx32 *)&src->x;
    PackFx24(&msg->pos[0], px.value);
    py = *(Fx32 *)&src->y;
    PackFx24(&msg->pos[1], py.value);
    pz = *(Fx32 *)&src->z;
    PackFx24(&msg->pos[2], pz.value);
    if (*(void (**)(int, PosMsg *, int))(state->pOwner + 0x24) != 0) {
        (*(void (**)(int, PosMsg *, int))(state->pOwner + 0x24))(state->pOwner, msg, 0xe);
    }
}

void Ov213_LeapTick(int *node)
{
    struct Ov213State *state = (struct Ov213State *)node[1];
    VecFx32 pos;
    VecFx32 term;
    int hits[4];
    Sphere sphere;
    VecFx32 push;
    PosMsg msg;
    PosMsg land;
    PosMsg tmpl;
    int t;
    int t2;
    long i;
    long n;
    int hit;
    int t2x3;
    int t3;

    state->nClock += *(int *)(*node + 0x2c);
    hit = 0;
    t = FX_Div(state->nClock, 0x2000);
    if (t >= 0x1000) {
        t = 0x1000;
    }
    t2 = FX_MUL(t, t);
    t3 = FX_MUL(t2, t);
    t2x3 = 3 * t2;
    ScaleVec3Fx12(2 * t3 - t2x3 + 0x1000, &state->vFrom, &pos);
    {
        int h01 = -2 * t3;
        h01 += t2x3;
        ScaleVec3Fx12(h01, &state->vTo, &term);
    }
    VEC_Add(&pos, &term, &pos);
    ScaleVec3Fx12(t + (t3 - 2 * t2), &state->vTan0, &term);
    VEC_Add(&pos, &term, &pos);
    ScaleVec3Fx12(t3 - t2, &state->vTan1, &term);
    VEC_Add(&pos, &term, &pos);
    VEC_Subtract(&pos, state->pPos, &state->vStep);
    sphere = *(Sphere *)(state->pOwner + 0x74);
    VEC_Add(&sphere, &state->vStep, (VecFx32 *)&sphere);
    n = Ov107_CollectSphereOverlaps(state->pOwner, &sphere, hits);
    i = 0;
    if (n > 0) {
        tmpl = data_ov213_020d2f44;
        do {
            VEC_Subtract((void *)(hits[i] + 0x74), &sphere, &push);
            if (push.y < 0) {
                push.y = 0;
            }
            VEC_Normalize(&push, &push);
            ScaleVec3Fx12(0x100, &push, &push);
            if (Ov107_InvokeHitCallback(hits[i], state->pOwner, *(int *)(state->pOwner + 0x384), 5, &push, 0) != 0) {
                msg = tmpl;
                SendPos(state, &msg, (VecFx32 *)(hits[i] + 0x74));
                hit = 1;
                Ov107_BuildAndSendUpdate(*(int *)(state->pOwner + 0x384), 0, 0x4f, (void *)(state->pOwner + 0x74));
            }
        } while (++i < n);
    }
    if (t < 0x1000 && ((struct b2 *)(state->pOwner + 0x17a))->b1 == 0 && hit == 0) {
        return;
    }
    land = data_ov213_020d2f60;
    SendPos(state, &land, (VecFx32 *)(state->pOwner + 0x74));
    if (hit == 0) {
        Ov107_BuildAndSendUpdate(*(int *)(state->pOwner + 0x384), 0x122, 0xd, (void *)(state->pOwner + 0x74));
    }
    *(u8 *)(state->pOwner + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
