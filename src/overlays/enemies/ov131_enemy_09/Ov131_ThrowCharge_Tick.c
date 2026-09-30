/* Throw step: while the thrown object is in flight, tracks its bounces (off the ground and back)
 * and applies gravity; ends the flight after the second landing; once the release flag is set
 * queues action 8 and clears the step handler. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/ai_task.h"

struct Flags17a {
    u8 bBit0 : 1;
};

struct Flags3cc {
    int bBit0 : 1;
};

struct State {
    char *pActor;
    char pad04[0x14];
    VecFx32 vPrev18;
    VecFx32 vDelta24;
    int nPhase30;
    char pad34[0x24];
    int nActive58;
};

struct Node {
    AI_TASK_FIELDS(struct State)
};

extern void SetIndexedSlot(struct Node *node, int slot, int arg);

void Ov131_ThrowCharge_Tick(struct Node *node)
{
    struct State *st;
    int bHeld;

    st = node->pState;
    if (st->nActive58 != 0) {
        st->vPrev18 = st->vDelta24;
        bHeld = ((struct Flags17a *)(st->pActor + 0x17a))->bBit0;
        if (bHeld == 0 && st->nPhase30 == 0) {
            st->nPhase30 = st->nPhase30 + 1;
        } else if (bHeld != 0 && st->nPhase30 == 1) {
            st->nPhase30 = st->nPhase30 + 1;
        }
        if (st->nPhase30 == 2) {
            st->vDelta24.x = 0;
            st->vDelta24.z = 0;
            st->nPhase30 = 0;
            st->nActive58 = 0;
        } else {
            st->vDelta24.y = st->vDelta24.y -
                (int)(((long long)(*(int *)((char *)node->pList + 0x2c) * 0x1e) * 0x68 +
                       0x800) >> 12);
        }
    }
    if (((struct Flags3cc *)(st->pActor + 0x3cc))->bBit0 != 0) {
        *(u8 *)(st->pActor + 0x1c7) = 8;
        SetIndexedSlot(node, node->slot, 0);
    }
}
