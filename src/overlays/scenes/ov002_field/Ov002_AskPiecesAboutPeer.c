
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct Ov002Piece Ov002Piece;

typedef struct Ov002Owner {
    char pad000[0x80];
    char list[0xc];
} Ov002Owner;

extern int Ov022_GetEntryField66(int nPeer);          /* peer -> owner slot */
extern VecFx32 *func_ov022_020881f8(int nPeer);        /* where the peer is */
extern int Ov002_Event_GetField18(void);
extern Ov002Owner *Ov002_GetPieceOwner(int nSlot);
extern Ov002Piece **List_First(void *pList);     /* first */
extern int Ov002_PieceAnswersForPoint(Ov002Piece *pPiece, const VecFx32 *pPos, int nArg);

/* Asks every piece the peer owns whether it answers for the peer's position,
 * and stops at the first that does.
 *
 * The position is taken before anything is checked, so it is the peer's place
 * at the moment of asking rather than wherever it ends up.  A settled record
 * set, a peer with no owner slot, or a slot with no owner at all all answer no
 * without walking anything.
 */
int Ov002_AskPiecesAboutPeer(int nPeer, int nArg)
{
    VecFx32 vPos;
    Ov002Piece **ppPiece;
    Ov002Piece *pPiece;
    Ov002Owner *pOwner;
    int nSlot;

    nSlot = Ov022_GetEntryField66(nPeer);
    vPos = *func_ov022_020881f8(nPeer);

    if (Ov002_Event_GetField18() != -1 && nSlot >= 0
        && (pOwner = Ov002_GetPieceOwner(nSlot)) != 0) {
        ppPiece = List_First(pOwner->list);
        pPiece = (ppPiece == 0) ? 0 : *ppPiece;
        while (pPiece != 0) {
            if (Ov002_PieceAnswersForPoint(pPiece, &vPos, nArg) != 0) {
                return 1;
            }
            ppPiece = (Ov002Piece **)List_Next(pOwner->list);
            pPiece = (ppPiece == 0) ? 0 : *ppPiece;
        }
    }
    return 0;
}
