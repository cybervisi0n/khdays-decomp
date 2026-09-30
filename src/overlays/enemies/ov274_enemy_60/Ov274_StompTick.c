/* Stomp tick of the ov274 enemy. While the +0x24 timer (fed by the owner's rate) is between
 * 0x2aa and 0xc44 both feet are swept: the +0x3bc and then the +0x3c0 placement's sphere
 * (+0x68, radius x 1.5) collects the entities around it, each is pushed by 1.0 along the unit
 * direction from the placement's transform (kind 3) and, on acceptance, the overlay's 14-byte
 * template (data_ov274_020d423a for the first foot, data_ov274_020d421e for the second) carries
 * the entity's +0x74 position added to the transform and scaled 0.5 to the owner's +0x24
 * hook, where reaction 0x163 mode 0xc fires. Once the +0xc idle byte clears sub-state 0xa is
 * requested and the state ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { u8 hi, mid, lo; } Fx24;
typedef struct { int value; } Fx32;
typedef struct { VecFx32 pos; int nRadius; } Sphere;

typedef struct {
    u16 id;             /* +0x0 */
    u8 kind;            /* +0x2 */
    u8 cmd;             /* +0x3 */
    u8 flag;            /* +0x4 */
    Fx24 pos[3];        /* +0x5 */
} PosMsg;

struct Ov274State {
    int pOwner;                  /* 0x00 */
    VecFx32 *pPoint;                /* 0x04 */
    VecFx32 *pPos;                  /* 0x08 */
    u8 *pBusy;                   /* 0x0c */
    char pad10[0x14];
    int nTimer;                  /* 0x24 */
};

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

static inline void PackFx24(Fx24 *dst, int v) {
    dst->hi = ((u32)v >> 16 & 0x7f) | ((u32)v >> 24 & 0x80);
    dst->mid = (u32)v >> 8;
    dst->lo = v;
}

static inline void SendPos(struct Ov274State *state, PosMsg *msg, const VecFx32 *src)
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

extern int Ov107_CollectSphereOverlaps(int owner, Sphere *query, int *out);
extern void VEC_Subtract(void *a, void *b, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *a, VecFx32 *d);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void VEC_Add(void *a, void *b, VecFx32 *d);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const PosMsg data_ov274_020d423a;
extern const PosMsg data_ov274_020d421e;

static inline void SweepFoot(struct Ov274State *state, int nFoot, PosMsg tmpl, int *hits, Sphere *psphere)
{
    VecFx32 at;
    PosMsg msg;
    VecFx32 push;
    long i;
    long n;

    *psphere = *(Sphere *)(*(int *)(state->pOwner + nFoot) + 0x68);
    psphere->nRadius = FX_MUL(psphere->nRadius, 0x1800);
    n = Ov107_CollectSphereOverlaps(state->pOwner, psphere, hits);
    i = 0;
    if (n > 0) {
        do {
            VEC_Subtract((void *)(hits[i] + 0x74), (char *)*(int *)(state->pOwner + nFoot) + 4, &push);
            VEC_Normalize(&push, &push);
            ScaleVec3Fx12(0x1000, &push, &push);
            if (Ov107_InvokeHitCallback(hits[i], state->pOwner, state->pOwner, 3, &push, 0) != 0) {
                msg = tmpl;
                VEC_Add((void *)(hits[i] + 0x74), (char *)*(int *)(state->pOwner + nFoot) + 4, &at);
                ScaleVec3Fx12(0x800, &at, &at);
                SendPos(state, &msg, &at);
                Ov107_BuildAndSendUpdate(state->pOwner, 0x163, 0xc, &at);
            }
        } while (++i < n);
    }
}

void Ov274_StompTick(int *node)
{
    struct Ov274State *state = (struct Ov274State *)node[1];
    int hits[4];
    Sphere sphere;

    state->nTimer += *(int *)(*node + 0x2c);
    if (state->nTimer > 0x2aa && state->nTimer < 0xc44) {
        SweepFoot(state, 0x3bc, data_ov274_020d423a, hits, &sphere);
        SweepFoot(state, 0x3c0, data_ov274_020d421e, hits, &sphere);
    }
    if (*state->pBusy != 0) {
        return;
    }
    *(u8 *)(state->pOwner + 0x1c7) = 0xa;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
