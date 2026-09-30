/* Retreat decision of the ov252 actor: when an attack is queued (+0xa0) pose 3 and partner motion
 * 2 play and the node moves on to 020cfa28. Otherwise, with the +8 track more than 30.0 from the
 * origin or a clear line ahead along the +0x54 heading (swept sphere of the +0x80 radius), the
 * retreat mode (+0x579) comes from 020ce42c towards the origin and +0x18 becomes the home point
 * (3.375, 0, 7.25); far from the aim target (020cdfe8) with a +0x4e4 target it heads for that
 * target's +0x190 point instead; else the mode clears and 020cdef4 may end the node. A rider
 * (+0x78) forces mode 5. Mode 0 plays pose 2 / partner motion 1 and moves on to 020cf7d8; other
 * modes play pose 3m+1 (partner motion 3 or 6 for modes 1 / 2), clear +0x64 and move on to
 * 020cf6a0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { void *a; void *b; void *c; int d; } CollisionHit;

extern void Ov107_PostTagUpdate(int actor, int pose, int loop);
extern void Ov107_StartAnim(int part, int motion, int mode);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern CollisionHit *Collision_CastSphereEx(void *collision, VecFx32 *origin, VecFx32 *dir, int radius, void *ignore);
extern u8 Ov252_TurnSide(int *state, VecFx32 to);
extern int Ov252_CheckTarget(int *node, VecFx32 *to, int b);
extern int Ov252_PickMove(int *node);
extern void Ov252_SwaySettleTick(void);
extern void Ov252_SwayTurnTick(void);
extern void Ov252_SwayTick(void);
extern const short data_0203d210[];
extern const VecFx32 data_ov252_020d43a4;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

static inline void VecSet(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void Ov252_RetreatDecision(int *node)
{
    int *state = (int *)node[1];
    int item = *(int *)(*state + 4);
    VecFx32 toOrigin;
    VecFx32 fwd;
    VecFx32 home = data_ov252_020d43a4;
    CollisionHit *hit;
    unsigned int idx;
    int dist;

    if (state[0x28] != 0) {
        Ov107_PostTagUpdate(*state, 3, 0);
        Ov107_StartAnim(*(int *)(*state + 0x574), 2, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_SwaySettleTick);
        return;
    }
    VecSet(&toOrigin, -((VecFx32 *)state[2])->x, 0, -((VecFx32 *)state[2])->z);
    dist = VEC_Normalize(&toOrigin, &fwd);
    idx = ANG2IDX(state[0x15]);
    VecSet(&fwd, data_0203d210[idx * 2], 0, data_0203d210[idx * 2 + 1]);
    hit = Collision_CastSphereEx(*(void **)(item + 0x7c), (VecFx32 *)state[2], &fwd, *(int *)(*state + 0x80), 0);
    if ((hit != 0 && hit->c == 0) || dist > 0x1e000) {
        *(u8 *)(*state + 0x579) = Ov252_TurnSide(state, toOrigin);
        *(VecFx32 *)(state + 6) = home;
    } else if (Ov252_CheckTarget(node, &toOrigin, 0) > 0x1e000 && *(int *)(*state + 0x4e4) != 0) {
        *(u8 *)(*state + 0x579) = Ov252_TurnSide(state, toOrigin);
        *(VecFx32 *)(state + 6) = *(VecFx32 *)(*(int *)(*state + 0x4e4) + 0x190);
    } else {
        *(u8 *)(*state + 0x579) = 0;
        if (Ov252_PickMove(node) != 0) {
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
            return;
        }
    }
    if (state[0x1e] != 0) {
        *(u8 *)(*state + 0x579) = 5;
    }
    if (*(u8 *)(*state + 0x579) == 0) {
        *((u8 *)state + 0x84) = 2;
        Ov107_PostTagUpdate(*state, *((u8 *)state + 0x84), 0);
        Ov107_StartAnim(*(int *)(*state + 0x574), 1, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_SwayTurnTick);
        return;
    }
    *((u8 *)state + 0x84) = *(u8 *)(*state + 0x579) * 3 + 1;
    Ov107_PostTagUpdate(*state, *((u8 *)state + 0x84), 0);
    switch (*(u8 *)(*state + 0x579)) {
    case 1:
        Ov107_StartAnim(*(int *)(*state + 0x574), 3, 0);
        break;
    case 2:
        Ov107_StartAnim(*(int *)(*state + 0x574), 6, 0);
        break;
    }
    state[0x19] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_SwayTick);
}
