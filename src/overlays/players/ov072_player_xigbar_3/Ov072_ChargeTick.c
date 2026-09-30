/* Charge tick of the ov032 enemy (and its byte-identical twins). On the local player's session
 * a bit-36 actor whose +0x464 word carries bit 7 also gets bit 7 on +0x46c, and bit 4 goes to
 * +0x464; without bit 2 of +0x24 the actor is marked unrestricted (bit 46) with a cleared +0x58.
 * The +0x498 velocity gets the zero vector added, the +0x668 handler decides the fired bit,
 * a +0x4cc timer past 0x3000 with bit 1 of +0x2c30 set releases the +0x2644 item (clearing bit
 * 27 and that flag), and a fired shot sets bit 49, restarts the model's animation unless its
 * bit 5 is set and raises bit 1 of +0x464 on the local session. With bit 1 set the state is
 * reported through 020a35f4 (mode 0 plus the +0x664 handler with bit 2 of +0x24, else mode 2
 * after raising bit 2 unless both bit 36 and +0x464 bit 7 hold). The timer advances by 0x1800
 * in single-player frames (3c40 == 1) or 0x1000. */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct ActorBits {
    unsigned char bUnk0 : 1;
    unsigned char bFired : 1;
};

struct FlagBits2c30 {
    unsigned char b0 : 1;
    unsigned char bReleased : 1;
};

extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void func_ov022_0209190c(int item);
extern int Ov022_ActorSetState(char *self, int mode);
extern const VecFx32 data_02041dc8;

int Ov072_ChargeTick(char *self)
{
    int nRet = 0;
    VecFx32 v;

    if ((*(unsigned long long *)self & 0x1000000000ULL) != 0 &&
        (*(unsigned long long *)(self + 0x464) & 0x80) != 0 && Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x46c) |= 0x80;
    }
    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x464) |= 0x10;
    }
    if ((*(int *)(self + 0x24) & 4) == 0) {
        *(unsigned long long *)self |= 0x400000000000ULL;
        *(int *)(self + 0x58) = 0;
    }
    v = data_02041dc8;
    v.y = 0;
    VEC_Add((VecFx32 *)(self + 0x98 + 0x400), &v, (VecFx32 *)(self + 0x98 + 0x400));
    ((struct ActorBits *)(self + 0x694))->bFired = (*(int (**)(char *))(self + 0x668))(self);
    if (*(int *)(self + 0x4cc) >= 0x3000 && ((struct FlagBits2c30 *)(self + 0x2000 + 0xc30))->bReleased != 0) {
        func_ov022_0209190c(*(int *)(self + 0x2000 + 0x644));
        *(unsigned long long *)self &= ~0x8000000ULL;
        *(unsigned char *)(self + 0x2000 + 0xc30) &= ~2;
    }
    if (((struct ActorBits *)(self + 0x694))->bFired) {
        *(unsigned long long *)self |= 0x2000000000000ULL;
        if ((**(int **)(self + 0x20) & 0x20) == 0) {
            SceneNode_Enable(*(int **)(self + 0x20) + 1);
        }
        if (Session_GetLocalPlayerIndex() == 0) {
            *(unsigned long long *)(self + 0x464) |= 2ULL;
        }
    }
    if ((*(unsigned long long *)(self + 0x464) & 2) != 0) {
        if ((*(int *)(self + 0x24) & 4) != 0) {
            nRet = Ov022_ActorSetState(self, 0);
            (*(void (**)(char *, int))(self + 0x664))(self, 0);
        } else {
            if ((*(unsigned long long *)self & 0x1000000000ULL) == 0 ||
                (*(unsigned long long *)(self + 0x464) & 0x80) == 0) {
                *(unsigned long long *)self |= 4ULL;
            }
            nRet = Ov022_ActorSetState(self, 2);
        }
    }
    *(int *)(self + 0x4cc) += GetFrameRateMode() == 1 ? 0x1800 : 0x1000;
    return nRet;
}
