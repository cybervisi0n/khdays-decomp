/* Idle step of the ov033 enemy (x4: ov033/051/071/089): on the local player both 64-bit flag
 * words at +0x464 and +0x46c get bit 16; the node turns (through the turn helper) to its own
 * heading nudged 0xfff either way by the +0x1a steer bits, unless the facing is locked; the
 * sampler's motion is folded into the position at +0x498 (in the air with bit 36 or an unset
 * vertical speed, bit 46 is raised and the speed cleared); the actor's hook decides bit 1 of
 * +0x694 and, with the emitter at +0x22f8 busy, an activation rewinds the animation, clears the
 * timer and the bit and raises bit 29; the forward burst is tried; an active enemy hands over to
 * state 0x23, and a +0x1c state of 5 or 6 to state 0x22. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { unsigned char b0 : 1, b1 : 1; } Flags;

extern int Ov022_ClampAngleTowardTarget(char *self, unsigned int angle);            /* Ov022_ClampAngleTowardTarget */
extern int Ov022_StepAnchorDelta(char *self, void *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov022_IsState9Or6WithFlag200(char *emitter);
extern void Anim_SetFrameWrapped(void *animation, int track, int frame);          /* Anim_SetFrameWrapped */
extern void Ov051_FireForwardBurst(char *self);
extern void *Ov022_ActorSetState(char *self, int state);

void *Ov051_IdleStep(char *self)
{
    VecFx32 sample;
    VecFx32 step;
    void *next = 0;
    int a;
    unsigned int *node;
    int r;

    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x464) |= 0x10000;
    }
    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x46c) |= 0x10000;
    }
    a = (u16)(*(u16 *)(*(char **)(self + 0x20) + 0x80) - 0x8000);
    if ((*(u16 *)(self + 0x1a) & 0x20) != 0) {
        a += 0xfff;
    } else if ((*(u16 *)(self + 0x1a) & 0x10) != 0) {
        a -= 0xfff;
    }
    a = Ov022_ClampAngleTowardTarget(self, (u16)a);
    node = *(unsigned int **)(self + 0x20);
    if ((*node & 0x20) == 0) {
        *(u16 *)((char *)node + 0x80) = a + 0x8000;
        *(u16 *)((char *)node + 4) |= 0x20;
    }
    Ov022_StepAnchorDelta(self, &sample);
    if ((*(int *)(self + 0x24) & 4) == 0
        && ((*(unsigned long long *)self & 0x1000000000ULL) != 0 || *(int *)(self + 0x58) == 0x80000000)) {
        *(unsigned long long *)self |= 0x400000000000ULL;
        *(int *)(self + 0x58) = 0;
    }
    step = sample;
    step.y = 0;
    VEC_Add((VecFx32 *)(self + 0x98 + 0x400), &step, (VecFx32 *)(self + 0x98 + 0x400));
    if (next == 0) {
        r = (*(int (**)(char *))(self + 0x668))(self);
        ((Flags *)(self + 0x694))->b1 = (unsigned char)r;
        if (((Flags *)(self + 0x694))->b1 && Ov022_IsState9Or6WithFlag200(self + 0x2f8 + 0x2000) != 0) {
            Anim_SetFrameWrapped(*(char **)(self + 0x20) + 4, 0, 0);
            *(int *)(self + 0x7b0) = 0;
            ((Flags *)(self + 0x694))->b1 = 0;
            *(unsigned long long *)self |= 0x20000000;
        }
    }
    Ov051_FireForwardBurst(self);
    if (((Flags *)(self + 0x694))->b1) {
        next = Ov022_ActorSetState(self, 0x23);
    }
    if (next == 0 && (u16)(*(u16 *)(self + 0x1c) + 0xfffb) <= 1) {
        next = Ov022_ActorSetState(self, 0x22);
    }
    return next;
}
