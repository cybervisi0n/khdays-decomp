/* Ov162_OnHit: hit handler of the ov161 enemy (x2), ported from the matched ov131 sibling; the health always drops here and a kind-4 hit keeps the stored impact point. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct HitState {
    char *pTarget;
    char pad04[0x20];
    VecFx32 vPoint;
    char pad30[8];
    int nSource38;
    char pad3c[8];
    int nParam44;
    char pad48[0x10];
    int nFlag58;
    char pad5c[1];
    u8 bAlternate5d : 1;
};

struct HitMsg {
    u32 uFlags : 16;
    u32 uKind : 16;
    VecFx32 vPoint;
    char pad10[0x10];
    u32 uMode20;
    u32 uResult24;
    int nDamage28;
};

extern int Ov107_CalcHitDamage(char *actor, struct HitMsg *msg);
extern void Ov107_BuildAndSendUpdate(char *actor, int id, u16 mode, int param);

int Ov162_OnHit(char *actor, int source, struct HitMsg *msg)
{
    struct HitState *hs;
    int state;
    int left;
    int step;

    hs = *(struct HitState **)(actor + 0x214);
    if (*(short *)(actor + 0x21a) <= 0) {
        return 0;
    }

    state = *(signed char *)(hs->pTarget + 0x1c6);
    if (state != 6) {
        if (state == 8) {
            msg->uMode20 = 1;
        }
    } else {
        msg->uMode20 = 2;
    }

    msg->nDamage28 = Ov107_CalcHitDamage(actor, msg);

    if ((msg->uFlags & 0x4000) != 0) {
        hs->vPoint = msg->vPoint;
        msg->uResult24 = msg->uResult24 & 0xffff0000 | 1;
        *(signed char *)(hs->pTarget + 0x1c7) = 5;
        return 1;
    }

    left = *(short *)(actor + 0x21a) - msg->nDamage28;
    if (left < 0) {
        step = 0;
    } else {
        step = *(short *)(actor + 0x218);
        if (left <= step) {
            step = left;
        }
    }
    *(short *)(actor + 0x21a) = (short)step;

    hs->nSource38 = source;
    if ((msg->uKind & 4) == 0) {
        hs->vPoint = msg->vPoint;
    }

    if (*(short *)(actor + 0x21a) == 0) {
        *(signed char *)(hs->pTarget + 0x1c7) = 3;
    } else {
        if ((msg->uKind & 0x80) != 0) {
            *(signed char *)(hs->pTarget + 0x1c7) = 8;
        } else {
            if ((msg->uFlags & 1) != 0 && (msg->uFlags & 0x10) != 0 &&
                *(signed char *)(hs->pTarget + 0x1c6) == 6) {
                *(signed char *)(hs->pTarget + 0x1c7) = 7;
            } else if (*(signed char *)(hs->pTarget + 0x1c6) == 7) {
                if ((msg->uFlags & 4) != 0) {
                    hs->vPoint.x = msg->vPoint.x;
                    hs->vPoint.y = 0x800;
                    hs->vPoint.z = msg->vPoint.z;
                    hs->nFlag58 = 1;
                }
            } else if ((msg->uFlags & 0x8000) != 0) {
                *(signed char *)(hs->pTarget + 0x1c7) = 5;
            }
        }
    }

    if (msg->nDamage28 > 0) {
        if ((msg->uFlags & 8) == 0 || (msg->uFlags & 0x80) == 0 ||
            msg->uKind != 0x80) {
            if ((msg->uFlags & 0x22) != 0) {
                hs->bAlternate5d = hs->bAlternate5d + 1;
                Ov107_BuildAndSendUpdate(actor, 0x152,
                                    (u16)((hs->bAlternate5d & 1) != 0 ? 2 : 3), hs->nParam44);
            } else {
                hs->bAlternate5d = hs->bAlternate5d + 1;
                Ov107_BuildAndSendUpdate(actor, 0x152,
                                    (u16)((hs->bAlternate5d & 1) != 0 ? 0 : 1), hs->nParam44);
            }
        }
    }
    return 1;
}
