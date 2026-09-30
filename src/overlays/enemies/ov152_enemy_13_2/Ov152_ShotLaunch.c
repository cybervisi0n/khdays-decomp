/* Shot launch of the ov151 enemy (and its byte-identical twin): clears the +0x38 hit count, sets
 * bit 0 of the owner's +0x60 high byte and of the +0x388 item's +8 flags, clears bits 0x8e of
 * the +0x60 high byte, and gives the +0xc velocity the forward axis rotated by the +0x18
 * orientation at the +0x28 speed (0x400) with the +0x34 travel reset. If the ray from the
 * +0x38c item's +0x74 point to the +8 position hits the scene collision within 0x200, the
 * overlay's 14-byte message goes to the owner's +0x24 hook with that position, reaction 0x14f
 * mode 6 fires there, sub-state 0 is requested and the state ends; otherwise the shot tick
 * (cd3ac) takes over. */

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

struct Ov151Owner {
    char pad000[0x24];
    void (*pfnMessage)(struct Ov151Owner *self, PosMsg *msg, int size);
};

struct hw60 { unsigned short lo : 8, hi : 8; };
struct b8 { unsigned int b : 8; };

struct Ov151ShotState {
    struct Ov151Owner *pOwner;  /* +0x00 */
    int pTarget;                /* +0x04 */
    VecFx32 *pPos;                 /* +0x08 */
    VecFx32 vVelocity;             /* +0x0c */
    char pad018[0x10];
    int nSpeed;                 /* +0x28 */
    char pad02c[8];
    int nTravel;                /* +0x34 */
    int nHits;                  /* +0x38 */
};

extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int Collision_CastSphere(void *collision, void *from, VecFx32 *dir, int radius);
extern void Ov107_BuildAndSendUpdate(struct Ov151Owner *owner, int a, int id, VecFx32 *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov152_ShotTick(void);
extern const VecFx32 data_02042258;
extern const PosMsg data_ov152_020d648a;

static inline void PackFx24(Fx24 *dst, int v) {
    dst->hi = ((u32)v >> 16 & 0x7f) | ((u32)v >> 24 & 0x80);
    dst->mid = (u32)v >> 8;
    dst->lo = v;
}

static inline void SendPos(struct Ov151ShotState *state, PosMsg *msg, const VecFx32 *src)
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

void Ov152_ShotLaunch(int *node)
{
    struct Ov151ShotState *state = (struct Ov151ShotState *)node[1];
    char *scene = *(char **)((char *)state->pOwner + 4);
    VecFx32 d;
    PosMsg msg;

    state->nHits = 0;
    {
        u16 hw = *(u16 *)((char *)state->pOwner + 0x60);
        *(u16 *)((char *)state->pOwner + 0x60) =
            (u16)((hw & ~0xff00) | (((((unsigned int)hw << 0x10) >> 0x18 | 1) << 0x18) >> 0x10));
    }
    ((struct b8 *)(*(int *)((char *)state->pOwner + 0x388) + 8))->b |= 1;
    ((struct hw60 *)((char *)state->pOwner + 0x60))->hi &= ~0x8e;
    state->nSpeed = 0x400;
    Vec3TransformViaTempMtx(&state->vVelocity, (char *)state + 0x18, &data_02042258);
    ScaleVec3Fx12(state->nSpeed, &state->vVelocity, &state->vVelocity);
    state->nTravel = 0;
    VEC_Subtract(state->pPos, (void *)(*(int *)((char *)state->pOwner + 0x38c) + 0x74), &d);
    if (scene != 0 && Collision_CastSphere(*(void **)(scene + 0x7c), (void *)(*(int *)((char *)state->pOwner + 0x38c) + 0x74), &d, 0x200) != 0) {
        msg = data_ov152_020d648a;
        SendPos(state, &msg, state->pPos);
        Ov107_BuildAndSendUpdate(state->pOwner, 0x14f, 6, state->pPos);
        *(u8 *)((char *)state->pOwner + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov152_ShotTick);
}
