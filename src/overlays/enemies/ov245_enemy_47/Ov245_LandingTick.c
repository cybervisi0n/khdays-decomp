/* Ov245_LandingTick -- landing tick: the +0xc velocity is the negated +0x4c8 anchor direction.
 * While still descending (+0x14 < 0) the actor's +0x3bc query block, moved by the velocity, is
 * swept (020c8df0) and each hit whose +2 slot bit is not in the +0x40 mask is pushed
 * (0, 1.0, -5.0) by the actor itself (020ca918, mode 4, 0x80): a landing hit gets effect 0 at its
 * +0x74 position and its bit set, and any landing fires reaction 0/0x51 at the actor's +0x74.
 * Once landed, every actor of the scene's +0xa8 list in the same scene whose bit is not yet in
 * +0x42 is hit at the origin with mode 7 (when its bit is in the +0x438 owner's +0x3b0 mask) or
 * mode 4 (when in +0x40), 0x218, and marked in +0x42. Once the +4 item's animation is free
 * (+0xad) the owner is released (020d4870), sub-state 2 set and the node slot freed. */

#include "nitro/fx_types.h"

struct Ov245Query { VecFx32 pos; int w[12]; };

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov107_CollectCapsuleOverlaps(int actor, struct Ov245Query *query, int *out);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, const VecFx32 *push, int z);
extern void func_ov107_020c0b90(int actor, int effect, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern int *List_First(void *list);
extern int *List_Next(void *list);
extern int Ov245_FourShape_ResetAiIfReady(int owner);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;

void Ov245_LandingTick(int *node) {
    int *state = (int *)node[1];
    VecFx32 push;
    int hits[4];
    struct Ov245Query query;
    int any;
    int nHits;
    int i;
    unsigned char bit;
    unsigned char mask;
    int scene;
    int *entry;
    int other;
    int actor;

    ScaleVec3Fx12(-0x1000, (VecFx32 *)(*(int *)(*state + 0x4c8) + 0x2c), (VecFx32 *)(state + 3));
    if (state[5] < 0) {
        query = *(struct Ov245Query *)(*state + 0x3bc);
        any = 0;
        VEC_Add(&query.pos, (VecFx32 *)(state + 3), &query.pos);
        push.x = 0;
        push.y = 0x1000;
        push.z = -0x5000;
        nHits = Ov107_CollectCapsuleOverlaps(*state, &query, hits);
        for (i = 0; i < nHits; i++) {
            bit = 1 << *(unsigned short *)(hits[i] + 2);
            if ((*((unsigned char *)state + 0x40) & bit) == 0) {
                if (Ov107_InvokeHitCallback(hits[i], *state, *state, 4, &push, 0x80) != 0) {
                    func_ov107_020c0b90(*state, 0, *(VecFx32 *)(hits[i] + 0x74), 0);
                    *((unsigned char *)state + 0x40) |= bit;
                    any = 1;
                }
            }
        }
        if (any != 0) {
            Ov107_BuildAndSendUpdate(*state, 0, 0x51, (void *)(*state + 0x74));
        }
    } else {
        scene = *(int *)(*state + 4);
        entry = List_First((void *)(scene + 0xa8));
        other = entry == 0 ? 0 : *entry;
        while (other != 0) {
            mask = 1 << *(unsigned short *)(other + 2);
            if ((*((unsigned char *)state + 0x42) & mask) == 0 && *(int *)(other + 4) == *(int *)(*state + 4)) {
                actor = *state;
                if ((mask & *(unsigned char *)(*(int *)(actor + 0x438) + 0x3b0)) != 0) {
                    Ov107_InvokeHitCallback(other, actor, actor, 7, &data_02041dc8, 0x218);
                    *((unsigned char *)state + 0x42) |= mask;
                } else if ((*((unsigned char *)state + 0x40) & mask) != 0) {
                    Ov107_InvokeHitCallback(other, actor, actor, 4, &data_02041dc8, 0x218);
                    *((unsigned char *)state + 0x42) |= mask;
                }
            }
            entry = List_Next((void *)(scene + 0xa8));
            other = entry == 0 ? 0 : *entry;
        }
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov245_FourShape_ResetAiIfReady(*(int *)(*state + 0x438));
    *(unsigned char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
