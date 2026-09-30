/* Ground sweep of the ov204 enemy's charge (and its byte-identical twin): a box centred 0x200
 * above the given point, axis-aligned (right/forward/up), with half-extent t * 0x2000 for the
 * 32.32 fraction t (clamped to 1.0). Every entity whose +0x1b4 kind bit is clear in the +0x44
 * mask is pushed away on the ground plane by 0x800 (lifted by 0x100) through the ov107 checker
 * (kind 2); on acceptance its +0x74 position is packed into the overlay's 14-byte template for
 * the actor's +0x24 message hook, reaction 0x132 mode 4 fires there and the kind bit is set. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

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
    void (*pfnMessage)(struct Ov204Owner *self, PosMsg *msg, int size);
};

struct Ov204ChargeState {
    struct Ov204Owner *pOwner;  /* +0x00 */
    char pad004[0x40];
    u8 bHitMask44;              /* +0x44 */
};

extern int Ov107_CollectEntitiesTouchingDisc(struct Ov204Owner *owner, struct BoxQuery *query, int *out);
extern void VEC_Subtract(void *a, void *b, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *a, VecFx32 *d);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern int Ov107_InvokeHitCallback(int hit, struct Ov204Owner *a, struct Ov204Owner *b, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(struct Ov204Owner *owner, int a, int id, VecFx32 *at);
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042258;
extern const VecFx32 data_02042264;
extern const PosMsg data_ov204_020d367c;

static inline void PackFx24(Fx24 *dst, int v) {
    dst->hi = ((u32)v >> 16 & 0x7f) | ((u32)v >> 24 & 0x80);
    dst->mid = (u32)v >> 8;
    dst->lo = v;
}

static inline void SendPos(struct Ov204ChargeState *state, PosMsg *msg, const VecFx32 *src)
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

void Ov204_GroundSweep(struct Ov204ChargeState *state, s64 t, const VecFx32 *at)
{
    int hits[4];
    struct BoxQuery query;
    VecFx32 push;
    VecFx32 pos;
    PosMsg msg;
    PosMsg tmpl;
    long i;
    long n;

    if (t >= 0x100000000LL) {
        t = 0x100000000LL;
    }
    query.vCenter = *at;
    query.vCenter.y += 0x200;
    query.vAxisX = data_02042270;
    query.vAxisZ = data_02042258;
    query.vAxisY = data_02042264;
    query.nExtent = (int)((t * 0x2000 + 0x80000000LL) >> 32);
    query.bFlag = 1;
    n = Ov107_CollectEntitiesTouchingDisc(state->pOwner, &query, hits);
    i = 0;
    if (n > 0) {
        tmpl = data_ov204_020d367c;
        do {
            if (((state->bHitMask44 >> *(u8 *)(hits[i] + 0x1b4)) & 1) == 0) {
                VEC_Subtract((void *)(hits[i] + 0x74), &query.vCenter, &push);
                push.y = 0;
                VEC_Normalize(&push, &push);
                push.y = 0x100;
                ScaleVec3Fx12(0x800, &push, &push);
                if (Ov107_InvokeHitCallback(hits[i], state->pOwner, state->pOwner, 2, &push, 0) != 0) {
                    msg = tmpl;
                    pos = *(VecFx32 *)(hits[i] + 0x74);
                    SendPos(state, &msg, &pos);
                    Ov107_BuildAndSendUpdate(state->pOwner, 0x132, 4, &pos);
                    state->bHitMask44 |= 1 << *(u8 *)(hits[i] + 0x1b4);
                }
            }
        } while (++i < n);
    }
}
