/* ov022: step one projectile a frame along its path.
 *
 * The mover works out this frame's delta, lets the collision pass adjust it
 * against the position it is leaving, and adds it in. The slot the projectile
 * came from is polled in the same tick. Once the projectile has travelled
 * further than its description allows, or has lived longer than its lifetime,
 * it is retired: the age is cleared and the retirement is marshalled out to
 * the other players.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Desc {
    u8 pad0000[0x14];
    int nMaxDist;                /* 0x0014 */
    int nMaxAge;                 /* 0x0018 */
};

struct Shot {
    u8 pad0000[2];
    signed char nState;          /* 0x0002 */
    u8 pad0003;
    int nAge;                    /* 0x0004 */
    u8 pad0008[8];
    VecFx32 vecStart;            /* 0x0010 */
    u8 pad001c[0xc];
    u16 nSlot;                   /* 0x0028 */
    u8 pad002a[0xa2];
    VecFx32 vecPos;              /* 0x00cc */
    u8 pad00d8[0x60];
    struct Desc *pDesc;          /* 0x0138 */
};

extern void VEC_Add(const VecFx32 *pA, const VecFx32 *pB, VecFx32 *pOut);
extern int VEC_Distance(const VecFx32 *pA, const VecFx32 *pB);
extern void Ov022_ComputeShotStep(VecFx32 *pDelta, void *pCtx, struct Shot *pShot,
                                int nFrame);
extern void Ov022_ResolveShotHit(void *pCtx, struct Shot *pShot,
                                const VecFx32 *pPos, VecFx32 *pDelta);
extern unsigned int func_ov022_02091540(u16 *pSlot, int nFrame);
extern void func_ov022_02091d80(void *pCtx, struct Shot *pShot, int nReason);

int Ov022_StepShot(void *pCtx, struct Shot *pShot, int nFrame)
{
    VecFx32 vPos;
    VecFx32 vDelta;
    struct Desc *pDesc;

    pDesc = pShot->pDesc;
    vPos = pShot->vecPos;
    Ov022_ComputeShotStep(&vDelta, pCtx, pShot, nFrame);
    Ov022_ResolveShotHit(pCtx, pShot, &vPos, &vDelta);
    VEC_Add(&vPos, &vDelta, &vPos);
    pShot->vecPos = vPos;
    func_ov022_02091540(&pShot->nSlot, nFrame);
    if (pShot->nState != 3
        && (VEC_Distance(&pShot->vecStart, &vPos) > pDesc->nMaxDist
            || pShot->nAge >= pDesc->nMaxAge)) {
        pShot->nState = 3;
    }
    if (pShot->nState == 3) {
        pShot->nAge = 0;
        func_ov022_02091d80(pCtx, pShot, 0);
    }
    return 0;
}
