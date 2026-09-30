
/* One entry of the session screen's marker table. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct Ov002SessionMarker {
    VecFx32 place;
    int nOwner;
    int nKind;              /* left alone when the caller passes -1 */
    char szName[0x30];
} Ov002SessionMarker;

typedef struct Ov002SessionBlock {
    char pad00[4];
    Ov002SessionMarker *pMarkers;
} Ov002SessionBlock;

extern char *data_ov002_0207fa00;

extern int Ov002_GetCtxTableByte(int nKind);      /* kind -> table byte */
extern void Ov002_ResolveNamedPlacement(const char *pName, int nSlot,
                                VecFx32 *pPlace, int *pOwner, int nIndex);
extern void Ov002_ScatterPlaceByIndex(VecFx32 *pPlace, int nOwner, int nIndex,
                                VecFx32 *pOut);
extern void strcpy(char *pDst, const char *pSrc);

/* Fills in one marker of the session screen's table and always returns 1.  An
   nIndex of -1 means take the slot QueryActiveStateOrDelegate hands back, but only the table
   write uses that substitute: the placement solver still gets the caller's
   original index.  With a name, the kind is resolved to a slot and, if it
   resolves, the placement is handed to Ov002_ResolveNamedPlacement and the name is
   dropped so it is not copied again below.  Without a name, the placement goes
   through Ov002_ScatterPlaceByIndex unless the caller skips it or flag 0x20e7 is
   already set. */
int Ov002_WriteSessionMarker(int nIndex, int nKind, const VecFx32 *pPlace,
                        int nOwner, const char *pName, int bSkipSolver)
{
    Ov002SessionBlock *pBlock;
    VecFx32 out;
    VecFx32 place;
    int nOwnerLocal;
    int nIdx;
    const char *pPending;
    int nSlot;

    pBlock = (Ov002SessionBlock *)(data_ov002_0207fa00 + 0x8bcc);
    pPending = pName;
    place = *pPlace;
    nIdx = nIndex;
    nOwnerLocal = nOwner;

    if (nIdx == -1) {
        nIdx = QueryActiveStateOrDelegate();
    }

    if (pName != 0) {
        nSlot = Ov002_GetCtxTableByte(nKind);
        if (nSlot >= 0) {
            Ov002_ResolveNamedPlacement(pName, nSlot, &place, &nOwnerLocal, nIdx);
            pPending = 0;
        }
        out = place;
    } else if (bSkipSolver == 0 && GameState_IsFlagSet(0x20e7) == 0) {
        Ov002_ScatterPlaceByIndex(&place, nOwnerLocal, nIndex, &out);
    } else {
        out = place;
    }

    pBlock->pMarkers[nIdx].place = out;
    pBlock->pMarkers[nIdx].nOwner = nOwnerLocal;
    if (nKind != -1) {
        pBlock->pMarkers[nIdx].nKind = nKind;
    }

    if (pPending != 0) {
        strcpy(pBlock->pMarkers[nIdx].szName, pPending);
    } else {
        pBlock->pMarkers[nIdx].szName[0] = 0;
    }
    return 1;
}
