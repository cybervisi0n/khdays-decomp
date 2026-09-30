/* Per-frame step of the ov034 enemy (x4: ov034/052/072/090): on the local player both 64-bit
 * flag words at +0x464 and +0x46c get bit 16, the shared sampler's motion is folded into the
 * position at +0x498 on the ground plane (and, unless bit 2 of +0x24 says the enemy is
 * grounded, bit 46 of the actor flags is raised and the vertical speed cleared), the actor's own
 * hook decides bit 1 of +0x694, and past 0xb000 on the +0x7b0 timer the enemy hands over to
 * state 0x23. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { unsigned char b0 : 1, b1 : 1; } Flags;

extern int Ov022_StepAnchorDelta(char *self, void *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void *Ov022_ActorSetState(char *self, int state);

void *Ov073_StepAndCheckTimer(char *self)
{
    int r;
    VecFx32 sample;
    VecFx32 step;
    void *next = 0;

    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x464) |= 0x10000;
    }
    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x46c) |= 0x10000;
    }
    Ov022_StepAnchorDelta(self, &sample);
    if ((*(int *)(self + 0x24) & 4) == 0) {
        *(unsigned long long *)self |= 0x400000000000ULL;
        *(int *)(self + 0x58) = 0;
    }
    step = sample;
    step.y = 0;
    VEC_Add((VecFx32 *)(self + 0x98 + 0x400), &step, (VecFx32 *)(self + 0x98 + 0x400));
    r = (*(int (**)(char *))(self + 0x668))(self);
    ((Flags *)(self + 0x694))->b1 = (unsigned char)r;
    if (*(int *)(self + 0x7b0) > 0xb000) {
        next = Ov022_ActorSetState(self, 0x23);
    }
    return next;
}
