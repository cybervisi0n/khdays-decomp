
#include "nitro/types.h"
#include "nitro/fx_types.h"

extern const VecFx32 data_02041dc8;

extern int VEC_Mag(const VecFx32 *v);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *ab);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *pUnit);
extern void ScaleVec3Fx12(int nFactor, const VecFx32 *pSrc, VecFx32 *pDst);
extern int VEC_Distance(const VecFx32 *a, const VecFx32 *b);
extern void Actor_SetVecAndSyncChild(void *pActor, const VecFx32 *pPos);
extern int Ov002_GetModuleScale(void);
extern void Mover_StepActor(void *pActorSlot, int nFlag);

/* Step one element's knockback and report how far it actually moved.
 *
 * Two modes share the tail. A launch (bit 9) carries the element towards a
 * stored landing spot until either its velocity dies or the flight timer runs
 * out, and reports completion. Ordinary knockback (bit 8 clear) integrates the
 * velocity, clamps the step to a maximum, bleeds the velocity by a per-class
 * factor, and stops once the element is nearly still.
 *
 * The return says what happened: 0 nothing, 1 moved, 2 finished or was already
 * flagged as struck. When an output vector is given it receives the actual
 * displacement, which is not the same as the intended one because the actor
 * call can refuse or clip the move.
 */
int Ov002_StepKnockback(int nUnused, char *pElement, VecFx32 *pOutDelta)
{
    VecFx32 vAfter;
    VecFx32 vBefore;
    VecFx32 vStep;
    int nResult;
    int bStruck;
    int nMag;
    int nBleed;
    int nDecay;

    nResult = 0;
    bStruck = (*(u16 *)(pElement + 0x448) & 4) != 0;

    if (pOutDelta != 0) {
        *pOutDelta = data_02041dc8;
    }

    if (bStruck || (*(u8 *)(pElement + 0x04) & 2) != 0) {
    vBefore = *(VecFx32 *)(*(char **)(pElement + 0x08) + 0xa8);

    if ((*(u16 *)(pElement + 0x448) & 0x200) != 0) {
        if (VEC_Mag((VecFx32 *)(pElement + 0x44c)) < 0x10) {
            Actor_SetVecAndSyncChild(*(void **)(pElement + 0x08),
                                     (VecFx32 *)(pElement + 0x458));
            *(u16 *)(pElement + 0x448) &= ~0x200;
            *(int *)(pElement + 0x44c) = 0;
            *(int *)(pElement + 0x450) = 0;
            *(int *)(pElement + 0x454) = 0;
        } else {
            *(int *)(pElement + 0x468) += Ov002_GetModuleScale();
            if (*(int *)(pElement + 0x468) >= 0x3000) {
                vAfter = *(VecFx32 *)(pElement + 0x458);
                nResult = 2;
                *(u16 *)(pElement + 0x448) &= ~0x200;
                *(int *)(pElement + 0x44c) = 0;
                *(int *)(pElement + 0x450) = 0;
                *(int *)(pElement + 0x454) = 0;
            } else {
                VEC_Add(&vBefore, (VecFx32 *)(pElement + 0x44c), &vAfter);
            }
            Actor_SetVecAndSyncChild(*(void **)(pElement + 0x08), &vAfter);
        }
    } else if ((*(u16 *)(pElement + 0x448) & 0x100) == 0) {
        *(int *)(pElement + 0x10) = data_02041dc8.x;
        *(int *)(pElement + 0x14) = 0;
        *(int *)(pElement + 0x18) = data_02041dc8.z;

        nMag = VEC_Mag((VecFx32 *)(pElement + 0x44c));

        if ((*(int *)(pElement + 0x0c) & 4) == 0) {
            if (*(int *)(pElement + 0x40) == (int)0x80000000) {
                *(int *)(pElement + 0x40) = 410;
            }
            *(u16 *)(pElement + 0x448) |= 1;
        }

        if ((*(u16 *)(pElement + 0x448) & 1) != 0
            && (*(int *)(pElement + 0x0c) & 4) != 0) {
            *(u16 *)(pElement + 0x448) &= ~1;
            if (*(int *)(pElement + 0x464) > 0) {
                nDecay = (int)(((long long)*(int *)(pElement + 0x464)
                                * *(short *)(pElement + 0x02) + 0x800) >> 12);
                *(int *)(pElement + 0x464) = nDecay;
                if (nDecay < 0x29) {
                    *(int *)(pElement + 0x464) = 0;
                } else {
                    *(int *)(pElement + 0x40) = nDecay;
                    *(u16 *)(pElement + 0x448) |= 1;
                }
            }
        }

        if ((*(u16 *)(pElement + 0x448) & 1) != 0
            && (*(int *)(pElement + 0x0c) & 2) == 0) {
            *(u16 *)(pElement + 0x448) &= ~2;
        }

        if (nMag > 0) {
            nBleed = *(short *)(pElement + 0x00);
            vStep = *(VecFx32 *)(pElement + 0x44c);

            if (*(int *)(pElement + 0x450) != 0) {
                *(int *)(pElement + 0x40) = *(int *)(pElement + 0x450);
                *(int *)(pElement + 0x464) = *(int *)(pElement + 0x450);
                *(int *)(pElement + 0x450) = 0;
                *(u16 *)(pElement + 0x448) |= 1;
            }

            if ((*(u16 *)(pElement + 0x448) & 1) != 0) {
                nBleed += ((0x1000 - nBleed) >> 1) + ((0x1000 - nBleed) >> 2);
            }

            if (nMag > 0xe00) {
                VEC_Normalize((VecFx32 *)(pElement + 0x44c), &vStep);
                ScaleVec3Fx12(0xe00, &vStep, &vStep);
            }

            vAfter = *(VecFx32 *)(*(char **)(pElement + 0x08) + 0xa8);
            VEC_Add(&vAfter, &vStep, &vAfter);
            ScaleVec3Fx12(nBleed, (VecFx32 *)(pElement + 0x44c),
                          (VecFx32 *)(pElement + 0x44c));

            *(int *)(pElement + 0x10) = vStep.x;
            *(int *)(pElement + 0x14) = 0;
            *(int *)(pElement + 0x18) = vStep.z;

            nMag = VEC_Mag((VecFx32 *)(pElement + 0x44c));
            if ((*(u16 *)(pElement + 0x448) & 1) == 0 && nMag <= 0x80) {
                *(int *)(pElement + 0x454) = 0;
                *(int *)(pElement + 0x450) = 0;
                *(int *)(pElement + 0x44c) = 0;
                *(u16 *)(pElement + 0x448) &= ~2;
            }
        }

        Mover_StepActor(pElement + 0x08, 1);
    }

    vAfter = *(VecFx32 *)(*(char **)(pElement + 0x08) + 0xa8);

    if (VEC_Distance(&vBefore, &vAfter) > 4
        && (vBefore.x != vAfter.x || vBefore.y != vAfter.y
            || vBefore.z != vAfter.z)) {
        if (nResult == 0) {
            nResult = 1;
        }
        if (pOutDelta != 0) {
            pOutDelta->x = vAfter.x - vBefore.x;
            pOutDelta->y = vAfter.y - vBefore.y;
            pOutDelta->z = vAfter.z - vBefore.z;
        }
    } else if (bStruck) {
        nResult = 2;
    }
    }

    return nResult;
}
