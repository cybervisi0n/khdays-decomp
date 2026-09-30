/* Descent step of the ov045 enemy (x4: ov045/064/083/100): on the local player both 64-bit flag
 * words at +0x464 and +0x46c get bit 16, the shared sampler's motion is folded into the position
 * at +0x498 (a vertical component becomes the vertical speed, otherwise -- unless grounded --
 * bit 46 is raised and the speed cleared; either way bit 7 of +0x24 is dropped), the actor's own
 * hook runs, a hit flag (bit 1 of +0x18) latches the rig's +0x2f08 marker, and once the +0x7b0
 * timer is within 0xf000 of the animation's end the enemy hands over: to state 0x22 without the
 * marker, otherwise it stops, raises bit 2 and goes to state 2 (or tells the slot callback and
 * goes to state 0 when grounded). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

extern int Ov022_StepAnchorDelta(char *self, void *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void *Ov022_ActorSetState(char *self, int state);
extern char *data_ov100_020bc1c0;

void *Ov100_DescentStep(char *self)
{
    VecFx32 sample;
    VecFx32 step;
    void *next = 0;
    char *rig = data_ov100_020bc1c0 + 0xdf0 + 0x2000;

    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x464) |= 0x10000;
    }
    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x46c) |= 0x10000;
    }
    Ov022_StepAnchorDelta(self, &sample);
    if (sample.y != 0) {
        *(int *)(self + 0x24) &= ~0x80;
        *(int *)(self + 0x58) = sample.y;
    } else if ((*(int *)(self + 0x24) & 4) == 0) {
        *(int *)(self + 0x24) &= ~0x80;
        *(unsigned long long *)self |= 0x400000000000ULL;
        *(int *)(self + 0x58) = 0;
    }
    step = sample;
    step.y = 0;
    VEC_Add((VecFx32 *)(self + 0x98 + 0x400), &step, (VecFx32 *)(self + 0x98 + 0x400));
    (*(int (**)(char *))(self + 0x668))(self);
    if ((*(u16 *)(self + 0x18) & 2) == 2) {
        *(int *)(rig + 0x118) = 1;
    }
    if (*(int *)(self + 0x7b0) >= Anim_GetLengthQ12(*(char **)(self + 0x20) + 4, 0) - 0xf000) {
        if (*(int *)(rig + 0x118) == 0) {
            next = Ov022_ActorSetState(self, 0x22);
        } else {
            *(int *)(self + 0x4a0) = 0;
            *(int *)(self + 0x49c) = 0;
            *(int *)(self + 0x498) = 0;
            *(int *)(self + 0x6a0) = 0;
            *(int *)(self + 0x69c) = 0;
            *(int *)(self + 0x698) = 0;
            *(unsigned long long *)self |= 4;
            if ((*(int *)(self + 0x24) & 4) != 0) {
                (*(void (**)(char *, int))(self + 0x664))(self, 0);
                next = Ov022_ActorSetState(self, 0);
            } else {
                next = Ov022_ActorSetState(self, 2);
            }
        }
    }
    return next;
}
