/* Per-frame tick of a thrown weapon, shared by ov287, ov288 and ov289. Before launch it normalises
 * the aim, gives the throw unit speed and a 0x300 rise, then walks every other actor picking the
 * nearest one inside a 0.866 cosine cone; for that target it sets the planned distance to the range
 * over 1638 and integrates a per-step fall into the drop accumulator, clamping the plan to 0xf000,
 * and marks itself launched. After launch it aborts on a blocked sphere cast, on a blocked ray cast
 * once half the plan is behind it, on any overlap with another actor's parts, or when the thrower's
 * own slot list stops naming it; otherwise it walks the arc by sampling the sine table at the
 * fraction of the plan travelled, and once the plan is spent it just applies a constant fall.
 * Aborting zeroes the velocity, sets the actor's state byte to 4 and re-registers the node. */

/*
 * Ov287_Throw_Tick -- Ov287_Throw_Tick.
 *
 * Per-frame tick of a thrown weapon, shared with ov288 and ov289. The AI node's +4 points at
 * the throw state; the scene at +0 supplies the frame delta at its +0x2c.
 *
 * Before launch it normalises the aim, gives the throw unit speed and a 0x300 rise, then walks
 * every other actor picking the nearest one inside a 0.866 cosine cone. For that target it sets
 * the planned distance to the range over 1638 and integrates a per-step fall into the drop
 * accumulator, clamps the plan to 0xf000, and marks itself launched.
 *
 * After launch it aborts on a blocked sphere cast, on a blocked ray cast once half the plan is
 * behind it, on any overlap with another actor's parts, or when the thrower's own slot list
 * stops naming it. Otherwise it walks the arc by sampling the sine table at the fraction of the
 * plan travelled, and once the plan is spent it just applies a constant fall. Aborting zeroes
 * the velocity, sets the actor's state byte to 4 and re-registers the node.
 *
 * Ghidra: Ov287_Throw_Tick(AiStateNode *pNode), types /khdays/ThrowState and
 * /khdays/AiStateNode. The offsets left as raw casts here belong to the actor and world types
 * this function does not establish: the flag byte at +0x17a, the collision world at +0x7c, the
 * actor list at +0x80, the collider at +0x74, the parts list at +0x22c, the ignore handle at
 * +0x390, the counter at +0x394 and the state byte at +0x1c7.
 *
 * Byte-exact codegen notes (mwccarm 3.0/139). This is a 372-instruction function and six
 * decisions carried it:
 *  - The whole 0x54 frame is ONE struct. Separate locals never reproduce the original's
 *    layout, because mwcc does not order them by declaration; as one object the member offsets
 *    become the frame offsets.
 *  - Consequently every access shares one base register, which is wrong for the slot array.
 *    Reading it as ((void **)&f.tail)[i] breaks that fold and restores the original's
 *    add r2,sp,#0x38 with ldr r1,[r2,r3,lsl #2]. That single change was worth 171 positions.
 *  - The gravity path is laid out FIRST, so it is written as an early exit rather than as the
 *    else of the arc.
 *  - The halfword at +0x60 is a bitfield pair, not a u8 cast: the original narrows with
 *    lsl 0x18 then lsr 0x18 where a cast emits and #0xff.
 *  - The aim components go through named temporaries so the two loads batch ahead of the two
 *    stores, and the list-node unwrap is spelled ln == 0 ? 0 : ln->item.
 *  - Last, the arc guard is written positively, `if (planned != 0) {...} else { s = 0; }`.
 *    Written the other way round the scheduler fills the umull latency slot with the rounding
 *    add and emits adds before mla inside the first fixed-point multiply. Nothing about the
 *    multiply's own spelling reaches that: a long long product variable, the constant first,
 *    LL suffixes, unsigned rounding, a split shift, the multiplier or the rounding value in a
 *    local, and a static inline helper all compile to the same bytes.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/actor.h"
#include "game/ai_task.h"
#include "game/enemy_common.h"
#include "game/engine.h"

struct SinCos {
    short sin;
    short cos;
};

struct Hit {
    char pad000[8];
    int blocked;
};

struct ListNode {
    void *item;
};

struct ThrowState {
    void *pActor;
    char pad004[4];
    void *pThrower;
    VecFx32 *pPos;
    VecFx32 vPrevPos;
    VecFx32 vVelocity;
    int nAimX;
    char pad02c[4];
    int nAimZ;
    char pad034[4];
    int nTravelled;
    int nPlanned;
    int nLastSin;
    int nDrop;
    int nBaseVelY;
    char pad04c[0xc];
    int nLaunched;
};

struct FrameTail {
    void *slots[4];
    VecFx32 delta;
};

struct Frame {
    VecFx32 fwd;
    VecFx32 flat;
    VecFx32 toTarget;
    VecFx32 aim;
    struct FrameTail tail;
};

struct AiStateNode {
    AI_TASK_FIELDS(struct ThrowState)
};

extern void VEC_Subtract(VecFx32 *a, VecFx32 *b, VecFx32 *ab);
extern int VEC_DotProduct(VecFx32 *a, VecFx32 *b);
extern int Ov107_CollectSphereOverlaps(void *actor, void *shape, void **out);
extern struct Hit *Collision_CastSphereEx(void *world, VecFx32 *from, VecFx32 *delta, int radius,
                                 void *ignore);
extern struct Hit *Collision_CastRay(void *world, VecFx32 *from, VecFx32 *delta);
extern struct ListNode *List_First(void *list);
extern int Ov107_HitShape_TestSphere(void *shape, void *other, int mode);
extern int FX_Div(int a, int b);
extern int VEC_Normalize(VecFx32 *out, VecFx32 *in);
extern void ScaleVec3Fx12(int scale, VecFx32 *in, VecFx32 *out);
extern int FX_Inv(int a);
extern void SetIndexedSlot(struct AiStateNode *node, int slot, void *arg);

extern const struct SinCos data_0203d210[];
extern const VecFx32 data_02042258;
extern const VecFx32 data_02041dc8;

void Ov287_Throw_Tick(struct AiStateNode *node)
{
    struct ThrowState *st;
    void *actor;
    void *world;
    Actor *obj;
    struct ListNode *ln;
    struct Hit *hit;
    struct Frame f;
    int i;
    int n;
    int best;
    int az;
    int ax;
    int mag;
    int dot;
    int step;
    int acc;
    int s;
    int t;
    int u;

    st = node->pState;
    VEC_Subtract(st->pPos, &st->vPrevPos, &f.tail.delta);
    st->vPrevPos = *st->pPos;
    if (st->nLaunched != 0) {
        actor = st->pActor;
        world = *(void **)((char *)actor + 4);
        if ((u32)((u32)*(u8 *)((char *)actor + 0x17a) << 0x1c) >> 0x1f) {
            goto abort;
        }
        n = Ov107_CollectSphereOverlaps(actor, (char *)actor + 0x74, f.tail.slots);
        for (i = 0; i < n; i++) {
            if (((void **)&f.tail)[i] != st->pThrower) {
                goto abort;
            }
        }
        hit = Collision_CastSphereEx(*(void **)((char *)world + 0x7c), st->pPos, &f.tail.delta, 0x300,
                            *(void **)((char *)st->pActor + 0x390));
        if (hit != 0 && hit->blocked == 0) {
            goto abort;
        }
        if (st->nPlanned <= 0 || st->nTravelled >= st->nPlanned / 2) {
            hit = Collision_CastRay(*(void **)((char *)world + 0x7c), st->pPos, &f.tail.delta);
            if (hit != 0 && hit->blocked == 0) {
                goto abort;
            }
        }
        ln = List_First((char *)world + 0x80);
        obj = ln == 0 ? 0 : (Actor *)ln->item;
        while (obj != 0) {
            if (obj != (Actor *)st->pActor && (obj->flags60.bits.lo & 1) != 0 &&
                (obj->field_1ac & 3) == 0 &&
                ((obj->field_1ac & 4) == 0 || (obj->flags & 0x8000) != 0)) {
                ln = List_First((char *)obj + 0x22c);
                while (ln != 0) {
                    if (Ov107_HitShape_TestSphere(ln->item, (char *)st->pActor + 0x74, 0) != 0) {
                        goto abort;
                    }
                    ln = (struct ListNode *)List_Next((char *)obj + 0x22c);
                }
            }
            ln = (struct ListNode *)List_Next((char *)world + 0x80);
            obj = ln == 0 ? 0 : (Actor *)ln->item;
        }
        if (st->nPlanned <= 0 || st->nTravelled > st->nPlanned) {
            st->vVelocity.y = st->vVelocity.y + *(int *)((char *)node->pList + 0x2c) * -0x60 / 136;
            return;
        }
        {
            if (st->nPlanned != 0) {
                t = FX_Div(st->nTravelled, st->nPlanned);
                u = (int)(((long long)t * 12868 + 0x800) >> 12);
                s = data_0203d210[(u16)(int)(((long long)u * 0x28be60db9391LL +
                                              0x80000000000LL) >> 44) >> 4].sin;
            } else {
                s = 0;
            }
            st->vVelocity.y = st->nBaseVelY +
                        (int)(((long long)(st->nDrop + 0x1800) * (s - st->nLastSin) + 0x800) >> 12);
            st->nLastSin = s;
            st->nTravelled = *(int *)((char *)node->pList + 0x2c) * 0x1e + st->nTravelled;
            return;
        }
    }
    if (st->pThrower == 0) {
        return;
    }
    world = *(void **)((char *)st->pActor + 4);
    az = st->nAimZ;
    ax = st->nAimX;
    f.aim.x = ax;
    f.aim.z = az;
    best = 0x7fffffff;
    f.aim.y = 0;
    if (VEC_Normalize(&f.aim, &f.aim) == 0) {
        goto abort;
    }
    ScaleVec3Fx12(0x666, &f.aim, &st->vVelocity);
    st->vVelocity.y = 0x300;
    st->nDrop = 0;
    st->nLastSin = 0;
    st->nPlanned = 0;
    st->nTravelled = 0;
    ln = List_First((char *)world + 0x80);
    obj = ln == 0 ? 0 : (Actor *)ln->item;
    if (obj != 0) {
        f.fwd = data_02042258;
        do {
            if (obj != (Actor *)st->pActor && (obj->flags60.bits.lo & 1) != 0 &&
                (obj->field_1ac & 3) == 0 &&
                ((obj->field_1ac & 4) == 0 || (obj->flags & 0x8000) != 0)) {
                VEC_Subtract(&obj->sphere.center, (VecFx32 *)((char *)st->pActor + 0x74), &f.toTarget);
                f.flat.x = f.toTarget.x;
                f.flat.y = 0;
                f.flat.z = f.toTarget.z;
                if (VEC_Normalize(&f.flat, &f.flat) == 0) {
                    f.flat = f.fwd;
                }
                mag = VEC_Normalize(&f.toTarget, &f.toTarget);
                if (mag == 0) {
                    f.toTarget = f.fwd;
                }
                dot = VEC_DotProduct(&f.aim, &f.flat);
                if (mag < best && dot > 0xddb) {
                    ScaleVec3Fx12(0x666, &f.toTarget, &st->vVelocity);
                    st->nPlanned = mag / 1638;
                    st->nBaseVelY = st->vVelocity.y;
                    i = 0;
                    st->nDrop = 0;
                    acc = 0;
                    if (st->nPlanned / 2 > 0) {
                        do {
                            step = FX_Inv(
                                *(int *)((char *)Ov107_GetActorManager() + 0x40));
                            acc = acc + (int)(((long long)step * -96LL + 0x800) >> 12);
                            st->nDrop = st->nDrop - acc;
                            i++;
                        } while (i < st->nPlanned / 2);
                    }
                    st->nPlanned = st->nPlanned << 12;
                    best = mag;
                }
            }
            ln = (struct ListNode *)List_Next((char *)world + 0x80);
            obj = ln == 0 ? 0 : (Actor *)ln->item;
        } while (obj != 0);
    }
    if (st->nPlanned != 0 && st->nPlanned < 0xf000) {
        ScaleVec3Fx12(FX_Div(st->nPlanned, 0xf000), &st->vVelocity, &st->vVelocity);
        st->nBaseVelY = st->vVelocity.y;
        st->nPlanned = 0xf000;
    }
    *(int *)((char *)st->pActor + 0x394) = 0;
    st->nLaunched = 1;
    return;

abort:
    st->vVelocity = data_02041dc8;
    *(u8 *)((char *)st->pActor + 0x1c7) = 4;
    SetIndexedSlot(node, node->slot, 0);
}
