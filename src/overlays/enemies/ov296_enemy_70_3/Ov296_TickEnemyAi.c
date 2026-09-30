/* Enemy AI tick. Raises bit 0 of the render component's flag word, then: if a query against the
 * target returns a hit whose state is absent or unflagged, walk toward the hit normal in Q12 steps
 * of 0x1200; otherwise ask for candidates in a 0x1200-radius sphere and steer toward the NEAREST
 * (VEC_Subtract + VEC_Mag seeded with 0x7fffffff), moving by 0x1200 - distance so it closes more
 * the further it is. With no candidates it either applies vDirection (when bFlags17a bit 0 is set)
 * or clears 0x48 of wFlags60 and gives up. Every non-giving-up path ends in SetIndexedSlot(task,
 * task->nIndex, 0). Q12 multiply throughout: ((s64)a * b + 0x800) >> 12. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/ai_task.h"

struct Vec4 {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
};

struct Sphere {
    VecFx32 center;
    s32 radius;
};

struct Component388 {
    char pad00[8];
    s32 flags08;
};

struct Transform {
    VecFx32 position;
    char pad0c[0x70];
    void *field7c;
};

struct Actor {
    void *node00;
    struct Transform *transform04;
    char pad08[0x58];
    u16 flags60;
    char pad62[0x3e];
    char fieldA0[0x84];
    VecFx32 direction124;
    char pad130[0x4a];
    u8 flags17a;
    char pad17b[0x20d];
    struct Component388 *component388;
};

struct Inner {
    struct Actor *actor00;
    VecFx32 *target04;
};

struct Task {
    AI_TASK_FIELDS(struct Inner)
};

struct HitInfo {
    char pad00[0x14];
    s16 x14;
    s16 y16;
    s16 z18;
};

struct HitState {
    char pad00[0x22];
    u16 flags22;
};

struct Hit {
    char pad00[4];
    struct HitInfo *info04;
    struct HitState *state08;
    void *field0c;
};

extern struct Vec4 data_02042264;

extern struct Hit *Collision_CastSimple(void *a, VecFx32 *b, VecFx32 *c, int d);
extern void ScaleVec3Fixed27(void *a, VecFx32 *b, VecFx32 *c);
extern void VEC_Normalize(VecFx32 *dst, VecFx32 *src);
extern int Ov107_MoveNodeAndRelayout(void *node, VecFx32 *pos);
extern int Quat_FromTwoVectors(struct Vec4 *out, struct Vec4 *m, VecFx32 *v);
extern int Srt_SetRotationQuat(void *dst, struct Vec4 *src);
extern void SetIndexedSlot(struct Task *obj, int idx, void *value);
extern int Ov107_QuerySphereContacts(void *a, struct Sphere *b, VecFx32 *outList, VecFx32 *outDir);
extern void VEC_Subtract(VecFx32 *a, VecFx32 *b, VecFx32 *out);
extern int VEC_Mag(VecFx32 *v);

struct StackLocals {
    VecFx32 diff;
    VecFx32 candidates[4];
    struct Sphere sphere;
    struct Vec4 matrix;
    VecFx32 direction;
    VecFx32 result;
    VecFx32 offset;
};

static inline s32 mul_round(s32 a, s32 b)
{
    return (s32)(((s64)a * b + 0x800) >> 12);
}

void Ov296_TickEnemyAi(struct Task *arg0)
{
    struct StackLocals s;
    struct Inner *inner;
    struct Component388 *component;
    struct Hit *hit;
    struct Transform *transform;
    int i;
    int count;
    int nearest;
    int distance;
    int scale;
    u16 flags;

    inner = arg0->pState;
    component = inner->actor00->component388;
    transform = inner->actor00->transform04;

    {
        s32 old = component->flags08;
        u32 low = (u32)(old << 24) >> 24;

        low |= 1;
        old &= ~0xff;
        low &= 0xff;

        component->flags08 = old | low;
    }

    s.offset.x = 0;
    s.offset.y = 0x1200;
    s.offset.z = 0;

    hit = Collision_CastSimple(transform->field7c, inner->target04, &s.offset, 0);

    if (hit != 0 &&
        (hit->state08 == 0 ||
         (hit->state08 != 0 && !(hit->state08->flags22 & 0xff)))) {
        ScaleVec3Fixed27(hit->field0c, &s.offset, &s.offset);

        s.direction.x = hit->info04->x14;
        s.direction.y = hit->info04->y16;
        s.direction.z = hit->info04->z18;

        VEC_Normalize(&s.direction, &s.direction);

        s.result.x = inner->target04->x + s.offset.x + mul_round(s.direction.x, 0x1200);
        s.result.y = inner->target04->y + s.offset.y + mul_round(s.direction.y, 0x1200);
        s.result.z = inner->target04->z + s.offset.z + mul_round(s.direction.z, 0x1200);

        Ov107_MoveNodeAndRelayout(inner->actor00, &s.result);
        Quat_FromTwoVectors(&s.matrix, &data_02042264, &s.direction);
        Srt_SetRotationQuat(&inner->actor00->fieldA0, &s.matrix);

        flags = inner->actor00->flags60;
        inner->actor00->flags60 = (u16)((flags & ~0xff00) |
            (((((u32)(flags << 16) >> 24) | 0x40) << 24) >> 16));

        flags = inner->actor00->flags60;
        inner->actor00->flags60 = (u16)((flags & ~0xff00) |
            (((u32)(u16)(((u32)(flags << 16) >> 24) & ~0x1e)) << 24) >> 16);

        SetIndexedSlot(arg0, arg0->slot, 0);
        return;
    }

    s.sphere.center = *inner->target04;
    s.sphere.radius = 0x1200;

    count = Ov107_QuerySphereContacts(transform->field7c, &s.sphere, s.candidates, &s.direction);

    if (count > 0) {
        nearest = 0x7fffffff;

        for (i = 0; i < count; i++) {
            VEC_Subtract(&s.sphere.center, &s.candidates[i], &s.diff);

            distance = VEC_Mag(&s.diff);

            if (distance < nearest) {
                nearest = distance;

                VEC_Normalize(&s.diff, &s.direction);
            }
        }

        scale = 0x1200 - nearest;

        s.result.x = inner->target04->x + mul_round(s.direction.x, scale);
        s.result.y = inner->target04->y + mul_round(s.direction.y, scale);
        s.result.z = inner->target04->z + mul_round(s.direction.z, scale);

        Ov107_MoveNodeAndRelayout(inner->actor00, &s.result);
        Quat_FromTwoVectors(&s.matrix, &data_02042264, &s.direction);
        Srt_SetRotationQuat(&inner->actor00->fieldA0, &s.matrix);

        flags = inner->actor00->flags60;
        inner->actor00->flags60 = (u16)((flags & ~0xff00) |
            (((((u32)(flags << 16) >> 24) | 0x40) << 24) >> 16));

        flags = inner->actor00->flags60;
        inner->actor00->flags60 = (u16)((flags & ~0xff00) |
            (((u32)(u16)(((u32)(flags << 16) >> 24) & ~0x1e)) << 24) >> 16);

        SetIndexedSlot(arg0, arg0->slot, 0);
        return;
    }

    if (((u32)(inner->actor00->flags17a << 31) >> 31) != 0) {
        Quat_FromTwoVectors(&s.matrix, &data_02042264, &inner->actor00->direction124);
        Srt_SetRotationQuat(&inner->actor00->fieldA0, &s.matrix);

        flags = inner->actor00->flags60;
        inner->actor00->flags60 = (u16)((flags & ~0xff00) |
            (((((u32)(flags << 16) >> 24) | 0x40) << 24) >> 16));

        SetIndexedSlot(arg0, arg0->slot, 0);
        return;
    }

    flags = inner->actor00->flags60;
    inner->actor00->flags60 = (u16)((flags & ~0xff00) |
        (((u32)(u16)(((u32)(flags << 16) >> 24) & ~0x48)) << 24) >> 16);
}
