
#include "nitro/fx_types.h"
#include "game/engine.h"

extern void *NNSi_FndGetCurrentRootHeap(void);
extern int Ov022_GetEntryField66(int nPlayer);
extern int Ov002_GetSlotTableByte(int nWorldId);
extern int Ov002_GetPeerRecordValue(int nSlot);
extern int VEC_Distance(const VecFx32 *a, const VecFx32 *b);
extern void VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void VEC_MultAdd(int k, const VecFx32 *a, const VecFx32 *b,
                        VecFx32 *out);
extern int VEC_Mag(const VecFx32 *v);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern int Ov002_CastRayAlongMotion(VecFx32 *pStart, VecFx32 *pMotion,
                               VecFx32 *pHitPos, VecFx32 *pOutNormal,
                               void *pExclude);
extern int Ov002_SweepSphereAlongMotion(VecFx32 *pStart, VecFx32 *pMotion,
                               VecFx32 *pHitPos, VecFx32 *pOutNormal,
                               void *pExclude);
extern int Ov002_CastRayForContactPoint(VecFx32 *pOrigin, VecFx32 *pDir,
                               VecFx32 *pOutContact, void *pExclude);

/* Pull the camera in until nothing sits between it and what it is looking at.
 *
 * Three casts run from the focus point toward the wanted camera position: the
 * ray, the sphere sweep and the plain contact query. Whichever reports the
 * nearest surface -- nearer than the shot itself -- wins, and the camera moves
 * to it. If none of them hits, one more ray runs the other way, from the wanted
 * position back toward the focus, to catch a camera that starts inside geometry.
 *
 * What follows is the vertical pass. A ceiling comes from the world's own limit,
 * lowered by anything hanging just above the camera, and the camera is clamped
 * under it. Then a ray straight down decides whether the camera is standing too
 * close to the floor, in which case it is lifted one 0xc00 clearance along the
 * surface normal. Finally, unless the context asks to skip it, a camera that has
 * ended up almost on top of the focus in the horizontal plane is pushed back out
 * to 0xc00 from it.
 *
 * The out-of-world early exit returns nothing at all: it is a bare `return;`,
 * which mwcc accepts here and which is why no return register is written on that
 * path. That is what the ROM does; giving it a value costs an instruction.
 */
int Ov002_ResolveCameraAgainstCollision(VecFx32 *pOutFocus, VecFx32 *pOutCamera,
                        VecFx32 *pFocus, VecFx32 *pCamera)
{
    VecFx32 vContact;
    VecFx32 vDir;
    VecFx32 vNormal;
    VecFx32 vCam;
    VecFx32 vFocus;
    VecFx32 vToCam;
    VecFx32 vProbe;
    VecFx32 vFlat;
    VecFx32 vCamFlat;
    VecFx32 vFocusFlat;
    VecFx32 vPush;
    int bDirect;
    int nResult;
    void *pCtx;
    void *pExclude;
    int nPlayer;
    int nWorldId;
    int bHit;
    int nBest;
    int nDist;
    int nCeil;
    int nLen;

    pCtx = NNSi_FndGetCurrentRootHeap();
    nPlayer = QueryActiveStateOrDelegate();
    nWorldId = Ov022_GetEntryField66(nPlayer);
    pExclude = 0;
    vCam = *pCamera;
    bHit = 0;
    bDirect = 0;
    vFocus = *pFocus;
    nResult = 0;
    if (nPlayer < 0 || nWorldId < 0) {
        return;
    }

    if (GetEntryField20ByIndex(nPlayer) != 0) {
        pExclude = *(void **)((char *)GetEntryField20ByIndex(nPlayer) + 0x20);
    }

    vContact.z = 0;
    vContact.y = 0;
    vContact.x = 0;
    nBest = 0x7fffffff;
    VEC_Subtract(pCamera, pFocus, &vDir);

    if (Ov002_CastRayAlongMotion(pFocus, &vDir, &vContact, &vNormal, pExclude)
        != 0) {
        nDist = VEC_Distance(pFocus, &vContact);
        if (nDist < nBest && nDist < VEC_Mag(&vDir)) {
            vCam = vContact;
            bHit = 1;
            nBest = nDist;
        }
    }

    if (Ov002_SweepSphereAlongMotion(pFocus, &vDir, &vContact, &vNormal, pExclude)
        != 0) {
        nDist = VEC_Distance(pFocus, &vContact);
        if (nDist < nBest && nDist < VEC_Mag(&vDir)) {
            vCam = vContact;
            bHit = 1;
            nBest = nDist;
        }
    }

    if (Ov002_CastRayForContactPoint(pFocus, &vDir, &vContact, pExclude) != 0) {
        nDist = VEC_Distance(pFocus, &vContact);
        if (nDist < nBest && nDist < VEC_Mag(&vDir)) {
            bHit = 1;
            vCam = vContact;
            bDirect = 1;
        }
    }

    if (!bHit) {
        VEC_Subtract(pFocus, pCamera, &vDir);
        if (Ov002_CastRayAlongMotion(pCamera, &vDir, &vContact, &vNormal, pExclude)
            != 0) {
            nLen = VEC_Mag(&vDir);
            nDist = VEC_Distance(pCamera, &vContact);
            if (nLen > nDist) {
                vCam = vContact;
                bHit = 1;
            }
        }
    }

    if (bHit) {
        VEC_Subtract(&vCam, (VecFx32 *)((char *)pCtx + 0x20), &vToCam);
        VEC_Subtract(pFocus, pCamera, &vDir);
        if (VEC_Mag(&vToCam) > 0x3000
            && VEC_DotProduct(&vToCam, &vDir) > 0) {
            *(int *)((char *)pCtx + 0x9c) = 1;
        }
    }

    nCeil = Ov002_GetPeerRecordValue(Ov002_GetSlotTableByte(nWorldId));
    vDir.x = 0;
    vDir.y = 0xa000;
    vDir.z = 0;
    vProbe.x = vCam.x;
    vProbe.y = vCam.y - 0x800;
    vProbe.z = vCam.z;
    if (bDirect) {
        if (nCeil > vProbe.y) {
            nCeil = vProbe.y;
        }
    } else {
        if (Ov002_CastRayForContactPoint(&vProbe, &vDir, &vContact, pExclude) != 0
            && vContact.y - vCam.y < 0xc00) {
            if (nCeil > vContact.y - 0xc00) {
                nCeil = vContact.y - 0xc00;
            }
        }
    }
    if (nCeil < vCam.y) {
        vCam.y = nCeil;
    }

    vDir.x = 0;
    vDir.y = -0xa000;
    vDir.z = 0;
    vProbe.x = vCam.x;
    vProbe.y = vCam.y + 0x800;
    vProbe.z = vCam.z;
    if (Ov002_CastRayAlongMotion(&vProbe, &vDir, &vContact, &vNormal, pExclude)
        != 0) {
        nDist = VEC_Distance(&vCam, &vContact);
        VEC_Subtract(&vCam, &vContact, &vFlat);
        VEC_Normalize(&vFlat, &vFlat);
        if (nDist < 0xc00 || VEC_DotProduct(&vFlat, &vNormal) < 0) {
            VEC_MultAdd(0xc00, &vNormal, &vContact, &vCam);
        }
    }

    if ((*(unsigned int *)((char *)pCtx + 0x38) & 0x1000000) == 0) {
        vCamFlat = vCam;
        vFocusFlat = *pFocus;
        vCamFlat.y = 0;
        vFocusFlat.y = 0;
        if (VEC_Distance(&vCamFlat, &vFocusFlat) < 0xc00) {
            VEC_Subtract(pCamera, pFocus, &vPush);
            VEC_Normalize(&vPush, &vPush);
            VEC_MultAdd(0xc00, &vPush, pFocus, &vCam);
            if (nCeil < vCam.y) {
                vCam.y = nCeil;
            }
        }
        nResult = 1;
    }

    *pOutCamera = vCam;
    *pOutFocus = vFocus;
    return nResult;
}
