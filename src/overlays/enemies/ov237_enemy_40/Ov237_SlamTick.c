/* Slam tick of the ov237 actor: the +0x30 clock runs up at the frame rate. At 1.46 the pending slam
 * (+0x34 = 1) lands: the +0x48 impact point is the +0x440 hand plus data_ov237_020d1b94 turned by the
 * +0x10 heading, effect 3 and the slam sound (0x12d variant 4) play. Until 2.13 a box there (axes of the
 * world, half-extents 1.5 / 0.5 / 1.5) hits once with push data_ov237_020d1b88 (hit sound variant 5).
 * The +0x3c aim point follows the +0x3d8 partner (020cdb50); once the +4 rig is idle bit 6 of the +0x60
 * high byte clears and the next move is 2. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;
typedef struct { VecFx32 pos; VecFx32 axis[3]; int ext[3]; } Box;

extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, int at);
extern int Ov237_AttackHitTest(int *node, void *sphere, void *box, void *segment, VecFx32 *push, int once, unsigned short effect, int kind);
extern VecFx32 Ov237_RotateByActorHeading(int *node, VecFx32 *target);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern const VecFx32 data_ov237_020d1b88;
extern const VecFx32 data_ov237_020d1b94;
extern const VecFx32 data_02042258;
extern const VecFx32 data_02042264;
extern const VecFx32 data_02042270;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov237_SlamTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 hand;
    VecFx32 off;
    Mtx33 rot;
    Box box;
    VecFx32 push;

    state[0xc] += *(int *)(node[0] + 0x2c);
    if (state[0xc] >= 0x1760 && state[0xd] == 1) {
        hand = *(VecFx32 *)(*(int *)(*state + 0x440) + 0x14);
        off = data_ov237_020d1b94;
        {
            int idx = ANG2IDX(state[4]) * 2;

            MTX_RotY33_(&rot, data_0203d210[idx], data_0203d210[idx + 1]);
        }
        MTX_MultVec33(&off, &rot, &off);
        VEC_Add(&hand, &off, &hand);
        state[0xd]--;
        func_ov107_020c0b90(*state, 3, hand, 0);
        Ov107_BuildAndSendUpdate(*state, 0x12d, 4, state[0xe]);
        *(VecFx32 *)(state + 0x12) = hand;
    }
    if (state[0xc] >= 0x1760 && state[0xc] < 0x2200) {
        push = data_ov237_020d1b88;
        box.pos = *(VecFx32 *)(state + 0x12);
        box.axis[0] = data_02042270;
        box.axis[1] = data_02042264;
        box.axis[2] = data_02042258;
        box.ext[0] = 0x1800;
        box.ext[1] = 0x800;
        box.ext[2] = 0x1800;
        if (Ov237_AttackHitTest(node, 0, &box, 0, &push, 1, 0, 0) != 0) {
            Ov107_BuildAndSendUpdate(*state, 0x12d, 5, state[0xe]);
        }
    }
    *(VecFx32 *)(state + 0xf) = Ov237_RotateByActorHeading(node, (VecFx32 *)(*(int *)(*state + 0x3d8) + 0x2c));
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    {
        u16 hw = *(u16 *)(state + 0x18);

        *(u16 *)(state + 0x18) = (hw & ~0xff00) |
            (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~0x40) << 0x18) >> 0x10);
    }
    *(signed char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
