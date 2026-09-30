/* Burst tick of the ov218 enemy's shot: for 0.5 after the bounce the +0x34 clock runs and the owner's
 * sphere (radius 2.0) hits every entity around the +0x390 body once (the +0x3c 64-bit kind mask): the
 * direct hits are pushed by 0.5 along the flattened direction away from the owner (kind 1 with a
 * +0x398 partner, else 0) and get message 0 at their position; every visible, unguarded actor of the
 * world list with a shown part inside the sphere gets a kind-4 hit packet (normal away from the
 * owner, the body's +0x290 power for that kind, the owner's +0x258 reaction, the part) through
 * 020c5cfc and message 0 on acceptance. Any hit fires reaction 0x135 mode 5 at the +8 target. Once
 * the +4 rig is idle sub-state 0 follows. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/actor.h"
#include "game/engine.h"

struct Sphere { VecFx32 centre; int radius; };
struct ListNode { void *item; };
struct W8 { unsigned int lo : 8; };

struct HitPacket {
    u32 flags00;
    VecFx32 normal;
    u32 field10;
    u32 field14;
    void *pPart;
    u32 tail[4];
};

struct ShotState {
    int pOwner;             /* +0x00 */
    int pRig;               /* +0x04 */
    int pTarget;            /* +0x08 */
    char pad0c[0x28];
    int timer034;           /* +0x34 */
    char pad38[4];
    unsigned long long mask; /* +0x3c */
};

extern int Ov107_CollectSphereOverlaps(int owner, struct Sphere *query, int *out);
extern void VEC_Subtract(void *a, void *b, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
/* Defined taking kind as int: declared narrower here, which is what makes mwcc truncate the
 * argument at the call as the ROM does (declared as defined, the code comes out different). */
extern int Ov107_InvokeHitCallback(int hit, int a, int b, u8 kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern struct ListNode *List_First(void *list);
extern int Ov107_HitShape_TestSphere(void *part, struct Sphere *shape, int flag);
extern int Ov107_AiState_ApplyHit(Actor *obj, int target, struct HitPacket *packet);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, int at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042258;

void Ov218_ShotBurstTick(int *node)
{
    struct ShotState *st = (struct ShotState *)node[1];
    struct Sphere sphere;
    int hits[4];
    VecFx32 push;
    long n;
    int hit;
    int world;
    u8 kind;
    long i;
    Actor *obj;
    struct ListNode *ln;
    struct ListNode *part;

    st->timer034 += *(int *)(node[0] + 0x2c);
    if (st->timer034 < 0x800) {
        hit = 0;
        world = *(int *)(st->pOwner + 4);
        kind = *(int *)(st->pOwner + 0x398) != 0 ? 1 : 0;
        sphere = *(struct Sphere *)(st->pOwner + 0x74);
        sphere.radius = 0x2000;
        n = Ov107_CollectSphereOverlaps(*(int *)(st->pOwner + 0x390), &sphere, hits);
        for (i = 0; i < n; i++) {
            u16 id = *(u16 *)(hits[i] + 2);

            if ((st->mask >> id & 1) != 0) {
                continue;
            }
            st->mask = st->mask | (unsigned long long)1 << id;
            VEC_Subtract((void *)(hits[i] + 0x74), (void *)(st->pOwner + 0x74), &push);
            push.y = 0;
            VEC_Normalize(&push, &push);
            ScaleVec3Fx12(0x800, &push, &push);
            if (Ov107_InvokeHitCallback(hits[i], st->pOwner, *(int *)(st->pOwner + 0x390), kind, &push, 0) != 0) {
                func_ov107_020c0b90(*(int *)(st->pOwner + 0x390), 0, *(VecFx32 *)(hits[i] + 0x74), 0);
                hit = 1;
            }
        }
        ln = List_First((char *)world + 0x80);
        obj = ln == 0 ? 0 : (Actor *)ln->item;
        if (obj != 0) {
            int power = kind * 6;

            do {
                if (obj != (Actor *)st->pOwner && (st->mask >> obj->id & 1) == 0) {
                    st->mask = st->mask | (unsigned long long)1 << obj->id;
                    if ((obj->flags60.bits.lo & 1) != 0 && (obj->field_1ac & 3) == 0) {
                        part = List_First((char *)obj + 0x22c);
                        while (part != 0) {
                            if ((((struct W8 *)((char *)part + 8))->lo & 1) != 0 &&
                                Ov107_HitShape_TestSphere(part->item, &sphere, 0) != 0) {
                                struct HitPacket packet = {0};

                                VEC_Subtract(&obj->sphere.center, (void *)(st->pOwner + 0x74), &packet.normal);
                                if (VEC_Normalize(&packet.normal, &packet.normal) == 0) {
                                    packet.normal = data_02042258;
                                }
                                packet.flags00 = packet.flags00 & 0xffff0000 | 4;
                                packet.field10 = packet.field10 & 0xffff0000 |
                                                 *(u16 *)(power + *(int *)(st->pOwner + 0x390) + 0x290);
                                packet.field14 = packet.field14 & 0xffff0000 |
                                                 (u16)*(int *)(st->pOwner + 0x258);
                                packet.pPart = part;
                                if (Ov107_AiState_ApplyHit(obj, 0, &packet) != 0) {
                                    func_ov107_020c0b90(*(int *)(st->pOwner + 0x390), 0, obj->sphere.center, 0);
                                    hit = 1;
                                }
                            }
                            part = (struct ListNode *)List_Next((char *)obj + 0x22c);
                        }
                    }
                }
                ln = (struct ListNode *)List_Next((char *)world + 0x80);
                obj = ln == 0 ? 0 : (Actor *)ln->item;
            } while (obj != 0);
        }
        if (hit != 0) {
            Ov107_BuildAndSendUpdate(st->pOwner, 0x135, 5, st->pTarget);
        }
    }
    if (*(u8 *)(st->pRig + 0xad) != 0) {
        return;
    }
    *(u8 *)(st->pOwner + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
