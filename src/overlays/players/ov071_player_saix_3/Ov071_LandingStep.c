/* Landing step of the ov033 enemy (x4: ov033/051/071/089): on the local player both 64-bit flag
 * words at +0x464 and +0x46c get bit 16, the shared sampler's motion is folded into the position
 * at +0x498 on the ground plane, and the actor's own hook decides bit 1 of +0x694: becoming
 * active raises bit 49, shows the node and, for the local player, sets bit 1 of +0x464. Once
 * that bit is set the velocities at +0x498 and +0x698 are cleared, bit 2 of the actor flags is
 * raised and the enemy hands over to state 0 when grounded (also telling the slot callback 0),
 * otherwise to state 2; a hand-over also drops bit 16 of the +0x46c flags. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { unsigned char b0 : 1, b1 : 1; } Flags;

extern int Ov022_StepAnchorDelta(char *self, void *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void *Ov022_ActorSetState(char *self, int state);

void *Ov071_LandingStep(char *self)
{
    int r;
    VecFx32 sample;
    VecFx32 step;
    void *next = 0;
    unsigned int *node;

    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x464) |= 0x10000;
    }
    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x46c) |= 0x10000;
    }
    Ov022_StepAnchorDelta(self, &sample);
    step = sample;
    step.y = 0;
    VEC_Add((VecFx32 *)(self + 0x98 + 0x400), &step, (VecFx32 *)(self + 0x98 + 0x400));
    r = (*(int (**)(char *))(self + 0x668))(self);
    ((Flags *)(self + 0x694))->b1 = (unsigned char)r;
    if (((Flags *)(self + 0x694))->b1) {
        *(unsigned long long *)self |= 0x2000000000000ULL;
        node = *(unsigned int **)(self + 0x20);
        if ((*node & 0x20) == 0) {
            SceneNode_Enable(node + 1);
        }
        if (Session_GetLocalPlayerIndex() == 0) {
            *(unsigned long long *)(self + 0x464) |= 2;
        }
    }
    if ((*(unsigned long long *)(self + 0x464) & 2) != 0) {
        *(int *)(self + 0x4a0) = 0;
        *(int *)(self + 0x49c) = 0;
        *(int *)(self + 0x498) = 0;
        *(int *)(self + 0x6a0) = 0;
        *(int *)(self + 0x69c) = 0;
        *(int *)(self + 0x698) = 0;
        *(unsigned long long *)self |= 4;
        if ((*(int *)(self + 0x24) & 4) != 0) {
            next = Ov022_ActorSetState(self, 0);
            (*(void (**)(char *, int))(self + 0x664))(self, 0);
        } else {
            next = Ov022_ActorSetState(self, 2);
        }
    }
    if (next != 0) {
        *(unsigned long long *)(self + 0x46c) &= ~0x10000ULL;
    }
    return next;
}
