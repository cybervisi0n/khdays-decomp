/* Hover step of the mission enemy: on the local player both 64-bit flag words at +0x464 and
 * +0x46c get bit 16. In mode 0x2f with the +0x7b0 timer still at or under 0xf000 the shared
 * sampler drives the step (its vertical part becomes the fall speed, or in the air bit 46 is
 * raised and the speed cleared). Otherwise, with a cached heading (+0x2abc) and a live but not
 * yet finished emitter at +0x22f8, the node turns towards the heading (plus the +0x478 offset,
 * clamped by the turn helper) unless the facing is locked, the enemy moves along it at the owner
 * block's +0xc speed and mode 0x32 is entered; failing that, mode 0x32 is left for 0x2f with the
 * animation and timer wound to 0x18000 and the enemy stays put, and in the air with bit 36 the
 * fall speed is -0x8f (-0xd6 at 20 fps). The step is applied on the ground plane, the attack
 * burst ticks when not sampling, and the actor's hook decides bit 1 of +0x694: becoming active
 * hands over to state 0x22 (finished emitter) or 0x23 (quiet emitter), or in mode 0x2f rewinds
 * the animation and timer to 0x18000. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { unsigned char b0 : 1, b1 : 1; } Flags;

extern int Ov022_IsState9Or6WithFlag200(char *emitter);
extern int Ov022_IsSlotReady(char *emitter);
extern int Ov022_ClampAngleTowardTarget(char *self, unsigned int angle);            /* Ov022_ClampAngleTowardTarget */
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);            /* ScaleVec3Fx12 */
extern void Anim_SetFrameWrapped(void *animation, int track, int frame);          /* Anim_SetFrameWrapped */
extern void Ov022_StepAnchorDelta(char *self, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Ov043_AttackBurstTick(char *self);
extern void *Ov022_ActorSetState(char *self, int state);
extern char *data_ov043_020b58e0;
extern short data_0203d210[];

void *Ov043_HoverStep(char *self)
{
    VecFx32 step;
    VecFx32 flat;
    void *next = 0;
    char *pBlock = data_ov043_020b58e0 + 0x138 + 0x2c00;
    int bSample;
    int a;
    unsigned int *node;
    int r;

    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x464) |= 0x10000;
    }
    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x46c) |= 0x10000;
    }
    if (*(int *)(self + 0x6bc) == 0x2f && *(int *)(self + 0x7b0) <= 0xf000) {
        bSample = 1;
    } else {
        bSample = 0;
        if (*(int *)(self + 0x2abc) != -1 && Ov022_IsState9Or6WithFlag200(self + 0x2f8 + 0x2000) != 0
            && Ov022_IsSlotReady(self + 0x2f8 + 0x2000) == 0) {
            a = Ov022_ClampAngleTowardTarget(self, (u16)(*(int *)(self + 0x2abc) + *(short *)(self + 0x478)));
            node = *(unsigned int **)(self + 0x20);
            if ((*node & 0x20) == 0) {
                *(u16 *)((char *)node + 0x80) = a + 0x8000;
                *(u16 *)((char *)node + 4) |= 0x20;
            }
            step.x = -data_0203d210[(a >> 4) * 2];
            step.z = -data_0203d210[(a >> 4) * 2 + 1];
            step.y = 0;
            ScaleVec3Fx12(*(int *)(pBlock + 0xc), &step, &step);
            (*(void (**)(char *, int))(self + 0x664))(self, 0x32);
        } else {
            if (*(int *)(self + 0x6bc) == 0x32) {
                (*(void (**)(char *, int))(self + 0x664))(self, 0x2f);
                Anim_SetFrameWrapped(*(char **)(self + 0x20) + 4, 0, 0x18000);
                *(int *)(self + 0x7b0) = 0x18000;
            }
            step.z = 0;
            step.y = 0;
            step.x = 0;
        }
    }
    if (bSample != 0) {
        Ov022_StepAnchorDelta(self, &step);
        if (step.y != 0) {
            *(int *)(self + 0x58) = step.y;
        } else if ((*(int *)(self + 0x24) & 4) == 0) {
            *(unsigned long long *)self |= 0x400000000000ULL;
            *(int *)(self + 0x58) = 0;
        }
    } else {
        if ((*(int *)(self + 0x24) & 4) == 0) {
            if ((*(unsigned long long *)self & 0x1000000000ULL) != 0) {
                *(int *)(self + 0x58) = GetFrameRateMode() == 1 ? -0xd6 : -0x8f;
            }
        }
    }
    flat = step;
    flat.y = 0;
    VEC_Add((VecFx32 *)(self + 0x98 + 0x400), &flat, (VecFx32 *)(self + 0x98 + 0x400));
    if (bSample == 0) {
        Ov043_AttackBurstTick(self);
    }
    r = (*(int (**)(char *))(self + 0x668))(self);
    ((Flags *)(self + 0x694))->b1 = (unsigned char)r;
    if (((Flags *)(self + 0x694))->b1) {
        if (Ov022_IsSlotReady(self + 0x2f8 + 0x2000) == 0) {
            if (Ov022_IsState9Or6WithFlag200(self + 0x2f8 + 0x2000) == 0) {
                next = Ov022_ActorSetState(self, 0x23);
            } else if (*(int *)(self + 0x6bc) == 0x2f) {
                Anim_SetFrameWrapped(*(char **)(self + 0x20) + 4, 0, 0x18000);
                *(int *)(self + 0x7b0) = 0x18000;
            }
        } else {
            next = Ov022_ActorSetState(self, 0x22);
        }
    }
    return next;
}
