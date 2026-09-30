/* Ov021_PrizeBoxStart -- Ov021_PrizeBoxStart: start handler of the prize box.  Marks the model
 * bound (bit 2 of +0x12) and, when the definition names a model (def +0x58), takes a copy of
 * the position (+0xe0), binds the model node (+0x2c, 0202b930 mode 1 / 4 on the resource from
 * ov002 0206da70), copies the facing (+0x18) into the transform (+0xb8, flag bit 5 of +0x3c)
 * unless it already has one (bit 5 of +0x38), puts the transform at the position (0202b450),
 * enables the node (0202af1c) but leaves it hidden (0202bedc 0) and plays the current track
 * (+0x1bc) with the animation length / frame (+0x1b4 / +0x1b0) through Ov021_PrizeBoxPlayAnim
 * (0207fa40).  Finally requests the resource pair of the box's effect: id 0x3f in mission
 * 0x41d (ov002 0206b84c), 0x38 elsewhere (0203355c). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct Ov021PrizeBoxDef {
    u8   pad_00[0x58];
    char szModel[0x10];       /* 0x58 */
} Ov021PrizeBoxDef;

typedef struct Ov021PrizeBox {
    u8   pad_000[8];
    Ov021PrizeBoxDef *pDef;   /* 0x008 */
    u8   pad_00c[6];
    u16  nPieceFlags;         /* 0x012: bit 2 = model bound */
    u8   pad_014[4];
    u16  nFacing;             /* 0x018 */
    u8   pad_01a[0x2c - 0x1a];
    u8   renderNode[0x38 - 0x2c]; /* 0x02c */
    u32  nNodeFlagsA;         /* 0x038: bit 5 = facing set; the transform */
    u16  nNodeFlagsB;         /* 0x03c: the model node */
    u8   pad_03e[0xb8 - 0x3e];
    u16  nNodeFacing;         /* 0x0b8 */
    u8   pad_0ba[0xe0 - 0xba];
    VecFx32 position;         /* 0x0e0 */
    u8   pad_0ec[0x1b0 - 0xec];
    int  nFrame;              /* 0x1b0 */
    int  nLength;             /* 0x1b4 */
    u8   pad_1b8[4];
    char nTrack;              /* 0x1bc */
} Ov021PrizeBox;

extern void *Ov002_LookupChannelEntry(const char *pName);                 /* name -> resource entry */
extern void  Ov021_StoreArgsRunTwoSubActionsIfFlag4(Ov021PrizeBox *pSelf, void *pNode, int nTrack, int nLength, int nFrame); /* Ov021_PrizeBoxPlayAnim */
extern int   Ov002_GetStateWord(void);                              /* the mission id */

void Ov021_PrizeBoxStart(Ov021PrizeBox *pSelf)
{
    VecFx32 position;
    Ov021PrizeBoxDef *pDef;
    u16 nFacing;

    pDef = pSelf->pDef;
    pSelf->nPieceFlags |= 4;
    if (pDef->szModel[0] != 0) {
        position = pSelf->position;
        Entity_Register(pSelf->renderNode, Ov002_LookupChannelEntry(pDef->szModel), 1, 4);
        nFacing = pSelf->nFacing;
        if ((pSelf->nNodeFlagsA & 0x20) == 0) {
            pSelf->nNodeFacing = nFacing;
            pSelf->nNodeFlagsB |= 0x20;
        }
        Actor_SetVecAndSyncChild(&pSelf->nNodeFlagsA, &position);
        SceneNode_Enable(&pSelf->nNodeFlagsB);
        Obj_SetFlagBit3(pSelf->renderNode, 0);
        Ov021_StoreArgsRunTwoSubActionsIfFlag4(pSelf, &pSelf->nNodeFlagsB, pSelf->nTrack, pSelf->nLength, pSelf->nFrame);
    }
    Res_RequestIdPair(Ov002_GetStateWord() == 0x41d ? 0x3f : 0x38);
}
