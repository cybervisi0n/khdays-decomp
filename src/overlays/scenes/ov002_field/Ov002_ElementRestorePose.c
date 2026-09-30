
#include "nitro/types.h"
#include "nitro/fx_types.h"

extern u8 data_0204c240;

extern int Ov002_LookupChannelEntry(void *pName);
extern void Entity_Register(char *pObj, int nRes, int a, int b);
extern void Actor_SetVecAndSyncChild(char *pNode, VecFx32 *pPos);
extern int Ov002_GetLastPositiveSlotValue(u16 *pAnim);
extern void Ov002_RebindAnimTracks(short *pAnim, int nTrack, int nFrame);
extern void SceneNode_Enable(u16 *pAnim);
extern void SceneNode_Disable(u16 *pAnim);
extern unsigned int GameState_GetField(int nId, int nSlot);
extern void Obj_SetFlagBit3(char *pObj, int bOn);
extern void Ov002_ForwardLinkEventKind1(int nKind, VecFx32 *pPos, int nParam);

/* Bring an actor element's model back after its owner has been rebound.
 *
 * The cached position is read first because binding the model moves the node,
 * then the node goes back there and the first placement remembers the angle.
 * The pose the model reports decides the track: one particular pose, with the
 * element ready and the global gate open, plays the alternate track instead of
 * the idle one. The object follows the game-state bit again, and the element's
 * pending bit is dropped once the work is done.
 */
void Ov002_ElementRestorePose(char *pElement)
{
    VecFx32 vPos;
    const VecFx32 *pCached;
    void *pOwnerName;
    int nState;
    int bVisible;
    u16 wAngle;

    pOwnerName = *(char **)(pElement + 8) + 0x58;
    pCached = (const VecFx32 *)(pElement + 0x1c);
    vPos.x = pCached->x;
    vPos.y = pCached->y;
    vPos.z = pCached->z;
    *(signed char *)(pElement + 0x1b8) = 0;

    Entity_Register(pElement + 0x2c, Ov002_LookupChannelEntry(pOwnerName), 1, 4);

    wAngle = *(u16 *)(pElement + 0x18);
    if ((*(unsigned int *)(pElement + 0x38) & 0x20) == 0) {
        *(u16 *)(pElement + 0xb8) = wAngle;
        *(u16 *)(pElement + 0x3c) |= 0x20;
    }

    Actor_SetVecAndSyncChild(pElement + 0x38, &vPos);

    *(signed char *)(pElement + 0x1b4) =
        (signed char)Ov002_GetLastPositiveSlotValue((u16 *)(pElement + 0x3c));

    if (*(signed char *)(pElement + 0x1b4) == 3
        && (*(u8 *)(pElement + 0x1b5) & 1) == 0
        && (data_0204c240 & 4) == 0
        && (*(u8 *)(pElement + 0x1b5) & 0x80) != 0) {
        *(signed char *)(pElement + 0x1b8) = 2;
    }

    Ov002_RebindAnimTracks((short *)(pElement + 0x3c),
                        *(signed char *)(pElement + 0x1b8), 0);

    if (*(signed char *)(pElement + 0x1b8) != 0) {
        SceneNode_Enable((u16 *)(pElement + 0x3c));
        *(u8 *)(pElement + 0x1b5) |= 2;
    } else {
        SceneNode_Disable((u16 *)(pElement + 0x3c));
        *(u8 *)(pElement + 0x1b5) |= 1;
    }

    nState = GameState_GetField(*(u16 *)(pElement + 0x14), *(u8 *)(pElement + 0x16));
    Obj_SetFlagBit3(pElement + 0x2c, (nState & 1) != 0);

    bVisible = (GameState_GetField(*(u16 *)(pElement + 0x14),
                              *(u8 *)(pElement + 0x16)) & 1) != 0;
    if (bVisible) {
        Ov002_ForwardLinkEventKind1(1, &vPos, 0);
    }

    *(u8 *)(pElement + 0x1b5) &= ~0x80;
}
