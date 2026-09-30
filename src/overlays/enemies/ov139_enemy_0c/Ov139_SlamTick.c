/* Slam tick of the ov139 enemy (and its byte-identical twin): the +0x14 velocity is the +0x390
 * part's motion step rotated by the actor's +0xa0 orientation and scaled by its speed. The first
 * time the part's +0xc motion goes negative with a zero speed, the overlay's 14-byte landing
 * message with the +0x48 point raised by 0x200 goes to the actor's +0x24 hook, bit 6 of the
 * +0x60 high byte clears, the +0x54 flag latches and reaction 0x11f mode 5 fires at the +0x4c
 * position. The +0x3c timer accumulates the frame-time; between 0xc44 and 0x1199 a box centred
 * 0x400 above the +0x48 point, axis-aligned, with half-extent (timer - 0xc44) / 0x555 times
 * 0x4000, pushes every entity whose +0x1b4 kind bit is clear in the +0x55 mask away on the
 * ground plane by 0x400 (kind 1); on acceptance the entity's +0x74 position raised by 0x800 is
 * packed into the overlay's 14-byte template for the +0x24 hook, the kind bit is set and
 * reaction 0 mode 0x53 fires there. Once the +0x50 busy byte clears with bit 0 of +0x17a or
 * +0x17c set, the +0x40 cooldown is re-armed at random between the actor's +0x224 and +0x228,
 * sub-state 2 is requested and the state ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { u8 hi, mid, lo; } Fx24;   /* sign + 23-bit magnitude, big-endian */
typedef struct { int value; } Fx32;
struct hw60 { unsigned short lo : 8, hi : 8; };
struct Bit0 { u8 bit0 : 1; };

typedef struct {
    u16 id;             /* +0x0 */
    u8 kind;            /* +0x2 */
    u8 cmd;             /* +0x3 */
    u8 flag;            /* +0x4 */
    Fx24 pos[3];        /* +0x5 */
} PosMsg;

typedef struct {
    u16 id;             /* +0x0 */
    u8 kind;            /* +0x2 */
    u8 cmd;             /* +0x3 */
} ShortMsg;

struct BoxQuery {
    VecFx32 vCenter;
    VecFx32 vAxisX;
    VecFx32 vAxisZ;
    VecFx32 vAxisY;
    int nExtent;
    int bFlag;
};

struct Ov204Owner {
    char pad000[0x24];
    void (*pfnMessage)(struct Ov204Owner *self, void *msg, int size);
};

struct Ov204SlamState {
    struct Ov204Owner *pOwner;  /* +0x00 */
    int pTarget;                /* +0x04 */
    char pad008[0xc];
    VecFx32 vVelocity;             /* +0x14 */
    char pad020[0x1c];
    int nTimer;                 /* +0x3c */
    int nCooldown;              /* +0x40 */
    char pad044[4];
    VecFx32 *pPoint;               /* +0x48 */
    VecFx32 *pPos;                 /* +0x4c */
    u8 *pBusy;                  /* +0x50 */
    u8 bLanded54;               /* +0x54 */
    u8 bHitMask55;              /* +0x55 */
};

extern int Ov107_ActionResource_GetOffsetAndScale(void *part, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void Ov107_BuildAndSendUpdate(struct Ov204Owner *owner, int a, int id, VecFx32 *at);
extern int FX_Div(int a, int b);
extern int Ov107_CollectEntitiesTouchingDisc(struct Ov204Owner *owner, struct BoxQuery *query, int *out);
extern void VEC_Subtract(void *a, void *b, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *a, VecFx32 *d);
extern int Ov107_InvokeHitCallback(int hit, struct Ov204Owner *a, struct Ov204Owner *b, int kind, VecFx32 *push, int z);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042258;
extern const VecFx32 data_02042264;
extern const PosMsg data_ov139_020cec2e;
extern const PosMsg data_ov139_020cec20;

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

static inline void PackFx24(Fx24 *dst, int v) {
    dst->hi = ((u32)v >> 16 & 0x7f) | ((u32)v >> 24 & 0x80);
    dst->mid = (u32)v >> 8;
    dst->lo = v;
}

static inline void SendPos(struct Ov204SlamState *state, PosMsg *msg, const VecFx32 *src)
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
    if (state->pOwner->pfnMessage != 0) {
        state->pOwner->pfnMessage(state->pOwner, msg, 0xe);
    }
}

void Ov139_SlamTick(int *node)
{
    struct Ov204SlamState *state = (struct Ov204SlamState *)node[1];
    VecFx32 step;
    VecFx32 landAt;
    PosMsg land;
    int hits[4];
    struct BoxQuery query;
    VecFx32 push;
    VecFx32 at;
    PosMsg msg;
    PosMsg tmpl;
    int speed;
    int t;
    long i;
    long n;
    int lo;
    int span;

    speed = Ov107_ActionResource_GetOffsetAndScale(*(void **)((char *)state->pOwner + 0x390), &step);
    Vec3TransformViaTempMtx(&state->vVelocity, (char *)state->pOwner + 0xa0, &step);
    ScaleVec3Fx12(speed, &state->vVelocity, &state->vVelocity);
    if (state->bLanded54 == 0 && *(int *)(*(int *)((char *)state->pOwner + 0x390) + 0xc) < 0 && speed == 0) {
        land = data_ov139_020cec2e;
        landAt = *state->pPoint;
        landAt.y += 0x200;
        SendPos(state, &land, &landAt);
        ((struct hw60 *)((char *)state->pOwner + 0x60))->hi &= ~0x40;
        state->bLanded54 = 1;
        Ov107_BuildAndSendUpdate(state->pOwner, 0x11f, 5, state->pPos);
    }
    state->nTimer += *(int *)(*node + 0x2c);
    if (state->nTimer >= 0xc44 && state->nTimer < 0x1199) {
        t = FX_Div(state->nTimer - 0xc44, 0x555);
        query.vCenter = *state->pPoint;
        query.vCenter.y += 0x400;
        query.vAxisX = data_02042270;
        query.vAxisZ = data_02042258;
        query.vAxisY = data_02042264;
        query.nExtent = FX_MUL(t, 0x4000);
        query.bFlag = 1;
        n = Ov107_CollectEntitiesTouchingDisc(state->pOwner, &query, hits);
        i = 0;
        if (n > 0) {
            tmpl = data_ov139_020cec20;
            do {
                if (((state->bHitMask55 >> *(u8 *)(hits[i] + 0x1b4)) & 1) == 0) {
                    VEC_Subtract((void *)(hits[i] + 0x74), &query.vCenter, &push);
                    push.y = 0;
                    VEC_Normalize(&push, &push);
                    ScaleVec3Fx12(0x400, &push, &push);
                    if (Ov107_InvokeHitCallback(hits[i], state->pOwner, state->pOwner, 1, &push, 0) != 0) {
                        msg = tmpl;
                        at = *(VecFx32 *)(hits[i] + 0x74);
                        at.y += 0x800;
                        SendPos(state, &msg, &at);
                        state->bHitMask55 |= 1 << *(u8 *)(hits[i] + 0x1b4);
                        Ov107_BuildAndSendUpdate(state->pOwner, 0, 0x53, &at);
                    }
                }
            } while (++i < n);
        }
    }
    if (*state->pBusy == 0) {
        if (((struct Bit0 *)((char *)state->pOwner + 0x17a))->bit0 != 0 || ((struct Bit0 *)((char *)state->pOwner + 0x17c))->bit0 != 0) {
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
}
