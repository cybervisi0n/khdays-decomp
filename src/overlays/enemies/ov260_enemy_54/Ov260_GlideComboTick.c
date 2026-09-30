/* Glide combo tick of the ov260 actor (stage +0x78): the +0x70 timer accumulates the frame rate and
 * the stage's effect starts once (+0x7b bit 0: 6 at 0x330, 7 at 0x1100, 8 at 0x2288); the second
 * +0x17a flag starts effect 5 once (bit 1); stage 0 cues at 0x440 and 0x770 (020cd04c 2 / 3). The
 * +0x20 velocity is its +0x428 part's +0x2c vector turned by the +0x64 heading and the body sweeps for
 * hits (020cd2a0, the stage as kind; from stage 2 along a segment from its +0x74 position along the
 * velocity, +0x80 wide). Once the partner holds no queued move, stage 2 ends the glide (pose 0x18,
 * motion 0xe, on to 020ce70c); earlier stages re-aim (020cd794), turn to the target, advance and play
 * pose 0x13 + 2 x stage, motion 9 + 2 x stage and effect 0x17 (stage 1) or 0x1a. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;
typedef struct { VecFx32 pos; VecFx32 axis; int length; int radius; } Capsule;
struct Flag17a { u8 b0 : 1; u8 b1 : 1; };

extern void Ov260_PlaySound(int owner, int mode, int arg);
extern void Ov260_MapHeldItemKindToAnim(int actor, int flag);
extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void Ov260_AttackSweep(int *state, int kind, void *sphere, void *cyl, void *seg);
extern void Ov107_PostTagUpdate(int actor, int pose, int loop);
extern int Ov107_StartAnim(int part, int motion, int mode);
extern int Ov260_PickTarget(int *node);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov260_DriftStep(void);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)
#define STAGE(s) (*((signed char *)(s) + 0x78))

void Ov260_GlideComboTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 rot;
    Capsule seg;

    state[0x1c] += *(int *)(node[0] + 0x2c);
    if ((*((u8 *)state + 0x7b) & 1) == 0) {
        if (STAGE(state) == 0 && state[0x1c] >= 0x330) {
            *((u8 *)state + 0x7b) |= 1;
            Ov260_PlaySound(*state, 6, state[4]);
        } else if (STAGE(state) == 1 && state[0x1c] >= 0x1100) {
            *((u8 *)state + 0x7b) |= 1;
            Ov260_PlaySound(*state, 7, state[4]);
        } else if (STAGE(state) == 2 && state[0x1c] >= 0x2288) {
            *((u8 *)state + 0x7b) |= 1;
            Ov260_PlaySound(*state, 8, state[4]);
        }
    }
    if ((*((u8 *)state + 0x7b) & 2) == 0 && ((struct Flag17a *)(*state + 0x17a))->b1) {
        *((u8 *)state + 0x7b) |= 2;
        Ov260_PlaySound(*state, 5, state[4]);
    }
    if (STAGE(state) == 0) {
        if ((*((u8 *)state + 0x7b) & 4) == 0 && state[0x1c] >= 0x440) {
            *((u8 *)state + 0x7b) |= 4;
            Ov260_MapHeldItemKindToAnim(*state, 2);
        }
        if ((*((u8 *)state + 0x7b) & 8) == 0 && state[0x1c] >= 0x770) {
            *((u8 *)state + 0x7b) |= 8;
            Ov260_MapHeldItemKindToAnim(*state, 3);
        }
    }
    {
        int idx = ANG2IDX(state[0x19]) * 2;

        MTX_RotY33_(&rot, data_0203d210[idx], data_0203d210[idx + 1]);
    }
    MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x428) + 0x2c), &rot, (VecFx32 *)(state + 8));
    if (STAGE(state) >= 2) {
        seg.pos = *(VecFx32 *)(*state + 0x74);
        seg.radius = *(int *)(*state + 0x80);
        seg.length = VEC_Normalize((VecFx32 *)(state + 8), &seg.axis);
        Ov260_AttackSweep(state, STAGE(state), 0, 0, &seg);
    } else {
        Ov260_AttackSweep(state, STAGE(state), 0, 0, 0);
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if (STAGE(state) >= 2) {
        Ov107_PostTagUpdate(*state, 0x18, 0);
        Ov107_StartAnim(*(int *)(*state + 0x428), 0xe, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_DriftStep);
        return;
    }
    Ov260_PickTarget(node);
    state[0x19] = state[0x1a];
    STAGE(state)++;
    *((u8 *)state + 0x7b) = 0;
    *((u8 *)state + 0x79) = 0;
    Ov107_PostTagUpdate(*state, STAGE(state) * 2 + 0x13, 0);
    Ov107_StartAnim(*(int *)(*state + 0x428), STAGE(state) * 2 + 9, 0);
    Ov260_PlaySound(*state, (u8)(STAGE(state) == 1 ? 0x17 : 0x1a), state[4]);
}
