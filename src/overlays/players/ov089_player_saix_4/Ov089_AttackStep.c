/* Attack step of the ov033 enemy (x4: ov033/051/071/089): on the local player both 64-bit flag
 * words at +0x464 and +0x46c get bit 16; the rig's dash flag (+0x2c34) drops without a hit or
 * once the +0x4cc counter passes 0x9000, and while set the sampler's motion is doubled before
 * being folded into the position at +0x498 (a vertical component becomes the vertical speed,
 * otherwise -- unless grounded -- bit 46 is raised and the speed cleared); the rig's mode
 * (+0x2c30) picks the recoil or the sustained burst, the actor's own hook decides bit 1 of
 * +0x694 (becoming active raises bit 49, shows the node and, for the local player, sets bit 1 of
 * +0x464), and once that bit is set the enemy hands over to state 0x21 while the emitter at
 * +0x22f8 is busy, otherwise to state 0x23. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { unsigned char b0 : 1, b1 : 1; } Flags;

extern int Ov022_StepAnchorDelta(char *self, void *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void *Ov022_ActorSetState(char *self, int state);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);         /* ScaleVec3Fx12 */
extern void Ov089_FireRecoilBurst(char *self);
extern void Ov089_FireSustainedBurst(char *self);
extern int Ov022_IsState9Or6WithFlag200(char *emitter);
extern char *data_ov089_020bc120;

void *Ov089_AttackStep(char *self)
{
    int r;
    VecFx32 sample;
    VecFx32 step;
    void *next = 0;
    char *rig = data_ov089_020bc120 + 0x2c + 0x2c00;
    unsigned int *node;

    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x464) |= 0x10000;
    }
    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x46c) |= 0x10000;
    }
    if ((*(u16 *)(self + 0x1a) & 1) == 0 || *(int *)(self + 0x4cc) >= 0x9000) {
        *(int *)(rig + 8) = 0;
    }
    Ov022_StepAnchorDelta(self, &sample);
    if (*(int *)(rig + 8) != 0) {
        ScaleVec3Fx12(0x2000, &sample, &sample);
    }
    if (sample.y != 0) {
        *(int *)(self + 0x58) = sample.y;
    } else if ((*(int *)(self + 0x24) & 4) == 0) {
        *(unsigned long long *)self |= 0x400000000000ULL;
        *(int *)(self + 0x58) = 0;
    }
    step = sample;
    step.y = 0;
    VEC_Add((VecFx32 *)(self + 0x98 + 0x400), &step, (VecFx32 *)(self + 0x98 + 0x400));
    if (*(int *)(rig + 4) == 0) {
        Ov089_FireRecoilBurst(self);
    } else {
        Ov089_FireSustainedBurst(self);
    }
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
            next = Ov022_ActorSetState(self, 0x23);
        } else {
            next = Ov022_ActorSetState(self, 0x21);
        }
    }
    return next;
}
