/* Ov015_ForEachPieceInSphere -- Ov015_ForEachPieceInSphere: for every piece owned by seat nSlot
 * (ov002 02073880, list +0x80) that answers for the centre (ov002 02074260 with -1), walk
 * its part nodes (+0x22c); the first active part (bit 0 of the node's flag byte +0x8)
 * whose shape meets the sphere (ov107 020c3504) is handed to pfn(index, piece, node, nArg)
 * and a 0 answer stops the walk.  Nothing happens while the record set is settled
 * (0207386c == -1) or the seat has no owner. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct SphereFx32 {
    VecFx32 center;
    int radius;
} SphereFx32;

typedef struct Ov015NodeFlags {
    unsigned int lowByte : 8;
    unsigned int rest : 24;
} Ov015NodeFlags;

typedef struct Ov015PartNode {
    void *pShape;             /* 0x00 */
    u8   pad_04[4];
    unsigned int nFlags;      /* 0x08: low byte bit 0 active */
} Ov015PartNode;

typedef struct Ov015Piece {
    u8   pad_000[0x22c];
    u8   partList[0xc];       /* 0x22c */
} Ov015Piece;

typedef struct Ov015Owner {
    u8   pad_000[0x80];
    u8   pieceList[0xc];      /* 0x80 */
} Ov015Owner;

typedef int (*Ov015PieceInSphereFn)(int nIndex, Ov015Piece *pPiece, Ov015PartNode *pNode, int nArg);

extern int  Ov002_Event_GetField18(void);                                  /* record set state */
extern Ov015Owner *Ov002_GetPieceOwner(int nSlot);                      /* seat owner */
extern void *List_First(void *pList);                                /* List_First */
extern int  Ov002_PieceAnswersForPoint(Ov015Piece *pPiece, const VecFx32 *pPos, int nArg); /* piece answers for a position */
extern int  Ov107_HitShape_TestSphere(void *pShape, const SphereFx32 *pSphere, int nMode); /* shape meets a sphere */

void Ov015_ForEachPieceInSphere(int nSlot, VecFx32 *pCenter, int nRadius, Ov015PieceInSphereFn pfn, int nArg)
{
    SphereFx32 sphere;
    Ov015Owner *pOwner;
    Ov015Piece **ppPiece;
    Ov015Piece *pPiece;
    int nIndex;
    Ov015PartNode *pNode;

    if (Ov002_Event_GetField18() == -1) {
        return;
    }
    pOwner = Ov002_GetPieceOwner(nSlot);
    sphere.center = *pCenter;
    sphere.radius = nRadius;
    if (pOwner == 0) {
        return;
    }
    ppPiece = List_First(pOwner->pieceList);
    pPiece = (ppPiece == 0) ? 0 : *ppPiece;
    nIndex = 0;
    while (pPiece != 0) {
        if (Ov002_PieceAnswersForPoint(pPiece, pCenter, -1) != 0) {
            for (pNode = List_First(pPiece->partList); pNode != 0; pNode = (Ov015PartNode *)List_Next(pPiece->partList)) {
                if ((((Ov015NodeFlags *)&pNode->nFlags)->lowByte & 1) != 0
                    && Ov107_HitShape_TestSphere(pNode->pShape, &sphere, 0) != 0) {
                    if (pfn(nIndex, pPiece, pNode, nArg) == 0) {
                        return;
                    }
                    break;
                }
            }
        }
        ppPiece = (Ov015Piece **)List_Next(pOwner->pieceList);
        pPiece = (ppPiece == 0) ? 0 : *ppPiece;
        nIndex++;
    }
}
