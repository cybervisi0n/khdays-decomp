/* Per-frame update for the ov102 panel. Raises the two visibility flag pairs while the
 * global gate is open, folds in the frame's movement delta, then runs the charge logic:
 * while the panel is in state 5 or 6 and displayable, a charge timer accumulates and,
 * once it passes its threshold, arms a spawn; when armed and the actor's energy at
 * +0x7b0 is high enough, the effect ring fires and the timer resets. Separately, once
 * the charge has run its full course, the energy is clamped back down. Finally the
 * registered predicate at +0x668 decides whether the panel has finished: if it has, the
 * cached vectors are cleared and the teardown handler runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct PanelBits {
    u8 b0 : 1;
    u8 bDone : 1;
    u8 rest : 6;
};

extern int Session_GetLocalPlayerIndex(void);
extern void Ov022_StepAnchorDelta(char *self, VecFx32 *pOut);
extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *pOut);
extern int Ov022_IsState9Or6WithFlag200(char *obj);
extern void Ov102_SpawnEffectRing(char *self);
extern int Ov022_GetGlobal34(void);
extern void Anim_SetFrameWrapped(int a, int b, int c);
extern void SceneNode_Enable(int a);
extern int Ov022_ActorSetState(char *self, int n);
extern char *data_ov102_020bb920;

int Ov102_PanelUpdate(char *self)
{
    VecFx32 vDelta;
    VecFx32 vTmp;
    int nRet = 0;
    char *blk = data_ov102_020bb920 + 0xc50 + 0x2000;
    int nEnergy;
    int nLimit;
    int *pLink;
    int nZero;
    int (*pfnIsDone)(char *self);

    if (Session_GetLocalPlayerIndex() == 0) {
        *(long long *)(self + 0x464) |= 0x10000;
    }
    if (Session_GetLocalPlayerIndex() == 0) {
        *(long long *)(self + 0x46c) |= 0x10000;
    }

    Ov022_StepAnchorDelta(self, &vDelta);
    if (vDelta.y != 0) {
        *(int *)(self + 0x58) = vDelta.y;
    } else if ((*(int *)(self + 0x24) & 4) == 0) {
        *(long long *)self |= 0x400000000000LL;
        *(int *)(self + 0x58) = 0;
    }

    /* Unconditional: both arms of the test above fall into this, which is why the
     * ROM materialises the literal zero twice rather than sharing one register. */
    vTmp = vDelta;
    vTmp.y = 0;
    VEC_Add((VecFx32 *)(self + 0x98 + 0x400), &vTmp,
            (VecFx32 *)(self + 0x98 + 0x400));

    if (*(int *)(blk + 8) == 0) {
        nEnergy = 0xf000;
        nLimit = 4;
        if (*(int *)blk != 0) {
            nEnergy = 0x12000;
            nLimit = 0x3e8;
        }

        /* Written as a bias of +0xfffb rather than -5: 0xfffb is not an ARM
         * immediate, so the ROM reaches it with two adds. */
        if ((u16)(*(u16 *)(self + 0x1c) + 0xfffb) <= 1 &&
            Ov022_IsState9Or6WithFlag200(self + 0x2f8 + 0x2000) != 0 &&
            *(int *)(blk + 0x124) > 0 && *(int *)(blk + 0x124) < nLimit) {
            if (*(int *)(blk + 0x120) >= 0x3000) {
                *(int *)(blk + 4) = 1;
            }
        }

        if (*(int *)(blk + 4) != 0) {
            if (*(int *)(self + 0x7b0) >= nEnergy) {
                Ov102_SpawnEffectRing(self);
                *(int *)(blk + 4) = 0;
                *(int *)(blk + 0x120) = 0;
            }
        } else {
            *(int *)(blk + 0x120) = *(int *)(blk + 0x120) + Ov022_GetGlobal34();
            if (*(int *)(blk + 0x120) >= 0x9000) {
                *(int *)(blk + 8) = 1;
            }
        }
    }

    if (*(int *)(blk + 8) == 0) {
        int nCap = 0x12000;
        int nRest = 0xc000;
        if (*(int *)blk != 0) {
            nCap = 0x15000;
            nRest = 0xf000;
        }
        if (*(int *)(self + 0x7b0) >= nCap) {
            Anim_SetFrameWrapped(*(int *)(self + 0x20) + 4, 0, nRest);
            *(int *)(self + 0x7b0) = nRest;
            *(long long *)self |= 0x20000000;
        }
    }

    pfnIsDone = *(int (**)(char *))(self + 0x668);
    ((struct PanelBits *)(self + 0x694))->bDone = pfnIsDone(self);
    if (((struct PanelBits *)(self + 0x694))->bDone) {
        *(long long *)self |= 0x2000000000000LL;
        pLink = *(int **)(self + 0x20);
        if ((*pLink & 0x20) == 0) {
            SceneNode_Enable((int)(pLink + 1));
        }
        if (Session_GetLocalPlayerIndex() == 0) {
            *(long long *)(self + 0x464) |= 2;
        }
    }

    if (((struct PanelBits *)(self + 0x694))->bDone) {
        /* One chained assignment from a named zero, and the flag OR AFTER it -- the
         * idiom the matched ov099 copy of this teardown uses. The chain evaluates
         * right to left, which is what gives the descending store order, and having
         * the zero live before the OR is what lets it take r1 while the OR's high
         * word still holds r0. */
        *(long long *)self |= 4;
        nZero = 0;
        *(int *)(self + 0x698) = *(int *)(self + 0x69c) = *(int *)(self + 0x6a0) =
            *(int *)(self + 0x498) = *(int *)(self + 0x49c) =
            *(int *)(self + 0x4a0) = nZero;
        if ((*(int *)(self + 0x24) & 4) != 0) {
            /* Two arguments, and the second is the same zero. The ROM sets up only
             * r0 before this call because r1 has held the zero since the flag OR and
             * nothing since has touched it -- that is also what keeps the zero alive
             * across the teardown, so the OR above can stay in place. Declaring this
             * callback with one argument costs the whole register assignment. */
            (**(void (**)(char *, int))(self + 0x664))(self, nZero);
            nRet = Ov022_ActorSetState(self, 0);
        } else {
            nRet = Ov022_ActorSetState(self, 2);
        }
    }
    return nRet;
}
