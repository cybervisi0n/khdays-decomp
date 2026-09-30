
#include "nitro/fx_types.h"

extern void *NNSi_FndGetCurrentRootHeap(void);
extern int QueryActiveStateOrDelegate(void);
extern void *Obj_GetCurrent(void);   /* the active scene */

extern VecFx32 *func_ov022_020881f8(int player);
extern int func_ov022_02083f5c(void);
extern int func_ov022_020881d8(void);
extern int func_ov022_02088338(void);
extern int Ov022_IsBit2SetVia0x20(int nHandle);
extern int func_ov022_020886d0(int nPlayer);
extern unsigned short func_ov022_02088254(int nPlayer);

extern void Ov002_Camera_UpdateFollow(void);
extern int Ov002_IsAngleBeyondLimit(int nAngleA, int nAngleB);
extern int Ov002_TurnAngleToward(int nAngleA, int nAngleB);
extern void Ov002_PlaceCameraForFrame(VecFx32 *pOutFocus, VecFx32 *pOutCamera,
                                int *pOutDist, VecFx32 *pAnchor, int nAngle,
                                int nDist, int nPrevDist);
extern void Ov002_TickCameraTransition(void *pScene);
extern int Ov002_GetCameraDistance(int nSelector);
extern int Ov002_Camera_GetPresetHeight(int nSelector);
extern int Ov002_UpdateCameraDistance(int nSelector);

extern unsigned char data_0204be04;
extern unsigned short data_0204c190;

/* Locked-yaw camera tick: hold the camera at the angle the session dictates,
 * and hand back the ordinary handler as soon as it stops dictating.
 */
void *Ov002_TickLockedCamera(void)
{
    char *pCam;
    void *pNext;
    int nPlayer;
    VecFx32 *pAnchor;
    int nHandle;
    int nDist;

    pCam = (char *)NNSi_FndGetCurrentRootHeap();
    pNext = 0;
    nPlayer = QueryActiveStateOrDelegate();
    pAnchor = func_ov022_020881f8(nPlayer);
    nHandle = func_ov022_02083f5c();

    if (data_0204be04 != *(unsigned int *)(pCam + 0xe0)) {
        return 0;
    }
    if ((*(unsigned int *)(pCam + 0x38) & 4) != 0) {
        return 0;
    }
    if (func_ov022_020881d8() != 0) {
        return 0;
    }
    if (func_ov022_02088338() == 0) {
        return 0;
    }

    if ((*(unsigned int *)(pCam + 0x38) & 0x4000) == 0) {
        *(int *)(pCam + 0x84) = Ov002_Camera_GetPresetHeight(*(int *)(pCam + 0x44));
    }
    *(int *)(pCam + 0x88) = Ov002_GetCameraDistance(*(int *)(pCam + 0x44));

    if ((data_0204c190 & 0x100) != 0 && (data_0204c190 & 0x200) == 0) {
        *(int *)(pCam + 0x50) = func_ov022_02088254(nPlayer);
    }

    if (Ov002_IsAngleBeyondLimit(*(int *)(pCam + 0x58), *(int *)(pCam + 0x50)) == 0
        || Ov022_IsBit2SetVia0x20(nHandle) != 0
        || func_ov022_020886d0(nPlayer) != 0) {
        *(int *)(pCam + 0x40) = 0;
        pNext = (void *)Ov002_Camera_UpdateFollow;
    } else {
        *(int *)(pCam + 0x80) =
            Ov002_TurnAngleToward(*(int *)(pCam + 0x58), *(int *)(pCam + 0x50));
        nDist = Ov002_UpdateCameraDistance(*(int *)(pCam + 0x44));
        Ov002_PlaceCameraForFrame((VecFx32 *)(pCam + 0x70), (VecFx32 *)(pCam + 0x64),
                            (int *)(pCam + 0x7c), pAnchor,
                            *(int *)(pCam + 0x80), nDist,
                            *(int *)(pCam + 0x7c));
    }

    Ov002_TickCameraTransition(Obj_GetCurrent());
    return pNext;
}
