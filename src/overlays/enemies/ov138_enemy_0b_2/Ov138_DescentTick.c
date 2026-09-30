/* Descent tick of the ov137 enemy: the target is re-acquired into +0x2c (no target ends the
 * state at once); a 50.0 downward probe (Ov138_ProbeGround) that hits turns the +8
 * sub-object onto the hit normal (ed60 by data_02042264) and lifts its +0x20 height by 0x100.
 * The +0x10 point tracks the +0x398 bone's x/z while its y sinks by 30 x rate x 0.5 per frame
 * and places the +4 sub-object; the +0x28 timer accumulates the rate and past 1.0 resets with
 * the +0x30 hit mask before handing over to Ov138_AiDescentProbe. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int q[4]; } Quat;

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

extern int Ov107_FindNearestObject(int owner, int flag);
extern int Ov138_ProbeGround(int *state, VecFx32 *dir, VecFx32 *out);
extern void Quat_FromTwoVectors(Quat *out, const VecFx32 *from, const VecFx32 *to);
extern void Srt_SetRotationQuat(void *transform, const Quat *q);
extern void Srt_SetTranslation(void *transform, const VecFx32 *translation);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov138_AiDescentProbe(int *node);
extern const VecFx32 data_02042264;

void Ov138_DescentTick(int *node)
{
    int *state = (int *)node[1];
    int step = *(int *)(node[0] + 0x2c) * 30;
    VecFx32 hit;
    VecFx32 down = {0};
    Quat q;

    state[0xb] = Ov107_FindNearestObject(*state, 0);
    if (state[0xb] == 0) {
        Task_MarkFinished(node);
        return;
    }
    down.y = -0x32000;
    if (Ov138_ProbeGround(state, &down, &hit) != 0) {
        Quat_FromTwoVectors(&q, &data_02042264, &hit);
        Srt_SetRotationQuat((void *)(state[2] + 4), &q);
        state[8] += 0x100;
        Srt_SetTranslation((void *)(state[2] + 4), (VecFx32 *)(state + 7));
    }
    state[4] = *(int *)(*(int *)(*state + 0x398) + 0x14);
    state[5] += FX_Mul(step, 0x800);
    state[6] = *(int *)(*(int *)(*state + 0x398) + 0x1c);
    Srt_SetTranslation((void *)(state[1] + 4), (VecFx32 *)(state + 4));
    state[0xa] += *(int *)(node[0] + 0x2c);
    if (state[0xa] <= 0x1000) {
        return;
    }
    state[0xa] = 0;
    *(unsigned char *)((char *)state + 0x30) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov138_AiDescentProbe);
}
