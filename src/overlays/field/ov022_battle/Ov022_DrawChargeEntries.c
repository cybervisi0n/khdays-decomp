/* ov022: place and submit the charge entries that are showing this frame.
 *
 * Three entries, each drawn only under its own condition. The third follows the
 * actor's guard point while the menu is open on the third page; the second
 * follows the point the aim helper works out, and only while either of the two
 * aim flags is up; the first follows the effect anchor, and only on the seventh
 * page. Each one has its position written and is then rendered on its own.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

#define AIM_FLAG_A 0x2000000000000ULL
#define AIM_FLAG_B 0x4000000000000ULL
#define MENU_GUARD 3
#define MENU_LOCKON 7

/* One charge entry: an animation object whose position sits at +0xa4. */
struct ChargeEntry {
    u8 pad0000[0xa4];
    VecFx32 vecPos;              /* 0x00a4 */
    u8 pad00b0[0x58];
};

/* Ov022Charge */
struct Charge {
    u8 pad0000[0x1c];
    struct ChargeEntry aEntries[3];   /* 0x001c */
};

/* Ov022Actor */
struct Actor {
    u8 pad0000[0x464];
    u64 nFlags2;                 /* 0x0464 */
    u8 pad046c[0x20];
    VecFx32 vecAim;              /* 0x048c */
    u8 pad0498[0x46c];
    VecFx32 vecGuard;            /* 0x0904 */
    u8 pad0910[0x1e60];
    struct Charge charge;        /* 0x2770 */
    u8 pad2aa4[0x14];
    u8 nMenuState;               /* 0x2ab8 */
};

extern void Scene_DrawNode(struct ChargeEntry *pEntry);   /* RenderNode */
extern void func_ov022_020ad44c(VecFx32 *pOut, struct Actor *pActor);
extern void Ov022_GetEffectAnchor(VecFx32 *pOut,
                                struct Actor *pActor);  /* effect anchor */

void Ov022_DrawChargeEntries(struct Actor *pActor)
{
    VecFx32 vecPoint;
    struct Charge *pCharge;

    vecPoint = pActor->vecAim;
    pCharge = &pActor->charge;
    if (pActor->nMenuState == MENU_GUARD) {
        pCharge->aEntries[2].vecPos = pActor->vecGuard;
        Scene_DrawNode(&pCharge->aEntries[2]);
    }
    if ((pActor->nFlags2 & AIM_FLAG_A) != 0
        || (pActor->nFlags2 & AIM_FLAG_B) != 0) {
        func_ov022_020ad44c(&vecPoint, pActor);
        pCharge->aEntries[1].vecPos = vecPoint;
        Scene_DrawNode(&pCharge->aEntries[1]);
    }
    if (pActor->nMenuState != MENU_LOCKON) {
        return;
    }
    Ov022_GetEffectAnchor(&vecPoint, pActor);
    pCharge->aEntries[0].vecPos = vecPoint;
    Scene_DrawNode(&pCharge->aEntries[0]);
}
