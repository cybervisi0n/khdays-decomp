/* Lunge tick of the ov250 enemy (and its byte-identical twin): the +0x54 velocity is the facing
 * of the +0x14 yaw at the +0x70 speed, which drops by 30 x dt / 256 each tick; the +0x1c timer
 * accumulates the frame-time and, between 0xc00 and 0x1280, a 0x1600 sphere at the +0x3a4
 * item's +0x14 point offers a kind-1 hit pushed along the facing at 0x800 to every entity found:
 * on acceptance the sphere centre plus the push is packed into the overlay's 14-byte template
 * for the actor's +0x24 message hook and reaction 0x159 mode 5 fires there. Once the +0xc busy
 * byte clears the +0x74 cooldown is re-armed at random between the actor's +0x224 and +0x228,
 * sub-state 2 is requested and the state ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { VecFx32 pos; int radius; } Sphere;
typedef struct { u8 hi, mid, lo; } Fx24;   /* sign + 23-bit magnitude, big-endian */
typedef struct { int value; } Fx32;
typedef struct { Fx32 x, y, z; } FxVec;

typedef struct {
    u16 id;             /* +0x0 */
    u8 kind;            /* +0x2 */
    u8 cmd;             /* +0x3 */
    u8 flag;            /* +0x4 */
    Fx24 pos[3];        /* +0x5 */
} PosMsg;

struct Ov250Owner {
    char pad000[0x24];
    void (*pfnMessage)(struct Ov250Owner *self, PosMsg *msg, int size);
};

struct Ov250SweepState {
    struct Ov250Owner *pOwner;  /* +0x00 */
    int pTarget;                /* +0x04 */
    char pad008[4];
    u8 *pBusy;                  /* +0x0c */
    char pad010[4];
    int nYaw;                   /* +0x14 */
    char pad018[4];
    int nTimer;                 /* +0x1c */
    char pad020[0x34];
    VecFx32 vVelocity;             /* +0x54 */
    char pad060[0x10];
    int nSpeed;                 /* +0x70 */
    int nCooldown;              /* +0x74 */
};

extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov107_CollectSphereOverlaps(struct Ov250Owner *owner, Sphere *sphere, int *out);
extern int Ov107_InvokeHitCallback(int hit, struct Ov250Owner *a, struct Ov250Owner *b, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(struct Ov250Owner *owner, int a, int id, VecFx32 *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern const PosMsg data_ov250_020d28c0;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

static inline void PackFx24(Fx24 *dst, int v) {
    dst->hi = ((u32)v >> 16 & 0x7f) | ((u32)v >> 24 & 0x80);
    dst->mid = (u32)v >> 8;
    dst->lo = v;
}

static inline void SendPos(struct Ov250SweepState *state, PosMsg *msg, const VecFx32 *src)
{
    FxVec vDead;
    vDead.x = *(Fx32 *)&src->x;
    PackFx24(&msg->pos[0], vDead.x.value);
    vDead.y = *(Fx32 *)&src->y;
    PackFx24(&msg->pos[1], vDead.y.value);
    vDead.z = *(Fx32 *)&src->z;
    PackFx24(&msg->pos[2], vDead.z.value);
    if (state->pOwner->pfnMessage != 0) {
        state->pOwner->pfnMessage(state->pOwner, msg, 0xe);
    }
}

void Ov250_LungeTick(int *node)
{
    struct Ov250SweepState *state = (struct Ov250SweepState *)node[1];
    VecFx32 facing;
    VecFx32 push;
    Sphere sphere;
    int hits[4];
    VecFx32 at;
    PosMsg msg;
    PosMsg tmpl;
    unsigned int idx;
    int i;
    int n;
    int lo;
    int span;

    idx = ANG2IDX(state->nYaw);
    state->vVelocity.x = data_0203d210[idx * 2];
    state->vVelocity.y = 0;
    state->vVelocity.z = data_0203d210[idx * 2 + 1];
    ScaleVec3Fx12(state->nSpeed, &state->vVelocity, &state->vVelocity);
    state->nSpeed -= (int)(((long long)(*(int *)(*node + 0x2c) * 30) * 0x10 + 0x800) >> 12);
    state->nTimer += *(int *)(*node + 0x2c);
    if (state->nTimer >= 0xc00 && state->nTimer < 0x1280) {
        idx = ANG2IDX(state->nYaw);
        facing.x = data_0203d210[idx * 2];
        facing.y = 0;
        facing.z = data_0203d210[idx * 2 + 1];
        ScaleVec3Fx12(0x800, &facing, &push);
        sphere.pos = *(VecFx32 *)(*(int *)((char *)state->pOwner + 0x3a4) + 0x14);
        sphere.radius = 0x1600;
        n = Ov107_CollectSphereOverlaps(state->pOwner, &sphere, hits);
        i = 0;
        if (n > 0) {
            tmpl = data_ov250_020d28c0;
            do {
                if (Ov107_InvokeHitCallback(hits[i], state->pOwner, state->pOwner, 1, &push, 0) != 0) {
                    msg = tmpl;
                    VEC_Add(&sphere.pos, &push, &at);
                    SendPos(state, &msg, &at);
                    Ov107_BuildAndSendUpdate(state->pOwner, 0x159, 5, &at);
                }
            } while (++i < n);
        }
    }
    if (*state->pBusy == 0) {
        lo = *(int *)((char *)state->pOwner + 0x224);
        span = *(int *)((char *)state->pOwner + 0x228) - lo;
        if (span < 0) {
            span = -span;
        }
        state->nCooldown = lo + RandNextScaled(span + 1);
        *(u8 *)((char *)state->pOwner + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
    }
}
