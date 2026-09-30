/* Ov245_SwingTick -- swing tick: the +0x2c timer runs up by the frame step; past 0.43
 * (flag 1 of +0x35) effect 0 plays at the origin, past 0.46 (flag 2) the +0x3b0 joint's +0x14
 * anchor (flattened) becomes the +0x10 strike point with effect 1 and reaction 0x15a/8. Unless
 * the +0x3b4 owner's +0x1ac bit 1 is set: while flag 2 holds and the +0x30 sweep timer stays
 * under 0.43, an oriented box (scale 1.0 + 4.0 * clamp(timer)/0.365, kind 1) at the strike
 * point is swept (020c8fd0); between 0.365 and 0.498 without box hits the actor's +0x388 shape
 * capsule is swept instead (020c8f44). Each hit whose +2 slot bit is not in the +0x34 mask is
 * pushed 1.6875 along the flattened direction from the actor (Z when degenerate) through the
 * owner (020ca918, mode 3): a landing hit gets effect 0 at its +0x74, its bit and the owner's
 * +0x3a0 flag; any landing fires reaction 0/0x53 at the +0xc anchor. Once the +4 item's
 * animation is free (+0xad), remaining +0x28 swings without a landing re-plan the turn
 * (020d40e8); otherwise pose 2 and the node moves to 020d462c. */

#include "nitro/fx_types.h"

typedef struct { VecFx32 p0; VecFx32 dir; int scale; } Segment;
struct Capsule { Segment seg; int radius; };
struct Ov245Box { VecFx32 pos; VecFx32 a; VecFx32 b; VecFx32 c; int scale; int kind; };

extern void func_ov107_020c0b90(int actor, int effect, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern int Ov107_CollectEntitiesTouchingDisc(int actor, struct Ov245Box *box, int *out);
extern int Ov107_CollectSegmentOverlaps(int actor, struct Capsule *cap, int *out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int a, int b, int kind, VecFx32 *push, int z);
extern void Ov245_TurnPlan(int *node);
extern void Ov107_PostTagUpdate(int actor, int pose, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042258;
extern const VecFx32 data_02042264;
extern void Ov245_FourShape_AiStep_QueueAction0OnAnimEnd(void);

void Ov245_SwingTick(int *node) {
    int *state = (int *)node[1];
    int hits[4];
    struct Ov245Box box;
    struct Capsule cap;
    VecFx32 push;
    long i;
    int nHits;
    unsigned char bit;

    state[0xb] += *(int *)(node[0] + 0x2c);
    if ((*((unsigned char *)state + 0x35) & 1) == 0 && state[0xb] >= 0x6e8) {
        *((unsigned char *)state + 0x35) |= 1;
        func_ov107_020c0b90(*state, 0, data_02041dc8, 0);
    }
    if ((*((unsigned char *)state + 0x35) & 2) == 0 && state[0xb] >= 0x770) {
        *((unsigned char *)state + 0x35) |= 2;
        *(VecFx32 *)(state + 4) = *(VecFx32 *)(*(int *)(*state + 0x3b0) + 0x14);
        state[5] = 0;
        func_ov107_020c0b90(*state, 1, *(VecFx32 *)(state + 4), 0);
        Ov107_BuildAndSendUpdate(*state, 0x15a, 8, (void *)(state + 4));
    }
    if ((*(unsigned short *)(*(int *)(*state + 0x3b4) + 0x100 + 0xac) & 2) == 0) {
        nHits = 0;
        if ((*((unsigned char *)state + 0x35) & 2) != 0 && (state[0xc] += *(int *)(node[0] + 0x2c)) <= 0x6e8) {
            box.pos = *(VecFx32 *)(state + 4);
            if (state[0xc] > 0x6e8 - 0x110) {
                nHits = 0x6e8 - 0x110;
            } else if (state[0xc] >= 0) {
                nHits = state[0xc];
            }
            box.scale = (nHits << 14) / 0x5d8 + 0x1000;
            box.a = data_02042270;
            box.b = data_02042258;
            box.c = data_02042264;
            box.kind = 1;
            nHits = Ov107_CollectEntitiesTouchingDisc(*state, &box, hits);
        }
        if (state[0xb] >= 0x5d8 && state[0xb] <= 0x5d8 + 0x220 && nHits == 0) {
            cap = *(struct Capsule *)(**(int **)(*state + 0x388) + 0x78);
            nHits = Ov107_CollectSegmentOverlaps(*state, &cap, hits);
        }
        for (i = 0; i < nHits; i++) {
            bit = 1 << *(unsigned short *)(hits[i] + 2);
            if ((*((unsigned char *)state + 0x34) & bit) == 0) {
                VEC_Subtract((VecFx32 *)(hits[i] + 0x74), (VecFx32 *)(*state + 0x74), &push);
                push.y = 0;
                if (VEC_Normalize(&push, &push) == 0) {
                    push = data_02042258;
                }
                ScaleVec3Fx12(0x1b00, &push, &push);
                if (Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x3b4), 3, &push, 0) != 0) {
                    func_ov107_020c0b90(*(int *)(*state + 0x3b4), 0, *(VecFx32 *)(hits[i] + 0x74), 0);
                    *((unsigned char *)state + 0x34) |= bit;
                    *(int *)(*state + 0x3a0) = 1;
                }
            }
        }
        if (*(int *)(*state + 0x3a0) != 0) {
            Ov107_BuildAndSendUpdate(*state, 0, 0x53, (void *)state[3]);
        }
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    state[10]--;
    if (state[10] > 0 && *(int *)(*state + 0x3a0) == 0) {
        Ov245_TurnPlan(node);
        return;
    }
    Ov107_PostTagUpdate(*state, 2, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_FourShape_AiStep_QueueAction0OnAnimEnd);
}
