/* Steer one flying part of the ov042 enemy (x4: ov042/061/081/098), the ov031 shape with the
 * trail: the solver moves the part from its +0xcc position by the step it computes, the new
 * position is stored and the trail laid along the step (Ov042_LayTrail). The part lands (state
 * 4) once it has travelled past the model's +0x14 distance from its +0x10 start, or, for a
 * timed part (bit 0 of +0), once its +4 timer reaches the model's +0x18. On landing: effect 0xc7
 * (variant 1) at the landing point if the owner's +0x694 bit is set, the part's +0xc word is
 * set to 0x2080 and its eight +0x13c bone links cleared to -1, its +0x3c animation restarted,
 * and the local player queues reaction 3/1 on the owner. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { u8 b0 : 1; } Bits1;

extern void Ov022_ComputeShotStep(VecFx32 *out, char *a, char *part, void *arg);
extern void Ov022_ResolveShotHit(char *a, char *part, VecFx32 *c, VecFx32 *d);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Ov042_LayTrail(VecFx32 *origin, VecFx32 *move);                     /* Ov042_LayTrail */
extern int VEC_Distance(const VecFx32 *a, const VecFx32 *b);                       /* VEC_Distance */
extern void Ov022_MarshalStateByte9(char *a, char *part);
extern void Slot_Spawn(int nId, int nSub, VecFx32 *pPos, int nFlag);           /* Slot_Spawn */
extern void Ov022_ReleaseRigSlots(char *part, int anim);
extern int Session_GetLocalPlayerIndex(void);                                                /* Session_GetLocalPlayerIndex */

int Ov042_SteerPart(char *group, char *part, void *arg)
{
    VecFx32 vTo;
    VecFx32 vStep;
    VecFx32 vFrom;
    char *owner = *(char **)(group + 8);
    char *model = *(char **)(part + 0x138);
    int i;

    vFrom = *(VecFx32 *)(part + 0xcc);
    Ov022_ComputeShotStep(&vStep, group, part, arg);
    Ov022_ResolveShotHit(group, part, &vFrom, &vStep);
    VEC_Add(&vFrom, &vStep, &vTo);
    *(VecFx32 *)(part + 0xcc) = vTo;
    Ov042_LayTrail(&vFrom, &vStep);
    if (*(signed char *)(part + 2) != 3) {
        if (VEC_Distance((VecFx32 *)(part + 0x10), &vTo) > *(int *)(model + 0x14)) {
            *(char *)(part + 2) = 4;
        }
    }
    Ov022_MarshalStateByte9(group, part);
    if (*(u8 *)part & 1) {
        if (*(int *)(part + 4) >= *(int *)(model + 0x18)) {
            *(char *)(part + 2) = 4;
        }
    }
    if (*(signed char *)(part + 2) != 2) {
        if (((Bits1 *)(owner + 0x694))->b0) {
            Slot_Spawn(0xc7, 1, &vTo, 0);
        }
        *(char *)(part + 2) = 4;
        *(int *)(part + 0xc) = 0x2080;
        for (i = 0; i < 8; i++) {
            ((short *)part)[i + 0x9e] = -1;
        }
        Ov022_ReleaseRigSlots(part, *(int *)(model + 0x3c));
        if (Session_GetLocalPlayerIndex() == 0) {
            *(u8 *)(owner + 0x47a) = 3;
            *(u8 *)(owner + 0x47b) = 1;
        }
    }
    return 0;
}
