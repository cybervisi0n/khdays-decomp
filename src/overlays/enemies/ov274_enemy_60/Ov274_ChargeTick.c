/* Charge tick of the ov274 enemy. The +0x14 velocity is the sine/cosine of the +0x40 heading
 * (y 0) normalised and scaled 0x800 (after a 0x200 scaling of the raw pair); the entities
 * inside 0x1d00 of the +8 point are swept and each one ahead of the velocity (positive dot
 * product of its offset from the +8 point) is tested along the velocity (kind 0): on
 * acceptance the overlay's 14-byte template (data_ov274_020d4256) carries the entity's +0x74
 * position added to the +0x3b0 body's transform and scaled 0x800 to the owner's
 * +0x24 hook and reaction 0x163 mode 6 fires at the +8 point; every accepted or rejected
 * candidate bumps the +0x52 count. Four hits end the charge (animation 5 mode 0, hand-over to
 * Ov274_AiQueue2OnFlagClear); otherwise the +0x20 step counter fires reaction 0x163 mode 4 at 5 and
 * mode 5 at 10 (wrapping), and the +0x24 timer, fed by the owner's rate, ends the charge the
 * same way past 3.0 or once the owner reports a wall (+0x17a bit 1). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

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
    char pad10[4];
    VecFx32 vVelocity;              /* 0x14 */
    int nStep;                   /* 0x20 */
    int nTimer;                  /* 0x24 */
    char pad28[0x18];
    int nHeading;                /* 0x40 */
    char pad44[0xe];
    u8 nHits52;                  /* 0x52 */
};

struct Bit1 { u8 bit0 : 1, bit1 : 1; };

static inline unsigned short FX_RadToIdx(int rad) {
    return (unsigned short)((0x28BE60DB9391LL * rad + 0x80000000000LL) >> 44);
}

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

extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *a, VecFx32 *d);
extern int Ov107_CollectSphereOverlaps(int owner, Sphere *query, int *out);
extern void VEC_Subtract(void *a, void *b, VecFx32 *d);
extern int VEC_DotProduct(VecFx32 *a, VecFx32 *b);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *d);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern const PosMsg data_ov274_020d4256;
extern void Ov274_AiQueue2OnFlagClear(int *node);

void Ov274_ChargeTick(int *node)
{
    struct Ov274State *state = (struct Ov274State *)node[1];
    Sphere query;
    VecFx32 dir;
    int hits[4];
    VecFx32 d;
    PosMsg msg;
    VecFx32 at;
    PosMsg tmpl;
    long n;
    long i;

    dir.x = data_0203d210[(FX_RadToIdx(state->nHeading) >> 4) * 2];
    dir.y = 0;
    dir.z = data_0203d210[(FX_RadToIdx(state->nHeading) >> 4) * 2 + 1];
    ScaleVec3Fx12(0x200, &dir, &state->vVelocity);
    VEC_Normalize(&dir, &dir);
    ScaleVec3Fx12(0x800, &dir, &dir);
    query.pos = *state->pPos;
    query.nRadius = 0x1d00;
    n = Ov107_CollectSphereOverlaps(state->pOwner, &query, hits);
    i = 0;
    if (n > 0) {
        tmpl = data_ov274_020d4256;
        do {
            VEC_Subtract((void *)(hits[i] + 0x74), state->pPos, &d);
            if (VEC_DotProduct(&d, &dir) > 0) {
                if (Ov107_InvokeHitCallback(hits[i], state->pOwner, state->pOwner, 0, &dir, 0) != 0) {
                    msg = tmpl;
                    VEC_Add((VecFx32 *)(hits[i] + 0x74), (VecFx32 *)(*(int *)(*(int *)(state->pOwner + 0x3b0)) + 4), &at);
                    ScaleVec3Fx12(0x800, &at, &at);
                    SendPos(state, &msg, &at);
                    Ov107_BuildAndSendUpdate(state->pOwner, 0x163, 6, state->pPos);
                }
                state->nHits52++;
            }
        } while (++i < n);
    }
    if (state->nHits52 >= 4) {
        Ov107_PostTagUpdate((Actor *)state->pOwner, 5, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov274_AiQueue2OnFlagClear);
        return;
    }
    state->nStep++;
    if (state->nStep == 5) {
        Ov107_BuildAndSendUpdate(state->pOwner, 0x163, 4, state->pPos);
    } else if (state->nStep == 0xa) {
        Ov107_BuildAndSendUpdate(state->pOwner, 0x163, 5, state->pPos);
        state->nStep = 0;
    }
    state->nTimer += *(int *)(*node + 0x2c);
    if (state->nTimer < 0x3000 && ((struct Bit1 *)(state->pOwner + 0x17a))->bit1 == 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)state->pOwner, 5, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov274_AiQueue2OnFlagClear);
}
