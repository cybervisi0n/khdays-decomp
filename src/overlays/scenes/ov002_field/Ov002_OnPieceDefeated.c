
#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct Ov002Owner {
    char pad000[0x68];
    s16 nSlotA;                 /* +0x68 */
    s16 nSlotB;                 /* +0x6a */
} Ov002Owner;

typedef struct Ov002PieceElement {
    char pad000[8];
    Ov002Owner *pOwner;         /* +0x08 */
    char pad00c[4];
    u8 nDropCtx;                /* +0x10 */
    char pad011[1];
    u16 nFlags;                 /* +0x12 */
    char pad014[8];
    VecFx32 vPlace;           /* +0x1c */
    char pad028[4];
    char aBodyNode[0x10];       /* +0x2c */
    short aAnimNode[1];         /* +0x3c */
    char pad03e[0xa2];
    int aSlotBlock[1];          /* +0xe0 */
    char pad0e4[0xd0];
    s16 nDropScale;             /* +0x1b4 */
    u8 bDropsOn : 1;            /* +0x1b6 */
    u8 nAnimCounter;            /* +0x1b7 */
} Ov002PieceElement;

/* The six amount bytes the drop dispatcher reads.  Zeroing them through this
   view rather than as a plain array is what makes mwcc hold the base in a
   register instead of folding every store onto sp. */
typedef struct Ov002DropSet {
    u8 nKind0;
    u8 nKind1;
    u8 nKind2;
    u8 nKind3;
    u8 nSpare4;
    u8 nSpare5;
} Ov002DropSet;

typedef void *Ov002StateFn(void *pElement);

extern u8 data_0204c240;                /* boot-mode flags */

extern int Rand16NextScaled(unsigned int nRange);        /* 0 .. nRange-1 */
extern int Ov002_GetRootField8d94(void);                   /* the drop multiplier */
extern void Ov002_SpawnAllDrops(const u8 *aAmount, int nCtxIndex,
                                const VecFx32 *pPlace);
extern void ReleaseNodeResources(char *pNode);
extern void Ov002_SetFieldBit0(char *pElement, int nMode);
extern void Slot_Spawn(int a, int b, void *pBlock, int d);
extern void Ov002_RebindAnimTracks(short *pAnim, int nTrack, int nFrame);
extern void SceneNode_Enable(short *pAnim);
extern void Scene_DrawNode(short *pAnim);

extern Ov002StateFn Ov002_DoneTick;
extern Ov002StateFn Ov002_TickPieceRecovery;

/* Runs the moment a piece is finished off: it pays out whatever it was carrying
   and then decides which state the piece goes to next.

   The payout is six bytes of amounts, one per drop kind, all starting at zero.
   Outside the boot mode that raises bit 2 the piece just drops a random handful
   of kinds 2 and 3; inside it, the signed scale at +0x1b4 decides instead, a
   positive one paying kind 0 and a negative one kind 2, both multiplied by the
   run's drop multiplier and rounded up through a 12-bit shift.

   A piece that is not marked for respawn is put to sleep and hands back the
   idle state; one that is keeps its slot, steps its animation counter and hands
   back the state that plays the recovery. */
Ov002StateFn *Ov002_OnPieceDefeated(Ov002PieceElement *pElement)
{
    Ov002Owner *pOwner;
    u8 aAmount[6];
    Ov002DropSet *pSet;
    int nMultiplier;

    pOwner = pElement->pOwner;
    if (pElement->bDropsOn != 0) {
        pSet = (Ov002DropSet *)aAmount;
        pSet->nKind0 = 0;
        pSet->nKind1 = 0;
        pSet->nKind2 = 0;
        pSet->nKind3 = 0;
        pSet->nSpare4 = 0;
        pSet->nSpare5 = 0;

        if ((data_0204c240 & 4) == 0) {
            aAmount[2] = (u8)Rand16NextScaled(6);
            aAmount[3] = (u8)Rand16NextScaled(6);
        } else if (pElement->nDropScale > 0) {
            nMultiplier = Ov002_GetRootField8d94();
            aAmount[0] = (u8)((pElement->nDropScale * nMultiplier + 0xfff) >> 12);
        } else if (pElement->nDropScale < 0) {
            nMultiplier = Ov002_GetRootField8d94();
            aAmount[2] = (u8)((-pElement->nDropScale * nMultiplier + 0xfff) >> 12);
        }

        Ov002_SpawnAllDrops(aAmount, pElement->nDropCtx, &pElement->vPlace);
        pElement->nFlags &= ~8;
        ReleaseNodeResources(pElement->aBodyNode);

        if ((pElement->nFlags & 4) == 0) {
            Ov002_SetFieldBit0((char *)pElement, 0);
            return Ov002_DoneTick;
        }

        if (pOwner->nSlotA >= 0 && pOwner->nSlotB >= 0) {
            Slot_Spawn(pOwner->nSlotA, pOwner->nSlotB, pElement->aSlotBlock, 0);
        }
        Ov002_RebindAnimTracks(pElement->aAnimNode, ++pElement->nAnimCounter, 0);
        SceneNode_Enable(pElement->aAnimNode);
        Scene_DrawNode(pElement->aAnimNode);
        return Ov002_TickPieceRecovery;
    }
    return 0;
}
