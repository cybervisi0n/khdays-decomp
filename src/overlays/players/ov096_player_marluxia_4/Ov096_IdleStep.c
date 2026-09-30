/*
 * Idle step: the per-frame body of the ov040 enemy's idle state.
 *
 * Marks bit 16 of both 64-bit flag words for the local player, samples the ground from a zero
 * vector (falling velocity to +0x58, or bit 46 and zero when grounded without bit 2 of +0x24)
 * and adds the horizontal step to +0x498. The state callback runs once for its side effects;
 * then, with no pattern pending (rig +4) and the actor in +0x1c mode 1, 5 or 6, the rig's +0x118
 * timer picks pattern 2 between 0x1b000 and 0x21000 and pattern 1 outside. The callback runs
 * again into bit 1 of +0x694; a finished state while the rig's hold latch (+8) is raised is
 * rewound to 0xc000 instead (bit 29, bit 1 dropped). A finished state with the latch clear hands
 * over: a pending pattern goes to 0x23 (alternate mode, rig +0) or 0x22, otherwise the
 * velocities are cleared, bit 2 set and state 0 (after the slot callback) or 2 follows.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { unsigned char b0 : 1, b1 : 1; } Flags;

extern int Ov022_StepAnchorDelta(char *self, void *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Anim_SetFrameWrapped(char *anim, int track, int frame);
extern void *Ov022_ActorSetState(char *self, int state);
extern char *data_ov096_020bc0c0;

void *Ov096_IdleStep(char *self)
{
    int r;
    VecFx32 sample;
    VecFx32 step;
    char *rig = data_ov096_020bc0c0 + 0xc50 + 0x2000;
    void *next = 0;

    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x464) |= 0x10000;
    }
    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x46c) |= 0x10000;
    }
    sample.y = 0;
    sample.z = 0;
    sample.x = 0;
    Ov022_StepAnchorDelta(self, &sample);
    if (sample.y != 0) {
        *(int *)(self + 0x58) = sample.y;
    } else if ((*(int *)(self + 0x24) & 4) == 0) {
        *(unsigned long long *)self |= 0x400000000000ULL;
        *(int *)(self + 0x58) = 0;
    }
    step = sample;
    step.y = 0;
    VEC_Add((VecFx32 *)(self + 0x98 + 0x400), &step, (VecFx32 *)(self + 0x98 + 0x400));
    (*(int (**)(char *))(self + 0x668))(self);
    if (*(int *)(rig + 4) == 0
        && (*(u16 *)(self + 0x1c) == 5 || *(u16 *)(self + 0x1c) == 6 || *(u16 *)(self + 0x1c) == 1)) {
        if (*(int *)(rig + 0x118) >= 0x1b000 && *(int *)(rig + 0x118) <= 0x21000) {
            *(int *)(rig + 4) = 2;
        } else {
            *(int *)(rig + 4) = 1;
        }
    }
    r = (*(int (**)(char *))(self + 0x668))(self);
    ((Flags *)(self + 0x694))->b1 = (unsigned char)r;
    if (((Flags *)(self + 0x694))->b1 && *(int *)(rig + 8) != 0) {
        ((Flags *)(self + 0x294 + 0x400))->b1 = 0;
        Anim_SetFrameWrapped(*(char **)(self + 0x20) + 4, 0, 0xc000);
        *(int *)(self + 0x7b0) = 0xc000;
        *(unsigned long long *)self |= 0x20000000ULL;
    }
    if (((Flags *)(self + 0x694))->b1 && *(int *)(rig + 8) == 0) {
        if (*(int *)(rig + 4) != 0) {
            if (*(int *)rig != 0) {
                next = Ov022_ActorSetState(self, 0x23);
            } else {
                next = Ov022_ActorSetState(self, 0x22);
            }
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
