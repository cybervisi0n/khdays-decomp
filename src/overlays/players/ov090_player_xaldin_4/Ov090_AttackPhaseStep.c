/* Attack-phase step of the ov034 enemy (x4: ov034/052/072/090): on the local player both 64-bit
 * flag words at +0x464 and +0x46c get bit 16, the shared sampler's motion is folded into the
 * position at +0x498 on the ground plane (raising bit 46 and clearing the vertical speed unless
 * bit 2 of +0x24 says the enemy is grounded), the heavy burst is tried, and the actor's own hook
 * decides bit 1 of +0x694: becoming active raises bit 49, shows the node and, for the local
 * player, sets bit 1 of +0x464. Once that bit is set the enemy hands over to state 0x21 while
 * the emitter at +0x22f8 is busy, otherwise to state 0x25. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { unsigned char b0 : 1, b1 : 1; } Flags;

extern int Ov022_StepAnchorDelta(char *self, void *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Ov090_FireHeavyBurst(char *self);
extern int Ov022_IsState9Or6WithFlag200(char *emitter);
extern void *Ov022_ActorSetState(char *self, int state);

void *Ov090_AttackPhaseStep(char *self)
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
    if ((*(int *)(self + 0x24) & 4) == 0) {
        *(unsigned long long *)self |= 0x400000000000ULL;
        *(int *)(self + 0x58) = 0;
    }
    step = sample;
    step.y = 0;
    VEC_Add((VecFx32 *)(self + 0x98 + 0x400), &step, (VecFx32 *)(self + 0x98 + 0x400));
    Ov090_FireHeavyBurst(self);
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
        if (Ov022_IsState9Or6WithFlag200(self + 0x2f8 + 0x2000) == 0) {
            next = Ov022_ActorSetState(self, 0x25);
        } else {
            next = Ov022_ActorSetState(self, 0x21);
        }
    }
    return next;
}
