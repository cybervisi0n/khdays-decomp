/* Throw windup tick of the ov218 actor: the +0x28 velocity is its +0x3ac part's +0x2c vector turned by
 * the +0xc heading; once the partner holds no queued move, in move 5 with an unguarded +0x394 partner
 * the node moves on to 020cd3a8, otherwise the next move is 4 and the node ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;
typedef struct { u16 lo : 8; u16 hi : 8; } flags16;
struct Ov218Actor { char pad[0x394]; int partners[2]; };

extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov218_AiEnterAnim3IfTarget(void);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov218_ThrowWindupTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 rot;
    int i;

    {
        int idx = ANG2IDX(state[3]) * 2;

        MTX_RotY33_(&rot, data_0203d210[idx], data_0203d210[idx + 1]);
    }
    MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x3ac) + 0x2c), &rot, (VecFx32 *)(state + 0xa));
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    if (*(signed char *)(*state + 0x1c6) == 5) {
        for (i = 0; i < 2; i++) {
            if ((((flags16 *)(((struct Ov218Actor *)*state)->partners[i] + 0x60))->lo & 1) == 0) {
                SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov218_AiEnterAnim3IfTarget);
                return;
            }
        }
    }
    *(unsigned char *)(*state + 0x1c7) = 4;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}
