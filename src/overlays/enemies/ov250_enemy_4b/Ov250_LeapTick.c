/* Leap tick of the ov250 enemy (and its byte-identical twin): the +0x54 velocity takes the
 * +0x78 direction, the +0x7c height drops by 30 x dt x 0xc0 / 4096 and the +0x1c/+0x6c clocks
 * accumulate the frame-time; past 0x1222 the +0x51 warning bit latches and reaction 0x159 mode 8
 * fires at the +4 anchor. Past 0x100, a 0x2120 sphere ahead of the +8 position by the velocity
 * offers a kind-2 hit pushed along the flattened direction at 0x1000 to every entity found: on
 * acceptance the entity's +0x190 point raised by 0x2000 is packed into the overlay's 14-byte
 * template for the actor's +0x24 message hook, reaction 0x159 mode 5 fires there and the
 * direction clears. Past 0x100 with bit 0 of +0x17a set, animation 0x10 plays, bit 6 of the
 * +0x60 high byte clears and the state hands off to d2440; otherwise a negative height plays
 * animation 0xf once (+0x50). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { VecFx32 pos; int radius; } Sphere;
struct hw60 { unsigned short lo : 8, hi : 8; };
struct Bits17a { u8 bit0 : 1, bit1 : 1; };
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
    VecFx32 *pAnchor;              /* +0x04 */
    VecFx32 *pPos;                 /* +0x08 */
    char pad00c[0x10];
    int nTimer;                 /* +0x1c */
    char pad020[0x30];
    u8 bLanded50;               /* +0x50 */
    u8 bit0 : 1;                /* +0x51 */
    u8 bWarned : 1;
    char pad052[2];
    VecFx32 vVelocity;             /* +0x54 */
    char pad060[0xc];
    int nClock;                 /* +0x6c */
    char pad070[8];
    int nDirX;                  /* +0x78 */
    int nHeight;                /* +0x7c */
    int nDirZ;                  /* +0x80 */
};

extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void Ov250_AiRollTimerQueue2(void);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov107_CollectSphereOverlaps(struct Ov250Owner *owner, Sphere *sphere, int *out);
extern int Ov107_InvokeHitCallback(int hit, struct Ov250Owner *a, struct Ov250Owner *b, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(struct Ov250Owner *owner, int a, int id, VecFx32 *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern const PosMsg data_ov250_020d28dc;

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))
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

void Ov250_LeapTick(int *node)
{
    struct Ov250SweepState *state = (struct Ov250SweepState *)node[1];
    Sphere sphere;
    VecFx32 push;
    int hits[4];
    VecFx32 at;
    PosMsg msg;
    PosMsg tmpl;
    int i;
    int n;

    state->vVelocity = *(VecFx32 *)&state->nDirX;
    state->nHeight -= FX_MUL(*(int *)(*node + 0x2c) * 30, 0xc0);
    state->nTimer += *(int *)(*node + 0x2c);
    state->nClock += *(int *)(*node + 0x2c);
    if (state->bWarned == 0 && state->nClock >= 0x1222) {
        state->bWarned = 1;
        Ov107_BuildAndSendUpdate(state->pOwner, 0x159, 8, state->pAnchor);
    }
    if (state->nTimer > 0x100) {
        VEC_Add(state->pPos, &state->vVelocity, &sphere.pos);
        sphere.radius = 0x2120;
        push.x = state->nDirX;
        push.y = 0;
        push.z = state->nDirZ;
        VEC_Normalize(&push, &push);
        ScaleVec3Fx12(0x1000, &push, &push);
        n = Ov107_CollectSphereOverlaps(state->pOwner, &sphere, hits);
        i = 0;
        if (n > 0) {
            tmpl = data_ov250_020d28dc;
            do {
                if (Ov107_InvokeHitCallback(hits[i], state->pOwner, state->pOwner, 2, &push, 0) != 0) {
                    at = *(VecFx32 *)(hits[i] + 0x190);
                    msg = tmpl;
                    at.y += 0x2000;
                    SendPos(state, &msg, &at);
                    Ov107_BuildAndSendUpdate(state->pOwner, 0x159, 5, &at);
                    state->nDirX = 0;
                    state->nDirZ = 0;
                }
            } while (++i < n);
        }
    }
    if (state->nTimer > 0x100 && ((struct Bits17a *)((char *)state->pOwner + 0x17a))->bit0 != 0) {
        Ov107_PostTagUpdate((Actor *)state->pOwner, 0x10, 0);
        ((struct hw60 *)((char *)state->pOwner + 0x60))->hi &= ~0x40;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov250_AiRollTimerQueue2);
        return;
    }
    if (state->bLanded50 != 0) {
        return;
    }
    if (state->nHeight >= 0) {
        return;
    }
    state->bLanded50 = 1;
    Ov107_PostTagUpdate((Actor *)state->pOwner, 0xf, 0);
}
