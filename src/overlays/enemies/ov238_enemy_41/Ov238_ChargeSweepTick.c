/* Charge sweep tick of the ov238 enemy. Between 0.17 and 1.49 of the +0x2c clock a sphere at the +8
 * position (radius growing from 1.04 with the clock, capped at 1.71) collects the entities around the
 * owner; each one not yet in the +0x1c 64-bit mask with a +0x1c4 hit hook gets a 40-byte packet (the
 * +0x28 heading, the partner's +0x296 power and +0x293 knock bits, a random reaction of the owner's
 * four at +0x39c, knock 100, the partner as source); an acceptance latches +0x394, marks the kind and
 * sends message 3 from the partner 1.5 above the victim's +0x190 point. Every visible, unguarded actor
 * of the world list (not the partner) with a shown part inside the sphere then gets a kind-4 44-byte
 * packet through 020c5cfc (random reaction, partner power, owner +0x258 reaction, knock 100) and is
 * marked on acceptance. Once the clock reaches 2.0 +0x390 clears and sub-state 0 follows. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/actor.h"
#include "game/engine.h"

typedef struct { VecFx32 pos; int nRadius; } Sphere;
struct Atk { u8 b0; u8 bits; u8 r2; u8 r3; u16 power; };
struct Rider { char pad[0x28c]; struct Atk atk[2]; };
struct ListNode { void *item; };
struct W8 { unsigned int lo : 8; };

struct HitPacket40 {
    int nKind;
    VecFx32 vNormal;
    int nPower;
    int nReaction;
    u8 bKnock;
    int pSource;
    int w[2];
};

struct HitPacket {
    u32 flags00;
    VecFx32 normal;
    u32 field10;
    u32 field14;
    void *pPart;
    u8 bKnock;
    u32 tail[3];
};

struct ChargeState {
    int pOwner;             /* +0x00 */
    int pad04;
    VecFx32 *pPos;             /* +0x08 */
    char pad0c[0x10];
    unsigned long long mask; /* +0x1c */
    int pad24;
    int nHeading;           /* +0x28 */
    int nClock;             /* +0x2c */
};

typedef int (*HitHook)(u16 id, struct HitPacket40 *packet);

extern int Ov107_CollectSphereOverlaps(int owner, Sphere *query, int *out);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern struct ListNode *List_First(void *list);
extern int Ov107_HitShape_TestSphere(void *part, Sphere *shape, int flag);
extern int Ov107_AiState_ApplyHit(Actor *obj, int target, struct HitPacket *packet);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov238_ChargeSweepTick(int *node)
{
    struct ChargeState *st = (struct ChargeState *)node[1];
    Sphere sphere;
    VecFx32 dir;
    int hits[4];
    struct HitPacket40 packet1 = {0};
    VecFx32 at;
    long n;
    long i;
    int partner = *(int *)(st->pOwner + 0x398);
    int bit;
    Actor *obj;
    struct ListNode *ln;
    struct ListNode *part;
    int world;

    if (st->nClock >= 0x2a8 && st->nClock < 0x17e8) {
        int idx = ANG2IDX(st->nHeading) * 2;

        dir.y = 0;
        dir.x = data_0203d210[idx];
        dir.z = data_0203d210[idx + 1];
        sphere.pos = *st->pPos;
        if (st->nClock < 0x17e8 - 0xaa0) {
            sphere.nRadius = st->nClock + 0xe00;
        } else {
            sphere.nRadius = 0x17e8 + 0x360;
        }
        n = Ov107_CollectSphereOverlaps(st->pOwner, &sphere, hits);
        i = 0;
        if (n > 0) {
            do {
                bit = 1 << *(u16 *)(hits[i] + 2);
                if ((st->mask & (long long)bit) == 0 && *(HitHook *)(hits[i] + 0x1c4) != 0) {
                    packet1.vNormal = dir;
                    packet1.nKind |= 1;
                    packet1.nPower = ((u16 *)partner)[0x14b];
                    if ((((u8 *)(partner + 0x292))[1] & 1) != 0) {
                        packet1.nKind |= 0x20;
                    }
                    if ((((u8 *)(partner + 0x292))[1] & 2) != 0) {
                        packet1.nKind |= 0x40;
                    }
                    packet1.nReaction = *(short *)(st->pOwner + RandNextScaled(4) * 2 + 0x39c);
                    packet1.bKnock = 100;
                    packet1.pSource = *(int *)(st->pOwner + 0x398);
                    if ((*(HitHook *)(hits[i] + 0x1c4))(*(u16 *)(hits[i] + 2), &packet1) != 0) {
                        at = *(VecFx32 *)(hits[i] + 0x190);
                        *(int *)(st->pOwner + 0x394) = 1;
                        st->mask |= (long long)bit;
                        at.y += 0x1800;
                        func_ov107_020c0b90(*(int *)(st->pOwner + 0x398), 3, at, 0);
                    }
                }
            } while (++i < n);
        }
        world = *(int *)(st->pOwner + 4);
        {
            struct HitPacket packet2 = {0};

            packet2.flags00 = packet2.flags00 & 0xffff0000 | 4;
            packet2.flags00 = (u16)packet2.flags00 |
                              (u32)*(short *)(st->pOwner + RandNextScaled(4) * 2 + 0x39c) << 16;
            packet2.field10 = packet2.field10 & 0xffff0000 | ((struct Rider *)*(int *)(st->pOwner + 0x398))->atk[1].power;
            packet2.field14 = packet2.field14 & 0xffff0000 | (u16)*(int *)(st->pOwner + 0x258);
            packet2.bKnock = 100;
            packet2.normal = dir;
            ln = List_First((char *)world + 0x80);
            obj = ln == 0 ? 0 : (Actor *)ln->item;
            while (obj != 0) {
                if ((st->mask >> obj->id & 1) == 0 && obj != *(Actor **)(st->pOwner + 0x398) &&
                    (obj->flags60.bits.lo & 1) != 0 && (obj->field_1ac & 7) == 0) {
                    for (part = List_First((char *)obj + 0x22c); part != 0;
                         part = (struct ListNode *)List_Next((char *)obj + 0x22c)) {
                        if ((((struct W8 *)((char *)part + 8))->lo & 1) != 0 &&
                            Ov107_HitShape_TestSphere(part->item, &sphere, 0) != 0) {
                            packet2.pPart = part;
                            if (Ov107_AiState_ApplyHit(obj, *(int *)(st->pOwner + 0x25c), &packet2) != 0) {
                                st->mask = st->mask | (unsigned long long)1 << obj->id;
                                break;
                            }
                        }
                    }
                }
                ln = (struct ListNode *)List_Next((char *)world + 0x80);
                obj = ln == 0 ? 0 : (Actor *)ln->item;
            }
        }
    }
    st->nClock += *(int *)(node[0] + 0x2c);
    if (st->nClock < 0x1fe0) {
        return;
    }
    *(int *)(st->pOwner + 0x390) = 0;
    *(u8 *)(st->pOwner + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
