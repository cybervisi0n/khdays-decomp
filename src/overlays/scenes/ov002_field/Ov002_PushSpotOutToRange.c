
/* A stored spot: a position plus the index of the party slot it belongs to. */

#include "nitro/fx_types.h"

typedef struct {
    VecFx32 vPos;                      /* +0x00 */
    unsigned char bSlot;            /* +0x0c */
} Ov002Spot;

extern int VEC_Mag(const VecFx32 *v);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *ab);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *pUnit);
extern void ScaleVec3Fx12(int nFactor, const VecFx32 *pSrc, VecFx32 *pDst);
extern VecFx32 *Ov002_Element_CallHook2C(int nObj);
extern int func_ov022_020882f8(void);
extern VecFx32 *func_ov022_020881f8(int nSlot);

/* Pull a spot in towards its party member when it is closer than a radius.
 *
 * The spot's horizontal distance is measured with its height zeroed. If that
 * is inside the radius and the spot belongs to a live party slot, the spot is
 * replaced by the direction from the object to that member, scaled out to the
 * radius. The original height is put back either way.
 */
void Ov002_PushSpotOutToRange(int nObj, const Ov002Spot *pSpot, int nRange,
                         VecFx32 *pOut)
{
    int nSlot;
    VecFx32 *pFrom;

    *pOut = pSpot->vPos;
    pOut->y = 0;

    if (nRange > VEC_Mag(pOut)) {
        nSlot = pSpot->bSlot;
        if (nSlot < func_ov022_020882f8()) {
            pFrom = Ov002_Element_CallHook2C(nObj);
            VEC_Subtract(pFrom, func_ov022_020881f8(nSlot), pOut);
            VEC_Normalize(pOut, pOut);
            ScaleVec3Fx12(nRange, pOut, pOut);
        }
    }

    pOut->y = pSpot->vPos.y;
}
