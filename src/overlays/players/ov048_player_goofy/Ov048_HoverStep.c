/* Hover step of the ov048 enemy (x4: ov048/067/086/103): on the local player both 64-bit flag
 * words at +0x464 and +0x46c get bit 16, a zero motion sample is folded into the position at
 * +0x498 (raising bit 46 and clearing the vertical speed unless grounded), and the actor's own
 * hook runs. Unless the hit flag (bit 0 of +0x1a) is set while the emitter at +0x22f8 is busy,
 * the enemy hands over: to state 0x22 once the rig's +0x2f84 counter passed 1, otherwise it
 * stops, raises bit 2 and goes to state 2 (or tells the slot callback and goes to state 0 when
 * grounded). Hit while busy, it rewinds the animation at 0x9000 on the +0x7b0 timer, clears the
 * timer and raises bit 29.
 *
 * Lever: the slot callback takes (self, 0) -- the zero of the six velocity stores is still live
 * in r1 at the blx, which is why the ROM colours it r1 and re-materialises it for state 0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov022_IsState9Or6WithFlag200(char *emitter);
extern void *Ov022_ActorSetState(char *self, int state);
extern void Anim_SetFrameWrapped(void *animation, int track, int frame);              /* Anim_SetFrameWrapped */
extern char *data_ov048_020b4b80;

void *Ov048_HoverStep(char *self)
{
    VecFx32 sample;
    VecFx32 step;
    char *rig = data_ov048_020b4b80 + 0x2c + 0x2c00;
    void *next = 0;

    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x464) |= 0x10000;
    }
    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x46c) |= 0x10000;
    }
    sample.z = 0;
    sample.y = 0;
    sample.x = 0;
    if ((*(int *)(self + 0x24) & 4) == 0) {
        *(unsigned long long *)self |= 0x400000000000ULL;
        *(int *)(self + 0x58) = 0;
    }
    step = sample;
    step.y = 0;
    VEC_Add((VecFx32 *)(self + 0x98 + 0x400), &step, (VecFx32 *)(self + 0x98 + 0x400));
    (*(int (**)(char *))(self + 0x668))(self);
    if ((*(u16 *)(self + 0x1a) & 1) == 0 || Ov022_IsState9Or6WithFlag200(self + 0x2f8 + 0x2000) == 0) {
        if (*(int *)(rig + 0x358) > 1) {
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
    } else if (*(int *)(self + 0x7b0) >= 0x9000) {
        Anim_SetFrameWrapped(*(char **)(self + 0x20) + 4, 0, 0);
        *(int *)(self + 0x7b0) = 0;
        *(unsigned long long *)self |= 0x20000000;
    }
    return next;
}
