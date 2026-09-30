/* Flight tick of the ov299 projectile. The +0x10 vertical speed loses 0x60 x dt / 0x88 per
 * tick. A capsule from the previous +0x18 position towards the +4 position with the actor's
 * +0x80 radius is queried: any entity accepting a kind-0 hit pushed away from the actor at 0x800
 * (through the +0x390 item) ends the flight with effect 1 at the position, reaction 0x170/5 and
 * sub-state 0. Then every other ready actor of the scene's +0x80 list (not the +0x390 item, with
 * bits 0-2 of +0x1ac clear) whose active +0x22c shapes cross the capsule gets a kind-4 hit
 * packet (zero normal, the +0x390 item's +0x290 id and the actor's +0x258 id) through the +0x25c
 * source (only the first crossing shape of each actor is tried); an accepted hit ends the
 * flight the same way. Otherwise the +0x18 position
 * advances to the +4 position: a line cast (fff888) or a 0x300 sphere cast (fff8e8) against the
 * scene's +0x7c collision along the step stops at the contact (effect 1 there, reaction 0x170/6,
 * effect 2 too on a 0xd-typed surface) and, past 0x3c000 of +0x24 flight time, the flight ends
 * with effect 1 at the position. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct hw60 { unsigned short lo : 8, hi : 8; };
struct w8 { unsigned int lo : 8, rest : 24; };

typedef struct Segment {
    VecFx32 p0;
    VecFx32 dir;
    int scale;
} Segment;

struct Capsule {
    Segment seg;
    int radius;
};

struct HitPacket {
    u32 flagsLo : 16;
    u32 flagsHi : 16;
    VecFx32 normal;
    int field_10 : 16;
    int field_12 : 16;
    int field_14 : 16;
    int field_16 : 16;
    void *field_18;
    signed char field_1c;
    u8 pad01d[3];
    int field_20;
    u32 flags24Lo : 16;
    u32 flags24Hi : 16;
    int field_28;
};

extern void VEC_Subtract(const void *a, const void *b, VecFx32 *d);
extern void VEC_Add(const void *a, const void *b, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern int Ov107_CollectSegmentOverlaps(int actor, struct Capsule *cap, int *out);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int actor, int effect, VecFx32 at, int d);
extern void Ov107_BuildAndSendUpdate(int actor, int id, int mode, void *anchor);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int *List_First(int list);
extern int *List_Next(int list);
extern int Ov107_HitShape_TestSegment(void *shape, Segment *seg, int flags);
extern int Ov107_AiState_ApplyHit(int other, int source, struct HitPacket *packet);
extern int Collision_CastRayEx(void *collision, VecFx32 *from, VecFx32 *step, void *ignore);
extern int Collision_CastSphereEx(void *collision, VecFx32 *from, VecFx32 *step, int radius, void *ignore);
extern void ScaleVec3Fixed27(int t, VecFx32 *v, VecFx32 *d);
extern const VecFx32 data_02041dc8;

void Ov299_ProjectileFlightTick(int *node)
{
    int *state = (int *)node[1];
    struct Capsule cap;
    VecFx32 last;
    VecFx32 step;
    int hits[4];
    VecFx32 push;
    int i;
    int nHits;
    int scene;
    int other;
    int *entry;
    int *shape;
    int hit;
    int rec;
    int res;

    scene = *(int *)(*state + 4);
    state[4] += *(int *)(*node + 0x2c) * -0x60 / 0x88;
    cap.seg.p0 = *(VecFx32 *)(state + 6);
    VEC_Subtract((void *)state[1], state + 6, &cap.seg.dir);
    cap.seg.scale = VEC_Normalize(&cap.seg.dir, &cap.seg.dir);
    cap.radius = *(int *)(*state + 0x80);
    nHits = Ov107_CollectSegmentOverlaps(*state, &cap, hits);
    for (i = 0; i < nHits; i++) {
        VEC_Subtract((void *)(hits[i] + 0x74), (void *)(*state + 0x74), &push);
        push.y = 0;
        VEC_Normalize(&push, &push);
        ScaleVec3Fx12(0x800, &push, &push);
        if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x390), 0, &push, 0) != 0) {
            func_ov107_020c0b90(*state, 1, *(VecFx32 *)state[1], 0);
            Ov107_BuildAndSendUpdate(*state, 0x170, 5, (void *)state[1]);
            *(u8 *)(*state + 0x1c7) = 0;
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
            return;
        }
    }
    entry = List_First(scene + 0x80);
    other = entry == 0 ? 0 : *entry;
    if (other != 0) {
        do {
            if (other != *(int *)(*state + 0x390) && (((struct hw60 *)(other + 0x60))->lo & 1) != 0 &&
                (*(u16 *)(other + 0x1ac) & 1) == 0 && (*(u16 *)(other + 0x1ac) & 2) == 0 &&
                (*(u16 *)(other + 0x1ac) & 4) == 0) {
                for (shape = List_First(other + 0x22c); shape != 0; shape = List_Next(other + 0x22c)) {
                    if ((((struct w8 *)(shape + 2))->lo & 1) != 0 && Ov107_HitShape_TestSegment((void *)shape[0], &cap.seg, 0) != 0) {
                        struct HitPacket packet = {0};
                        packet.flagsLo = 4;
                        packet.normal = data_02041dc8;
                        packet.field_10 = *(u16 *)(*(int *)(*state + 0x390) + 0x290);
                        packet.field_14 = *(int *)(*state + 0x258);
                        packet.field_18 = shape;
                        if (Ov107_AiState_ApplyHit(other, *(int *)(*state + 0x25c), &packet) != 0) {
                            func_ov107_020c0b90(*state, 1, *(VecFx32 *)state[1], 0);
                            Ov107_BuildAndSendUpdate(*state, 0x170, 5, (void *)state[1]);
                            *(u8 *)(*state + 0x1c7) = 0;
                            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
                            return;
                        }
                        break;
                    }
                }
            }
            entry = List_Next(scene + 0x80);
            other = entry == 0 ? 0 : *entry;
        } while (other != 0);
    }
    VEC_Subtract((void *)state[1], state + 6, &step);
    last = *(VecFx32 *)(state + 6);
    *(VecFx32 *)(state + 6) = *(VecFx32 *)state[1];
    hit = Collision_CastRayEx(*(void **)(scene + 0x7c), &last, &step, 0);
    if (hit != 0) {
        rec = *(int *)(hit + 4);
        ScaleVec3Fixed27(*(int *)(hit + 0xc), &step, &step);
        VEC_Add(&step, &last, &last);
        func_ov107_020c0b90(*state, 1, last, 0);
        Ov107_BuildAndSendUpdate(*state, 0x170, 6, &last);
        if (rec != 0 && *(u8 *)(rec + 0x83) == 0xd) {
            func_ov107_020c0b90(*state, 2, last, 0);
        }
        *(u8 *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    res = Collision_CastSphereEx(*(void **)(scene + 0x7c), &last, &step, 0x300, 0);
    if (res != 0 && *(int *)(res + 8) == 0) {
        ScaleVec3Fixed27(*(int *)(res + 0xc), &step, &step);
        VEC_Add(&step, &last, &last);
        func_ov107_020c0b90(*state, 1, last, 0);
        Ov107_BuildAndSendUpdate(*state, 0x170, 6, &last);
        *(u8 *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    state[9] += *(int *)(*node + 0x2c);
    if (state[9] < 0x3c000) {
        return;
    }
    func_ov107_020c0b90(*state, 1, *(VecFx32 *)state[1], 0);
    *(u8 *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
