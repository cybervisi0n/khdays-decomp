
/* The collision cast block, as the matched sources in src/calls spell it. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct CollCastParams {
    VecFx32 *origin;
    VecFx32 *direction;
    s32 radius;
    u16 directionIsUnit;
    u16 flags;
    void *exclude;
} CollCastParams;

/* Only the fields this routine touches; the rest of the element is modelled in
 * Ghidra as Ov002PieceElement. */
typedef struct Ov002PieceElement {
    char pad0000[0x2c];
    u8 nColumn;                     /* +0x2c */
    u8 nRow;                        /* +0x2d */
    u16 nPieceId;                   /* +0x2e */
    char pad0030[2];
    u8 nSlot;                       /* +0x32 */
} Ov002PieceElement;

/* Likewise only the trailing spot table; aSpots holds the last place each spot
 * was spawned at. */
typedef struct Ov002SpotStage {
    char pad0000[0x2528];
    VecFx32 aSpots[1];            /* +0x2528 */
} Ov002SpotStage;

/* The stage pointer sits one word into the holder, and the overlay's own
 * sources reach it by indexing rather than through a named field. */
extern Ov002SpotStage *data_ov002_0207fa28[];

extern int Ov002_GetCtxTableByte(int nSlot);
extern void *func_0202c208(int nId, CollCastParams *pParams);
extern void ScaleVec3Fixed27(void *pObject, VecFx32 *pOut, VecFx32 *pIn);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *ab);
extern void Ov002_SpawnSpot(int nRow, int nColumn, int nId, int nSlot,
                                VecFx32 *pAt, int nKind, int nFlags);

/* Settle where a board slot's marker belongs and hand it to the placer.
 *
 * The marker starts at the slot's own spot position and is cast straight down.
 * A first hit moves the start point onto the surface and the cast is retried
 * from a little above it; if that second cast misses, the surface point and the
 * spot position are added together. With no hit at all the spot position stands
 * as it is.
 */
void Ov002_PlaceSlotMarkerOnGround(Ov002PieceElement *pSlot)
{
    CollCastParams params;
    VecFx32 vDown;
    VecFx32 vAt;
    void *pHit;
    int nId;

    nId = Ov002_GetCtxTableByte(pSlot->nSlot);

    params.directionIsUnit = 0;
    params.exclude = 0;
    params.flags = 0xf;
    params.origin = &data_ov002_0207fa28[1]->aSpots[pSlot->nRow];
    vDown.z = 0;
    vDown.x = 0;
    vDown.y = -0x64000;
    params.direction = &vDown;

    pHit = func_0202c208((u16)nId, &params);
    if (pHit != 0) {
        ScaleVec3Fixed27(*(void **)((char *)pHit + 0xc), &vDown, &vDown);
        vDown.y += 0x1000;
        /* Written as a mask where another call truncates with a cast: mwcc would otherwise compute the
         * truncation once and keep it, while the ROM truncates again at each call. */
        if (func_0202c208(nId & 0xffff, &params) == 0) {
            VEC_Add(&vDown, &data_ov002_0207fa28[1]->aSpots[pSlot->nRow],
                    &vAt);
        } else {
            vAt = data_ov002_0207fa28[1]->aSpots[pSlot->nRow];
        }
    } else {
        vAt = data_ov002_0207fa28[1]->aSpots[pSlot->nRow];
    }

    Ov002_SpawnSpot(pSlot->nRow, pSlot->nColumn, pSlot->nPieceId,
                        pSlot->nSlot, &vAt, 1, 0);
}
