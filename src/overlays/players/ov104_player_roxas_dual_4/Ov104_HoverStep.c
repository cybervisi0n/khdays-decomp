/* Hover step of the ov049 enemy (x4: ov049/068/087/104): on the local player both 64-bit flag
 * words at +0x464 and +0x46c get bit 16; a zero motion sample is folded into the position at
 * +0x498 (in the air bit 7 of +0x24 is dropped, bit 46 raised and the vertical speed cleared);
 * the effect spawner runs with the heading at +0x2aba, the actor's hook decides bit 1 of +0x694
 * (becoming active raises bit 49, shows the node and, for the local player, sets bit 1 of
 * +0x464), a hit (bit 1 of +0x18) latches the rig's +0x2e18 marker, and an active enemy with a
 * busy emitter and no marker either hands over to state 0x22 once (busy effect context, latched
 * at +0x2e14) or ends the hover: bit 49 dropped, node hidden, animation set 0xc000 before its
 * end, bit 29 raised; otherwise it stops, raises bit 2 and lands (state 0 with the slot
 * callback when grounded, else state 2). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { unsigned char b0 : 1, b1 : 1; } Flags;

extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Ov104_SpawnNextEffect(char *self, int dt);
extern int Ov022_IsState9Or6WithFlag200(char *emitter);
extern int Ov022_IsSlotReady(char *context);
extern void Anim_SetFrameWrapped(void *animation, int track, int frame);              /* Anim_SetFrameWrapped */
extern void *Ov022_ActorSetState(char *self, int state);
extern char *data_ov104_020bc2a0;

void *Ov104_HoverStep(char *self)
{
    VecFx32 sample;
    VecFx32 step;
    void *next = 0;
    char *rig = data_ov104_020bc2a0 + 0xfc + 0x2c00;
    int r;
    unsigned int *node;
    int len;

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
        *(int *)(self + 0x24) &= ~0x80;
        *(unsigned long long *)self |= 0x400000000000ULL;
        *(int *)(self + 0x58) = 0;
    }
    step = sample;
    step.y = 0;
    VEC_Add((VecFx32 *)(self + 0x98 + 0x400), &step, (VecFx32 *)(self + 0x98 + 0x400));
    Ov104_SpawnNextEffect(self, *(short *)(self + 0x2aba));
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
    if ((*(u16 *)(self + 0x18) & 2) == 2) {
        *(int *)(rig + 0x11c) = 1;
    }
    if (((Flags *)(self + 0x694))->b1) {
        if (Ov022_IsState9Or6WithFlag200(self + 0x2f8 + 0x2000) != 0 && *(int *)(rig + 0x11c) == 0) {
            if (Ov022_IsSlotReady(self + 0x2f8 + 0x2000) != 0 && *(int *)(rig + 0x118) == 0) {
                *(int *)(rig + 0x118) = 1;
                next = Ov022_ActorSetState(self, 0x22);
            } else {
                len = Anim_GetLengthQ12(*(char **)(self + 0x20) + 4, 0) - 0xc000;
                *(unsigned long long *)self &= ~0x2000000000000ULL;
                node = *(unsigned int **)(self + 0x20);
                if ((*node & 0x20) == 0) {
                    SceneNode_Disable(node + 1);
                }
                Anim_SetFrameWrapped(*(char **)(self + 0x20) + 4, 0, len);
                *(int *)(self + 0x7b0) = len;
                *(unsigned long long *)self |= 0x20000000;
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
                *(int *)(self + 0x58) = 0;
                next = Ov022_ActorSetState(self, 2);
            }
        }
    }
    return next;
}
