

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/actor.h"
#include "game/ai_task.h"
#include "game/enemy_common.h"
#include "game/engine.h"

struct Sphere {
    VecFx32 centre;
    int radius;
};

struct ListNode {
    void *item;
};

struct HitPacket {
    u32 flags00;
    VecFx32 normal;
    u32 field10;
    u32 field14;
    void *pPart;
    u32 tail[4];
};

struct State {
    void *pActor;
    void *pOwner;
    void *pTarget;
    char pad00c[0x28];
    int timer034;
    char pad038[0x14];
    unsigned long long mask;
    u8 sent054;
    char pad055[3];
};

struct Node {
    AI_TASK_FIELDS(struct State)
};

struct Msg {
    u16 h[7];
};

/* Coordinates are held in a one-value wrapper type (Fx32), a tentative
 * reconstruction of the original's coordinate type: copying a wrapped value is a
 * struct copy, which mwcc keeps, and that is the ROM's unread stack copy. */
typedef struct { int value; } Fx32;
typedef struct { Fx32 x, y, z; } FxVec;

extern void VEC_Subtract(VecFx32 *a, VecFx32 *b, VecFx32 *ab);
extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *ab);
extern struct ListNode *List_First(void *list);
extern int Ov107_HitShape_TestSphere(void *part, struct Sphere *shape, int mode);
extern int VEC_Normalize(VecFx32 *out, VecFx32 *in);
extern void ScaleVec3Fx12(int scale, VecFx32 *in, VecFx32 *out);
extern int Ov107_AiState_ApplyHit(Actor *obj, void *target, struct HitPacket *packet);
extern void func_ov107_020c0b90(void *actor, int a, VecFx32 v, int d);
extern void SetIndexedSlot(struct Node *node, int slot, void *arg);

void Ov288_AreaSweepAttack_Tick(struct Node *node)
{
    int mode;
    struct State *st;
    void *scene;
    void *actor;
    void *other;
    void *world;
    Actor *obj;
    struct ListNode *ln;
    struct ListNode *part;
    struct Sphere shape;
    int timer;
    int scale;
    int slot;
    u8 hasMode;
    int limit;
    int count;
    unsigned int clamp;

    scene = node->pList;
    st = node->pState;
    timer = st->timer034 + *(int *)((char *)scene + 0x2c);
    st->timer034 = timer;
    if (timer < 0x800) {
        actor = st->pActor;
        mode = *(int *)((char *)actor + 0x38c);
        if (mode == 0) {
            if (mode != 0) {
                return;
            }
            if (timer < 0x200) {
                return;
            }
        }
        world = *(void **)((char *)actor + 4);
        hasMode = (u8)(mode != 0);
        shape = *(struct Sphere *)((char *)actor + 0x74);
        scale = *(int *)((char *)st->pActor + 0x38c) != 0 ? 0x6000 : 0x1000;
        shape.radius = (int)(((long long)shape.radius * scale + 0x800) >> 12);
        ln = List_First((char *)world + 0x80);
        obj = ln == 0 ? 0 : (Actor *)ln->item;
        if (obj == 0) {
            return;
        }
        slot = hasMode * 6;
        do {
            if (obj != (Actor *)st->pActor && (obj->flags60.bits.lo & 1) != 0 &&
                (obj->field_1ac & 3) == 0 &&
                (st->mask >> obj->id & 1) == 0) {
                part = List_First((char *)obj + 0x22c);
                while (part != 0) {
                    if (Ov107_HitShape_TestSphere(part->item, &shape, 0) != 0) {
                        struct HitPacket packet = {0};
                        VecFx32 hitNormal;
                        VEC_Subtract(&obj->sphere.center, (VecFx32 *)((char *)st->pActor + 0x74),
                                     &packet.normal);
                        VEC_Normalize(&packet.normal, &packet.normal);
                        hitNormal = packet.normal;
                        packet.flags00 = packet.flags00 & 0xffff0000 | 4 | 0x8000;
                        packet.field10 = packet.field10 & 0xffff0000 |
                                         *(u16 *)((char *)slot + (int)st->pActor + 0x290);
                        packet.field14 = packet.field14 & 0xffff0000 |
                                         (u16)*(int *)((char *)st->pActor + 0x258);
                        packet.field14 = (u16)packet.field14 |
                                         (u32)*(u8 *)((char *)st->pActor + 0x19c) << 0x10;
                        packet.pPart = part;
                        if (Ov107_AiState_ApplyHit(obj, st->pTarget, &packet) != 0) {
                            st->mask = st->mask | (unsigned long long)1 << obj->id;
                            ScaleVec3Fx12(shape.radius, &hitNormal, &hitNormal);
                            VEC_Add(&hitNormal, &shape.centre, &hitNormal);
                            func_ov107_020c0b90(st->pActor, 0, hitNormal, 0);
                        }
                    }
                    part = (struct ListNode *)List_Next((char *)obj + 0x22c);
                }
            }
            ln = (struct ListNode *)List_Next((char *)world + 0x80);
            obj = ln == 0 ? 0 : (Actor *)ln->item;
        } while (obj != 0);
        return;
    }
    if (st->sent054 == 0 && st->pTarget != 0) {
        struct Msg msg = {0};
        FxVec v;
        struct Msg *m;
        void *src;
        FxVec *pos;
        count = 0;
        m = &msg;
        other = *(void **)((char *)st->pTarget + 0x18c);
        limit = 4;
        do {
            if ((st->mask >> limit & 1) != 0) {
                obj = (Actor *)Ov107_FindMessageHandler((u16)limit);
                if (obj == 0 || *(u8 *)((char *)obj + 0x19c) != 0x6b) {
                    count++;
                }
            }
            limit++;
        } while (limit < 0x40);
        m->h[0] = *(u16 *)((char *)st->pActor + 2);
        ((u8 *)m)[2] = 5;
        ((u8 *)m)[3] = 3;
        src = st->pActor;
        pos = (FxVec *)((char *)src + 0x74);
        v.x = pos->x;
        ((u8 *)&msg)[5] = (u8)(((u32)v.x.value >> 16 & 0x7f) | ((u32)v.x.value >> 24 & 0x80));
        ((u8 *)&msg)[6] = (u8)((u32)v.x.value >> 8);
        ((u8 *)&msg)[7] = (u8)v.x.value;
        v.y = pos->y;
        ((u8 *)&msg)[8] = (u8)(((u32)v.y.value >> 16 & 0x7f) | ((u32)v.y.value >> 24 & 0x80));
        ((u8 *)&msg)[9] = (u8)((u32)v.y.value >> 8);
        ((u8 *)&msg)[10] = (u8)v.y.value;
        v.z = pos->z;
        ((u8 *)&msg)[11] = (u8)(((u32)v.z.value >> 16 & 0x7f) | ((u32)v.z.value >> 24 & 0x80));
        ((u8 *)&msg)[12] = (u8)((u32)v.z.value >> 8);
        ((u8 *)&msg)[13] = (u8)v.z.value;
        clamp = *(u8 *)((char *)other + 0x2ab3);
        if (clamp > 8) {
            clamp = 8;
        } else if (clamp < 1) {
            clamp = 1;
        }
        ((u8 *)&msg)[4] = (u8)((count + 1) * clamp);
        MsgQueue_Post(1, &msg.h, 0xe);
        st->sent054 = 1;
    }
    if (*(u8 *)((char *)st->pOwner + 0xad) != 0) {
        return;
    }
    *(u8 *)((char *)st->pActor + 0x1c7) = 3;
    SetIndexedSlot(node, node->slot, 0);
}
