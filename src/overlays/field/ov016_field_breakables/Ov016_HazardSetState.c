/* Ov016_HazardSetState -- Ov016_HazardSetState: switch the hazard on or off.  Writes the state
 * into bit 1 of the hazard's GameState field (GameState_GetField / SetField on +0x14 / +0x16),
 * rewinds the model node (+0x3c) to track 1 (on) or 0 (off) and disables it while the node is
 * bound (bit 2 of the node byte at +0x34); when asked to spawn (bSpawn) and the definition has
 * a drop slot (def +0x68) with an id for the new state (def +0x6a when switching on, +0x6c when
 * switching off) that drop is spawned at the hazard's position (+0xe0, 02033d0c). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct Ov016HazardDef {
    u8 pad_00[0x68];
    short nDropSlot;          /* 0x68 */
    short aDropId[2];         /* 0x6a: [0] when switched on, [1] when switched off */
} Ov016HazardDef;

typedef struct Ov016Hazard {
    u8 pad_000[0x8];
    Ov016HazardDef *pDef;     /* 0x08 */
    u8 pad_00c[0x8];
    u16 nStateField;          /* 0x14: GameState field */
    u8  nStateBit;            /* 0x16 */
    u8  pad_017[0x34 - 0x17];
    u8  nNodeFlags34;         /* 0x34: bit 2 = model bound */
    u8  pad_035[0x3c - 0x35];
    u16 nNodeFlagsB;          /* 0x3c: the model node */
    u8  pad_03e[0xe0 - 0x3e];
    VecFx32 position;         /* 0xe0 */
} Ov016Hazard;

extern int  GameState_GetField(int nField, int nBit);                      /* GameState_GetField */
extern void GameState_SetField(unsigned int nField, unsigned int nBit, unsigned int nValue);          /* GameState_SetField */
extern void Ov002_RebindAnimTracks(void *pNode, int nTrack, int nFrame); /* rewind a sequence */
extern void SceneNode_Disable(void *pNode);                              /* SceneNode_Disable */
extern int  Slot_Spawn(int nSlot, int nId, VecFx32 *pPos, unsigned int nFlags); /* Slot_Spawn */

void Ov016_HazardSetState(Ov016Hazard *pSelf, int bState, int bSpawn)
{
    Ov016HazardDef *pDef;
    int nState;
    int nTrack;
    u16 nOn;

    nTrack = (bState != 0);
    pDef = pSelf->pDef;
    nOn = (bState ? 1 : 0);
    nState = GameState_GetField((u16)pSelf->nStateField, (u8)pSelf->nStateBit);
    GameState_SetField((u16)pSelf->nStateField, (u8)pSelf->nStateBit,
                       (u16)((nOn << 1) | (nState & 0xffff0001)));
    if (pSelf->nNodeFlags34 & 4) {
        Ov002_RebindAnimTracks(&pSelf->nNodeFlagsB, nTrack, 0);
        SceneNode_Disable(&pSelf->nNodeFlagsB);
    }
    if (bSpawn && pDef->nDropSlot >= 0 && pDef->aDropId[bState == 0] >= 0) {
        Slot_Spawn(pDef->nDropSlot, pDef->aDropId[bState == 0], &pSelf->position, (u16)0);
    }
}
