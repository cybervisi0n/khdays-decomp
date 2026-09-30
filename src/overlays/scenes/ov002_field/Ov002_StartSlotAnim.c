
/* One object slot, 0x18 bytes. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct Ov002ObjectSlot {
    void *pObject;                      /* +0x00 */
    VecFx32 vAt;                      /* +0x04 */
    int nMode;                          /* +0x10 */
    u8 bEntryIndex;                     /* +0x14 */
    char pad15[3];
} Ov002ObjectSlot;

typedef struct Ov002ObjectContext {
    char pad00[0x44];
    Ov002ObjectSlot *pSlots;            /* +0x44 */
} Ov002ObjectContext;

/* The block the positional animation start takes: a place and the mode. */
typedef struct Ov002AnimAt {
    VecFx32 vAt;                      /* +0x00 */
    int nMode;                          /* +0x0c */
} Ov002AnimAt;

extern Ov002ObjectContext *data_ov002_0207fa14;

extern int Ov107_CreateRestartTask(void);
extern int Ov107_CreateTriggerSphere(Ov002AnimAt *pAt);
extern void Ov107_Spawner_SetMoveAnim(void *pObject, int nAnim, int nFlag);
extern void Ov002_SetKeyNodeVisible(int nKey, int nParam, int nFlag);

/* Start a slot's animation and record where it was started from.
 *
 * Mode zero takes the plain animation; any other mode takes the positional
 * one, which is handed the place and the mode together in a block on the
 * stack. Either way the animation goes to the slot's object. The place is then
 * copied into the slot when the caller gave one, the mode and the key are
 * stamped in, and the key's node is made visible.
 */
void Ov002_StartSlotAnim(int nIndex, int nKey, int nMode, VecFx32 *pAt,
                         int nParam)
{
    Ov002ObjectContext *pCtx;
    Ov002AnimAt at;
    int nAnim;

    pCtx = data_ov002_0207fa14;
    if (nMode == 0) {
        nAnim = Ov107_CreateRestartTask();
        Ov107_Spawner_SetMoveAnim(pCtx->pSlots[nIndex].pObject, nAnim, 1);
    } else {
        at.vAt.x = pAt->x;
        at.vAt.y = pAt->y;
        at.vAt.z = pAt->z;
        at.nMode = nMode;
        nAnim = Ov107_CreateTriggerSphere(&at);
        Ov107_Spawner_SetMoveAnim(pCtx->pSlots[nIndex].pObject, nAnim, 1);
    }

    if (pAt != 0) {
        pCtx->pSlots[nIndex].vAt = *pAt;
    }
    pCtx->pSlots[nIndex].nMode = nMode;
    pCtx->pSlots[nIndex].bEntryIndex = (u8)nKey;
    Ov002_SetKeyNodeVisible(nKey, nParam, -1);
}
