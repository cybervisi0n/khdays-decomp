
#include "nitro/fx_types.h"

extern void *NNSi_FndGetCurrentRootHeap(void);
extern int QueryActiveStateOrDelegate(void);
extern void *GetEntryField20ByIndex(int nPlayer);
extern int FX_Sqrt(int v);
extern void VEC_Normalize(const VecFx32 *v, VecFx32 *out);   /* normalise */
extern int VEC_Distance(const VecFx32 *a, const VecFx32 *b);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void VEC_MultAdd(int k, const VecFx32 *a, const VecFx32 *b,
                        VecFx32 *out);
extern int Ov002_ResolveCameraAgainstCollision(VecFx32 *pOutFocus, VecFx32 *pOutCamera,
                               VecFx32 *pFocus, VecFx32 *pCamera);

extern short data_0203d210[];   /* angle sin/cos table, 4 bytes per entry */

static inline int FX_Mul(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

/* Place the camera for this frame.
 *
 * The focus is either the camera state's own look-at point or the caller's
 * anchor raised by the state's ground offset, depending on one bit of the
 * player entry's 64-bit flag word. The eye then swings out around the state's
 * look-at point: the horizontal reach is the wanted distance with the vertical
 * offset taken out by Pythagoras, clamped to the wanted distance above and to
 * 0xc00 below, and the angle indexes the shared sin/cos table. That first
 * placement is only a direction, so it is normalised and re-scaled to the full
 * wanted distance before the collision resolver gets to pull it in.
 *
 * The resolver is skipped when the state asks for it, and also in camera mode 4
 * while the player entry is in state 2. Finally the reported distance sticks to
 * the previous frame's value while it is within 0x10, which stops the camera
 * jittering between two nearly equal solutions.
 *
 * Three shapes carry the codegen, and they are the same shape three times: read
 * the components into scalars and write them all afterwards. mwcc merges an
 * adjacent triple of loads into one ldm and an adjacent triple of stores into
 * one stm, but only when nothing is interleaved between them, so both the focus
 * copy and the eye assembly are written that way, and the eye components are
 * computed Z, X, Y as the ROM's dataflow does. The last one is the distance
 * clamp, written as two stores rather than one store of a clamped value, which
 * keeps the address of the eye vector out of the slot before it.
 */
void Ov002_PlaceCameraForFrame(VecFx32 *pOutFocus, VecFx32 *pOutCamera,
                         int *pOutDist, VecFx32 *pAnchor, int nAngle,
                         int nDist, int nPrevDist)
{
    VecFx32 vEye;
    VecFx32 vAway;
    VecFx32 vScratch;
    int nX;
    int nY;
    int nZ;
    void *pCam;
    void *pEntry;
    int nHeight;
    int nRadius;
    int nIndex;
    int nOut;
    int nDelta;
    int nEyeX;
    int nEyeY;
    int nEyeZ;

    pCam = NNSi_FndGetCurrentRootHeap();
    pEntry = GetEntryField20ByIndex(QueryActiveStateOrDelegate());

    if ((*(unsigned long long *)((char *)pEntry + 0x464) & 0x400000000ULL)
        != 0) {
        *pOutFocus = *(VecFx32 *)((char *)pCam + 0x14);
    } else {
        nX = pAnchor->x;
        nZ = pAnchor->z;
        nY = pAnchor->y + *(int *)((char *)pCam + 0x60);
        pOutFocus->x = nX;
        pOutFocus->y = nY;
        pOutFocus->z = nZ;
    }

    nHeight = *(int *)((char *)pCam + 0x5c);
    nRadius = FX_Sqrt(FX_Mul(nDist, nDist) - FX_Mul(nHeight, nHeight));
    if (nRadius > nDist) {
        nRadius = nDist;
    } else if (nRadius < 0xc00) {
        nRadius = 0xc00;
    }

    nIndex = nAngle >> 4;
    nEyeZ = ((VecFx32 *)((char *)pCam + 0x14))->z
            + FX_Mul(data_0203d210[nIndex * 2 + 1], nRadius);
    nEyeX = ((VecFx32 *)((char *)pCam + 0x14))->x
            + FX_Mul(data_0203d210[nIndex * 2], nRadius);
    nEyeY = ((VecFx32 *)((char *)pCam + 0x14))->y
            + *(int *)((char *)pCam + 0x5c);
    vEye.x = nEyeX;
    vEye.y = nEyeY;
    vEye.z = nEyeZ;

    VEC_Subtract(&vEye, (VecFx32 *)((char *)pCam + 0x14), &vAway);
    VEC_Normalize(&vAway, &vAway);
    VEC_MultAdd(nDist, &vAway, (VecFx32 *)((char *)pCam + 0x14), &vEye);

    if ((*(unsigned int *)((char *)pCam + 0x38) & 0x10000) == 0) {
        if (*(int *)((char *)pCam + 0x4c) != 4
            || *(int *)((char *)pEntry + 0xc) != 2) {
            Ov002_ResolveCameraAgainstCollision(&vScratch, &vEye,
                                (VecFx32 *)((char *)pCam + 0x14), &vEye);
        }
    }

    nOut = VEC_Distance((VecFx32 *)((char *)pCam + 0x14), &vEye);
    nDelta = nOut - nPrevDist;
    if (nDelta < 0) {
        nDelta = -nDelta;
    }
    if (nDelta <= 0x10) {
        *pOutDist = nPrevDist;
    } else {
        *pOutDist = nOut;
    }
    *pOutCamera = vEye;
}
