/* Descent tick of the ov158 enemy: the target is re-acquired into +0x2c (no target ends the
 * state at once); a 50.0 downward probe (Ov158_ProbeGround) that hits turns the +8 sub-object
 * onto the hit normal (ed60 by data_02042264) and lifts its +0x20 height by 0x100. A second
 * probe of 30 x rate x 0.5 that hits turns the +0xc sub-object onto its normal, moves it (and
 * the +0x10 point) to the +0x1c point, fires reaction 0x150 mode 7 there and hands over to
 * Ov158_EnterGroundDrop. Otherwise the segment from the +0x10 point along the normalised probe
 * (radius 0x400) is swept over the actor list: with hits, the +0x10 point moves to the nearest
 * one along the segment, the +0xc sub-object goes there, the +0x28 distance clears, reaction
 * 0x150 mode 8 fires and the same hand-over happens; without, the +0x10 point tracks the +0x1c
 * x / +0x24 z while its y sinks by the probe and the +4 sub-object follows. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int q[4]; } Quat;
typedef struct { VecFx32 origin; VecFx32 dir; int nLength; int nRadius; } Segment;

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

extern int Ov107_FindNearestObject(int owner, int flag);
extern int Ov158_ProbeGround(int *state, VecFx32 *dir, VecFx32 *out);
extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern void Srt_SetRotationQuat(void *transform, const Quat *q);
extern void Srt_SetTranslation(void *transform, const VecFx32 *translation);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int Ov107_CollectSegmentOverlaps(int owner, Segment *query, int *results);
extern int Segment_ClosestPoint(VecFx32 *point, Segment *seg, fx64 *outDist);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Ov158_EnterGroundDrop(int *node);
extern const VecFx32 data_02042264;

void Ov158_DescentTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 down = {0};
    int step = *(int *)(node[0] + 0x2c) * 30;
    VecFx32 hit;
    Quat q1;
    Quat q2;
    int results[4];
    Segment seg;
    fx64 along;
    int i;
    int n;
    fx64 best;

    state[0xb] = Ov107_FindNearestObject(*state, 0);
    if (state[0xb] == 0) {
        Task_MarkFinished(node);
        return;
    }
    down.y = -0x32000;
    if (Ov158_ProbeGround(state, &down, &hit) != 0) {
        Quat_FromTwoVectors(&q1, &data_02042264, &hit);
        Srt_SetRotationQuat((void *)(state[2] + 4), &q1);
        state[8] += 0x100;
        Srt_SetTranslation((void *)(state[2] + 4), (VecFx32 *)(state + 7));
    }
    down.y = -FX_Mul(step, 0x800);
    if (Ov158_ProbeGround(state, &down, &hit) != 0) {
        Quat_FromTwoVectors(&q2, &data_02042264, &hit);
        Srt_SetRotationQuat((void *)(state[3] + 4), &q2);
        *(VecFx32 *)(state + 4) = *(VecFx32 *)(state + 7);
        Srt_SetTranslation((void *)(state[3] + 4), (VecFx32 *)(state + 4));
        Ov107_BuildAndSendUpdate(*state, 0x150, 7, state + 4);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov158_EnterGroundDrop);
        return;
    }
    seg.origin = *(VecFx32 *)(state + 4);
    seg.nLength = VEC_Normalize(&down, &seg.dir);
    seg.nRadius = 0x400;
    n = Ov107_CollectSegmentOverlaps(*state, &seg, results);
    if (n != 0) {
        best = 0x7fffffffffffffffLL;
        for (i = 0; i < n; i++) {
            Segment_ClosestPoint((VecFx32 *)(results[i] + 0x74), &seg, &along);
            if (along < best) {
                best = along;
            }
        }
        state[4] = (fx32)((best * seg.dir.x + 0x80000000LL) >> 32);
        state[5] = (fx32)((best * seg.dir.y + 0x80000000LL) >> 32);
        state[6] = (fx32)((best * seg.dir.z + 0x80000000LL) >> 32);
        VEC_Add(&seg.origin, (VecFx32 *)(state + 4), (VecFx32 *)(state + 4));
        Srt_SetTranslation((void *)(state[3] + 4), (VecFx32 *)(state + 4));
        state[0xa] = 0;
        Ov107_BuildAndSendUpdate(*state, 0x150, 8, state + 4);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov158_EnterGroundDrop);
        return;
    }
    state[4] = state[7];
    state[5] += down.y;
    state[6] = state[9];
    Srt_SetTranslation((void *)(state[1] + 4), (VecFx32 *)(state + 4));
}
