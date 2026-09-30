/* ov022: keep the rumble in step with whether the local player is on flat ground.
 *
 * Only the local player's own object drives it -- anyone else's is left alone
 * and reported as flat, so a remote player never shakes the pad.
 *
 * Flat is the default. It is only given up when the actor's collision block has
 * a contact mode set and a contact array to read: the face's packed normal is
 * widened and its vertical has to come within a hair of straight up. A slope
 * shallower than that counts as not flat.
 *
 * Either way the rumble is told the answer, on with a fixed strength or off.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

#define FLAT_MIN 0xfc0
#define RUMBLE_STRENGTH 0x333

/* Ov022CollBlock */
struct CollBlock {
    int nHandle;                 /* 0x00 */
    s16 *pContacts;              /* 0x04 */
    void *pRide;                 /* 0x08 */
    u8 pad00c[0x90];
    int nContactY;               /* 0x9c */
    u8 pada0[4];
    int nContactMode;            /* 0xa4 */
};

/* Ov022Actor */
struct Actor {
    u8 pad000[8];
    u8 nOwner;                   /* 0x0008 */
    u8 pad009[0x5f];
    struct CollBlock collMain;   /* 0x0068 */
};

extern int Session_GetLocalPlayerIndex(void);
extern void VecFx32FromVecS16(void *pModel, s16 *pFace, VecFx32 *pOut);
extern int func_ov022_02083f0c(void);
extern void Ov002_StoreVAndToggleBit25(int nHandle, int bOn, int nStrength);

int Ov022_FollowGroundRumble(struct Actor *pActor)
{
    VecFx32 vecNormal;
    struct CollBlock *pColl;
    int bFlat;

    bFlat = 1;
    if (pActor->nOwner != Session_GetLocalPlayerIndex()) {
        return bFlat;
    }
    if (pActor->collMain.nContactMode != 0
        && (pColl = &pActor->collMain) != 0 && pColl->pContacts != 0) {
        VecFx32FromVecS16((void *)pColl->nHandle, pColl->pContacts + 0xa,
                      &vecNormal);
        if (vecNormal.y <= FLAT_MIN) {
            bFlat = 0;
        }
    }
    if (bFlat != 0) {
        Ov002_StoreVAndToggleBit25(func_ov022_02083f0c(), 1, RUMBLE_STRENGTH);
    } else {
        Ov002_StoreVAndToggleBit25(func_ov022_02083f0c(), 0, 0);
    }
    return bFlat;
}
