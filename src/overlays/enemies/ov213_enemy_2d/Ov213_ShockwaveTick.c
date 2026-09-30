/* Shockwave tick of the ov213 enemy: the +0x1c timer accumulates the frame rate. At 0xaaa (phase
 * 0 of +0x68) the ground note with the actor's position (raised to 0.125 above its +0x13c base)
 * goes out through the +0x24 hook and reaction 0x122/9 fires there. In phase 1, until 0x14cc, a
 * cylinder at the actor's feet (+0.5) grows its +0x60 radius a fifth of the way to 6.5 per tick
 * (swept at twice it) and every entity not yet in the +0x69 mask that accepts a kind-3 hit pushed
 * 1.0 outwards and 0.5 up gets the overlay's 14-byte message with its +0x74 position, its mask
 * bit and reaction 0/0x53 there. Once the +8 flag byte clears the next move is 6. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; VecFx32 axis[3]; int radius; int flag; } Cyl;
typedef struct { int value; } Fx32;
typedef struct { Fx32 x, y, z; } FxVec;
struct Msg14 { u16 h[7]; };

struct Ov213Actor {
    char pad000[0x24];
    void (*pfnMessage)(struct Ov213Actor *self, void *msg, int size);
};

extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern int Ov107_CollectEntitiesTouchingDisc(int owner, Cyl *cyl, int *hits);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const struct Msg14 data_ov213_020d2e90;
extern const struct Msg14 data_ov213_020d2e9e;
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042258;
extern const VecFx32 data_02042264;

#define PACK3(msg, base, v) \
    ((u8 *)(msg))[(base)] = (u8)(((u32)(v) >> 0x10 & 0x7f) | ((u32)(v) >> 0x18 & 0x80)); \
    ((u8 *)(msg))[(base) + 1] = (u8)((u32)(v) >> 8); \
    ((u8 *)(msg))[(base) + 2] = (u8)(v)

void Ov213_ShockwaveTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 at;
    struct Msg14 note;
    int hits[4];
    Cyl cyl;
    VecFx32 push;
    struct Msg14 msg;
    struct Msg14 tmpl;
    FxVec vAt;
    FxVec vHit;
    VecFx32 *pPos;
    int nHits;
    int i;

    state[7] += *(int *)(node[0] + 0x2c);
    if (*((u8 *)state + 0x68) == 0 && state[7] >= 0xaaa) {
        {
            note = data_ov213_020d2e90;
            at = *(VecFx32 *)(*state + 0x74);
            at.y = at.y - *(int *)(*state + 0x13c) + 0x200;
            vAt.x = *(Fx32 *)&at.x;
            PACK3(&note, 5, vAt.x.value);
            vAt.y = *(Fx32 *)&at.y;
            PACK3(&note, 8, vAt.y.value);
            vAt.z = *(Fx32 *)&at.z;
            PACK3(&note, 11, vAt.z.value);
            if (((struct Ov213Actor *)*state)->pfnMessage != 0) {
                ((struct Ov213Actor *)*state)->pfnMessage((struct Ov213Actor *)*state, &note, 0xe);
            }
            Ov107_BuildAndSendUpdate(*state, 0x122, 9, &at);
            *((u8 *)state + 0x68) = 1;
        }
    } else if (*((u8 *)state + 0x68) == 1 && state[7] <= 0x14cc) {
        cyl.pos = *(VecFx32 *)(*state + 0x74);
        cyl.pos.y = cyl.pos.y - *(int *)(*state + 0x13c) + 0x800;
        cyl.axis[0] = data_02042270;
        cyl.axis[1] = data_02042258;
        cyl.axis[2] = data_02042264;
        nHits = state[0x18];
        state[0x18] = nHits + (0x6800 - nHits) / 5;
        cyl.radius = state[0x18] * 2;
        cyl.flag = 1;
        nHits = Ov107_CollectEntitiesTouchingDisc(*state, &cyl, hits);
        i = 0;
        if (nHits > 0) {
            tmpl = data_ov213_020d2e9e;
            do {
                if ((*((u8 *)state + 0x69) & (1 << *(u16 *)(hits[i] + 2))) == 0) {
                    VEC_Subtract((void *)(hits[i] + 0x74), &cyl.pos, &push);
                    push.y = 0;
                    VEC_Normalize(&push, &push);
                    ScaleVec3Fx12(0x1000, &push, &push);
                    push.y = 0x800;
                    if (Ov107_InvokeHitCallback(hits[i], *state, *state, 3, &push, 0) != 0) {
                        msg = tmpl;
                        pPos = (VecFx32 *)(hits[i] + 0x74);
                        vHit.x = *(Fx32 *)&pPos->x;
                        PACK3(&msg, 5, vHit.x.value);
                        vHit.y = *(Fx32 *)&pPos->y;
                        PACK3(&msg, 8, vHit.y.value);
                        vHit.z = *(Fx32 *)&pPos->z;
                        PACK3(&msg, 11, vHit.z.value);
                        if (((struct Ov213Actor *)*state)->pfnMessage != 0) {
                            ((struct Ov213Actor *)*state)->pfnMessage((struct Ov213Actor *)*state, &msg, 0xe);
                        }
                        *((u8 *)state + 0x69) |= 1 << *(u16 *)(hits[i] + 2);
                        Ov107_BuildAndSendUpdate(*state, 0, 0x53, (void *)(hits[i] + 0x74));
                    }
                }
            } while (++i < nHits);
        }
    }
    if (*(u8 *)state[2] != 0) {
        return;
    }
    *(s8 *)(*state + 0x1c7) = 6;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
