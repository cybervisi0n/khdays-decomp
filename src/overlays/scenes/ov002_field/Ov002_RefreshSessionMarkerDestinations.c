#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct Ov002SessionMarker {
    VecFx32 place;
    int nOwner;
    int nKind;
    char szName[0x30];
} Ov002SessionMarker;
typedef struct Ov002SessionBlock {
    int nSessionToken;
    Ov002SessionMarker *pMarkers;
} Ov002SessionBlock;
typedef struct Ov002SessionActorFlags {
    unsigned long long qwFlags;
    unsigned char pad008[0x45c];
    unsigned int dwStateFlags;
} Ov002SessionActorFlags;

extern char *data_ov002_0207fa00;
extern char *strcpy(char *pDest, const char *pSource);
extern int Ov002_GetCtxTableByte(int nDestination);
extern int Ov002_FindCodeOwner(int nCode, int *pSlot, int *pDestination);
extern int func_ov022_020882f8(void);
extern void Ov002_ResolveNamedPlacement(const char *pName, int nSlot,
    VecFx32 *pPlace, int *pExtra, int nPlayer);

/* Refresh marker destinations and optional named placements after a code change. */
void Ov002_RefreshSessionMarkerDestinations(void)
{
    char *pRoot = data_ov002_0207fa00;
    Ov002SessionBlock *pSession = (Ov002SessionBlock *)(pRoot + 0x8bcc);
    int nLocalPlayer = QueryActiveStateOrDelegate();
    Ov002SessionMarker *pMarker = &pSession->pMarkers[nLocalPlayer];
    char *pMarkerName = 0;
    int nOldDestination = pMarker->nKind;
    int nSlot;
    int nNewDestination;
    char szMarkerName[16];
    int i;
    int bHasName;
    Ov002SessionActorFlags *pActor;

    if (nOldDestination != -1) {
        if (pMarker->szName[0]) {
            pMarkerName = pMarker->szName;
            strcpy(szMarkerName, pMarkerName);
        }
        nSlot = Ov002_GetCtxTableByte(nOldDestination);
        if (nSlot == -1 && Ov002_FindCodeOwner(pRoot[0x8d79], &nSlot, &nNewDestination)
            && nOldDestination != nNewDestination) {
            pSession->pMarkers[nLocalPlayer].nKind = nNewDestination;
            nSlot = Ov002_GetCtxTableByte(nNewDestination);
            if (nSlot != -1) {
                for (i = 0; i < func_ov022_020882f8(); i++) {
                    pActor = (Ov002SessionActorFlags *)GetEntryField20ByIndex(i);
                    bHasName = pMarkerName != 0;
                    if (i == nLocalPlayer || (pActor->qwFlags & 0x10000ULL) != 0) {
                        if (bHasName) {
                            Ov002_ResolveNamedPlacement(szMarkerName, nSlot,
                                &pSession->pMarkers[i].place,
                                &pSession->pMarkers[i].nOwner, i);
                            pSession->pMarkers[i].szName[0] = 0;
                        }
                        pSession->pMarkers[i].nKind = nNewDestination;
                    }
                }
            }
        }
    }
}
