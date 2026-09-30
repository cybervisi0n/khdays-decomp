
#include "nitro/fx_types.h"

extern void *NNSi_FndGetCurrentRootHeap(void);
extern int QueryActiveStateOrDelegate(void);
extern void *Obj_GetCurrent(void);   /* the active scene */

extern VecFx32 *func_ov022_020881f8(int nPlayer);
extern int func_ov022_02083f5c(void);
extern int func_ov022_020881d8(void);
extern int func_ov022_02088338(void);

extern void Ov002_PlaceCameraForFrame(VecFx32 *pOutFocus, VecFx32 *pOutCamera,
                                int *pOutDist, VecFx32 *pAnchor, int nAngle,
                                int nDist, int nPrevDist);
extern void Ov002_TickCameraTransition(void *pScene);
extern int Ov002_GetCameraDistance(int nSelector);
extern int Ov002_Camera_GetPresetHeight(int nSelector);

extern unsigned char data_0204be04;
extern short data_0203d210[];   /* angle sin/cos table, 4 bytes per entry */

/* Reset the camera to its default pose and publish it in one go.
 */
void *Ov002_ResetCamera(void)
{
    char *pCam;
    int nPlayer;
    VecFx32 *pAnchor;

    pCam = (char *)NNSi_FndGetCurrentRootHeap();
    nPlayer = QueryActiveStateOrDelegate();
    pAnchor = func_ov022_020881f8(nPlayer);
    func_ov022_02083f5c();

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

    *(unsigned int *)(pCam + 0x38) &= 0xfffcdfff;
    *(int *)(pCam + 0x60) = Ov002_GetCameraDistance(0);
    *(int *)(pCam + 0x5c) = Ov002_Camera_GetPresetHeight(0);
    *(unsigned short *)(pCam + 0xa0) = 0x1555;
    *(unsigned short *)(pCam + 0xa2) = 0x1555;
    *(int *)pCam =
        data_0203d210[(*(unsigned short *)(pCam + 0xa0) >> 4) * 2];
    *(int *)(pCam + 4) =
        data_0203d210[(*(unsigned short *)(pCam + 0xa2) >> 4) * 2 + 1];

    Ov002_PlaceCameraForFrame((VecFx32 *)(pCam + 0x70), (VecFx32 *)(pCam + 0x64),
                        (int *)(pCam + 0x7c), pAnchor,
                        *(int *)(pCam + 0x58), 0x3000, 0x3000);

    *(VecFx32 *)(pCam + 0x20) = *(VecFx32 *)(pCam + 0x64);
    *(VecFx32 *)(pCam + 0x14) = *(VecFx32 *)(pCam + 0x70);
    *(int *)(pCam + 0x54) = *(int *)(pCam + 0x7c);
    *(int *)(pCam + 0x9c) = 1;
    *(int *)(pCam + 0xb4) = 0x1f;

    Ov002_TickCameraTransition(Obj_GetCurrent());
    return 0;
}
