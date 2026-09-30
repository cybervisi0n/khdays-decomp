
/* Index of the session's local player, or its delegate. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

/* Collision world id of that player's track entry, or -1 when it has none. */
extern int Ov022_GetEntryField66(int nPlayer);
/* Cast a ray through one world id; returns the shared hit record, or 0. */
extern int *EntityMgr_RunCastSimple(int nWorldId, VecFx32 *pOrigin, VecFx32 *pDir,
                          void *pExclude);
/* out = origin + t * dir, in fx32. */

/* Cast a ray from pOrigin along pDir and write the contact point to pOut.
 *
 * The plain form of the collision query: one cast, no pass-through retry and no
 * normal, just the point where the ray first meets the local player's world.
 * Returns 1 when it hit something, 0 when the ray ran clear or the player has
 * no collision world.
 */
int Ov002_CastRayForContactPoint(VecFx32 *pOrigin, VecFx32 *pDir, VecFx32 *pOut,
                        void *pExclude)
{
    int nPlayer;
    int nWorldId;
    int *pHit;

    nPlayer = QueryActiveStateOrDelegate();
    nWorldId = Ov022_GetEntryField66(nPlayer);
    if (nPlayer < 0 || nWorldId < 0) {
        return 0;
    }

    pHit = EntityMgr_RunCastSimple((u16)nWorldId, pOrigin, pDir, pExclude);
    if (pHit == 0) {
        return 0;
    }

    Vec3ScaleAddQ27(pHit[3], pDir, pOrigin, pOut);
    return 1;
}
