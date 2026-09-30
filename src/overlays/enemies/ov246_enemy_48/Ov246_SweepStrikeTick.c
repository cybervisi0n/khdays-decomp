/* Sweep strike tick of the ov246 enemy (the Ov143_StepSlamStrike shape). In mode 0 (+0x24) the
 * +0x14 direction first eases (x/z by a twentieth, y by a tenth) towards the unit direction from the
 * +4 point to the best-facing entity of the collision owner's +0xa8 list (flag bit 1 of +0x40
 * and bit 0 of +0x60 set, dot product above 0x400) and is re-normalised. The direction scaled
 * by the +0x20 length becomes the +8 vector, and the query is the segment from the +4 point
 * along it (radius 1.0). Mode 0 sweeps the actor list of the +0x38c item and asks the shared
 * checker whether each candidate is hit along the +8 vector; the first acceptance ends the
 * action with reaction 0x158 mode 7. Mode 1 instead locks on, fills a request with the
 * item's id, the object's kind and the lock handle (flags 0x2024) and, if the handle's +8 bit 0
 * is set and the lock is taken, ends the action with reaction 0 / 0x53. Either ending sends the
 * position message at the +4 point and clears the sub-state. Otherwise the +0x28 distance
 * advances by the length and the action ends (reaction 0x158 mode 7) once the object reports
 * contact or the distance passes 30.0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Segment { VecFx32 origin; VecFx32 dir; int nLength; int nRadius; };
struct HitWord { u32 lo : 16, hi : 16; };

struct HitCommand {
    struct HitWord flags00;
    VecFx32 vector04;
    u32 field10;
    u32 field14;
    void *hit18;
    int pad1c[4];
};

struct Ov246Contact { u8 bGrounded : 1, bBlocked : 1; };
struct Ov246Bits40 { int b0 : 1, b1 : 1; };
struct Ov246HalfByte { u16 lo : 8, hi : 8; };
struct Ov246Byte8 { u32 lo : 8, rest : 24; };

extern char **List_First(void *list);
extern char **List_Next(void *list);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *src, VecFx32 *dst);
extern int Ov107_CollectSegmentOverlaps(int item, struct Segment *query, int *results);
extern int Ov107_InvokeHitCallback(int ent, int actor, int item, int mode, void *dir, int flag);
extern void Ov246_SendPositionMessage(int *state);
extern void Ov107_BuildAndSendUpdate(int item, int id, int a, void *at);
extern void SetIndexedSlot(int *node, int slot, void *value);
extern int Ov107_FindEntityHitBySegment(int actor, struct Segment *query, void **out);
extern int Ov107_AiState_ApplyHit(int lock, int param, struct HitCommand *req);
extern VecFx32 data_02041dc8;

void Ov246_SweepStrikeTick(int *node)
{
    int *state = (int *)node[1];
    struct Segment query;
    VecFx32 d;
    VecFx32 bestDir;
    int results[4];
    void *handle;
    int lock;
    int i;
    int n;
    int nBest = 0x80000001;
    char *found = 0;
    char *list = *(char **)(*state + 4);
    char **it;
    char *e;
    int dot;

    if (state[9] == 0) {
        it = List_First(list + 0xa8);
        e = (it == 0) ? found : *it;
        while (e != 0) {
            if (((struct Ov246Bits40 *)(e + 0x40))->b1 != 0 && (((struct Ov246HalfByte *)(e + 0x60))->lo & 1) != 0) {
                VEC_Subtract((VecFx32 *)(e + 0x74), (VecFx32 *)state[1], &d);
                VEC_Normalize(&d, &d);
                dot = VEC_DotProduct((VecFx32 *)(state + 5), &d);
                if (dot > 0x400 && dot > nBest) {
                    nBest = dot;
                    bestDir = d;
                    found = e;
                }
            }
            it = List_Next(list + 0xa8);
            e = (it == 0) ? 0 : *it;
        }
        if (found != 0) {
            state[5] += (bestDir.x - state[5]) / 20;
            state[6] += (bestDir.y - state[6]) / 10;
            state[7] += (bestDir.z - state[7]) / 20;
            VEC_Normalize((VecFx32 *)(state + 5), (VecFx32 *)(state + 5));
        }
    }
    ScaleVec3Fx12(state[8], (VecFx32 *)(state + 5), (VecFx32 *)(state + 2));
    query.origin = *(VecFx32 *)state[1];
    query.dir = *(VecFx32 *)(state + 5);
    query.nLength = state[8];
    query.nRadius = 0x1000;

    if (state[9] == 0) {
        n = Ov107_CollectSegmentOverlaps(*(int *)(*state + 0x38c), &query, results);
        i = 0;
        if (n > 0) {
            do {
                if (Ov107_InvokeHitCallback(results[i], *state, *(int *)(*state + 0x38c), 1, state + 2, 0) != 0) {
                    Ov246_SendPositionMessage(state);
                    Ov107_BuildAndSendUpdate(*(int *)(*state + 0x38c), 0x158, 7, (void *)state[1]);
                    *(u8 *)(*state + 0x1c7) = 0;
                    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
                    return;
                }
            } while (++i < n);
        }
    } else {
        struct HitCommand spare = { 0 };

        if ((lock = Ov107_FindEntityHitBySegment(*state, &query, &handle)) != 0
            && (*(u16 *)(lock + 0x100 + 0xac) & 4) == 0) {
            struct HitCommand req = { 0 };

            req.flags00.lo = 0x2024;
            req.vector04 = data_02041dc8;
            req.field10 = (req.field10 & 0xffff0000) | *(u16 *)(*(int *)(*state + 0x38c) + 0x200 + 0x96);
            req.field14 = (req.field14 & 0xffff0000) | (u16)*(int *)(*state + 0x258);
            req.hit18 = handle;
            if ((((struct Ov246Byte8 *)((char *)handle + 8))->lo & 1) != 0
                && Ov107_AiState_ApplyHit(lock, *(int *)(*state + 0x25c), &req) != 0) {
                Ov246_SendPositionMessage(state);
                Ov107_BuildAndSendUpdate(*(int *)(*state + 0x38c), 0, 0x53, (void *)state[1]);
                *(u8 *)(*state + 0x1c7) = 0;
                SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
                return;
            }
        }
    }

    state[0xa] += state[8];
    if (((struct Ov246Contact *)(*state + 0x17a))->bGrounded == 0
        && ((struct Ov246Contact *)(*state + 0x17a))->bBlocked == 0
        && state[0xa] <= 0x1e000) {
        return;
    }
    Ov246_SendPositionMessage(state);
    Ov107_BuildAndSendUpdate(*(int *)(*state + 0x38c), 0x158, 7, (void *)state[1]);
    *(u8 *)(*state + 0x1c7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
