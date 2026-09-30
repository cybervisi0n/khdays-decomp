
#include "nitro/types.h"
#include "nitro/fx_types.h"

extern int Ov002_LookupChannelEntry(void *pName);
extern void Entity_Register(char *pObj, int nRes, int a, int b);
extern void Actor_SetVecAndSyncChild(char *pNode, VecFx32 *pPos);
extern void Ov002_RebindAnimTracks(short *pAnim, int nBlend, int nFrame);
extern void SceneNode_Disable(u16 *pNode);
extern int GameState_GetField(int nId, int nSlot);
extern void Obj_SetFlagBit3(char *pObj, int bOn);
extern void Obj_SetTransition(char *pObj, int bFlag, int nParam);

/* Bring an element's model back after its owner has been rebound.
 *
 * The cached position is read first because binding the model moves the node,
 * then the node is put back there, the first placement remembers the angle,
 * the element is marked as driving its table, its track is blended in from
 * frame zero, and the object follows the game-state bit again.
 */
void Ov002_ElementRestoreModel(char *pElement)
{
    char *pOwner;
    VecFx32 vPos;
    u16 wAngle;
    int nState;

    pOwner = *(char **)(pElement + 8);

    if (*(signed char *)(pOwner + 0x58) != 0) {
        vPos = *(VecFx32 *)(pElement + 0xd0);

        Entity_Register(pElement + 0x1c, Ov002_LookupChannelEntry(pOwner + 0x58),
                      1, 4);
        Actor_SetVecAndSyncChild(pElement + 0x28, &vPos);

        wAngle = *(u16 *)(pElement + 0x18);
        if ((*(unsigned int *)(pElement + 0x28) & 0x20) == 0) {
            *(u16 *)(pElement + 0xa8) = wAngle;
            *(u16 *)(pElement + 0x2c) |= 0x20;
        }

        *(u16 *)(pElement + 0x12) |= 4;

        Ov002_RebindAnimTracks((short *)(pElement + 0x2c),
                            *(signed char *)(pElement + 0x1a0), 0);
        SceneNode_Disable((u16 *)(pElement + 0x2c));

        nState = GameState_GetField(*(u16 *)(pElement + 0x14), *(unsigned char *)(pElement + 0x16));
        Obj_SetFlagBit3(pElement + 0x1c, (nState & 1) != 0);

        Obj_SetTransition(pElement + 0x1c,
                      (*(u16 *)(pElement + 0x12) & 0x100) != 0,
                      *(short *)(pElement + 0x1a2));
    }
}
