/* Bounce tick of the ov218 enemy's shot. On a fresh launch (+0xc) the +0x10 velocity is 0.5 along
 * the flattened +0x1c aim (forward when it has no length) or, better, towards the visible, unguarded
 * actor of the world list that lies most along the aim; the speed is set to 0.125 (+0x38 / +0x14),
 * +0x44 armed and the launch cleared. In flight (above 1/16) a wall contact (+0x17a bit 1) reflects
 * the velocity about the owner's +0x114 normal at 62.5 % of its length, and while armed a probe at
 * the owner (020c9094) that meets a hidden part releases the +0x394 hold. Once the hold is released
 * the shot plays pose 1, sends message 0 (flagged unless grounded) and reaction 0x135 mode 6 at the
 * +8 target, clears +0x34 and the velocity and hands over to 020cecb8. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Bit17a { unsigned char b0 : 1, b1 : 1; };
struct W8 { unsigned int lo : 8; };
struct Hw60 { unsigned short lo : 8, hi : 8; };

extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern int *List_First(void *list);
extern int *List_Next(void *list);
extern void VEC_Subtract(void *a, void *b, VecFx32 *d);
extern int VEC_DotProduct(VecFx32 *a, VecFx32 *b);
extern int Ov107_FindEntityHitBySphere(int owner, VecFx32 *at, int *out);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, unsigned char flag);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, int at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov218_ShotBurstTick(void);
extern const VecFx32 data_02042258;
extern const VecFx32 data_02041dc8;

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

void Ov218_ShotBounceTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 tmp;
    VecFx32 dir;
    VecFx32 d;
    VecFx32 n;
    VecFx32 u;
    VecFx32 r;
    int out;
    int best;
    int world;
    int dot;
    int len;
    int *ln;
    int obj;
    int owner;

    if (state[3] != 0) {
        world = *(int *)(*state + 4);
        dir = *(VecFx32 *)(state + 7);
        dir.y = 0;
        best = 0x800;
        if (VEC_Normalize(&dir, &dir) == 0) {
            dir = data_02042258;
        }
        ScaleVec3Fx12(0x800, &dir, (VecFx32 *)(state + 4));
        ln = List_First((void *)(world + 0x80));
        obj = ln == 0 ? 0 : *ln;
        while (obj != 0) {
            if (obj != *state && (((struct Hw60 *)(obj + 0x60))->lo & 1) != 0 &&
                (*(unsigned short *)(obj + 0x100 + 0xac) & 7) == 0) {
                VEC_Subtract((void *)(obj + 0x74), (void *)(*state + 0x74), &d);
                d.y = 0;
                VEC_Normalize(&d, &d);
                dot = VEC_DotProduct(&dir, &d);
                if (dot > best) {
                    ScaleVec3Fx12(0x800, &d, (VecFx32 *)(state + 4));
                    best = dot;
                }
            }
            ln = List_Next((void *)(world + 0x80));
            obj = ln == 0 ? 0 : *ln;
        }
        state[0xe] = 0x200;
        state[5] = 0x200;
        state[0x11] = 1;
        state[3] = 0;
    } else if (VEC_Normalize((VecFx32 *)(state + 4), &tmp) >= 0x100) {
        if (((struct Bit17a *)(*state + 0x17a))->b1 != 0) {
            n = *(VecFx32 *)(*state + 0x114);
            len = VEC_Normalize((VecFx32 *)(state + 4), &u);
            ScaleVec3Fx12(-0x1000, &u, &u);
            ScaleVec3Fx12(VEC_DotProduct(&u, &n) << 1, &n, &r);
            VEC_Subtract(&r, &u, &r);
            ScaleVec3Fx12(FX_MUL(len, 0xa00), &r, (VecFx32 *)(state + 4));
        }
        if (state[0x11] != 0 && Ov107_FindEntityHitBySphere(*state, (VecFx32 *)(*state + 0x74), &out) != 0) {
            if ((((struct W8 *)(out + 8))->lo & 1) != 0) {
                *(int *)(*state + 0x394) = 0;
            }
        }
    }
    if (*(int *)(*state + 0x394) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 1, 0);
    owner = *state;
    func_ov107_020c0b90(owner, 0, *(VecFx32 *)state[2], ((struct Bit17a *)(owner + 0x17a))->b0 == 0 ? 1 : 0);
    Ov107_BuildAndSendUpdate(*state, 0x135, 6, state[2]);
    state[0xd] = 0;
    *(VecFx32 *)(state + 4) = data_02041dc8;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov218_ShotBurstTick);
}
