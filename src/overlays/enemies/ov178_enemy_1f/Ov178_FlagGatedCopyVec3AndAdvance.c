/* AI step: once the actor is active, places the tracking node 0x2000 above the actor's position
 * (+0xb0), records whether the context mode where it lands is 8, then makes the stored action
 * (+0x1c9) pending and clears the step handler. */

#include "nitro/fx_types.h"
#include "game/actor.h"
#include "game/ai_task.h"

struct Holder {
    Actor *node;
    char pad4[0x10];
    VecFx32 f14;
    char pad20[0x68];
    int f88;
};

struct Obj {
    AI_TASK_FIELDS(struct Holder)
};

extern int Ov107_MoveNodeAndRelayout(int node, VecFx32 *v);
extern signed char Ov002_GetCtxModeByte(int x);
extern int SetIndexedSlot();

void Ov178_FlagGatedCopyVec3AndAdvance(struct Obj *this_) {
    struct Holder *h = this_->pState;
    Actor *node = h->node;

    if (((unsigned)(node->flags60.raw << 24) >> 24 & 1) == 0) return;

    h->f14 = node->srt.translation;
    h->f14.y += 0x1c00;
    h->f88 = (Ov002_GetCtxModeByte(Ov107_MoveNodeAndRelayout((int)h->node, &h->f14)) == 8);
    node = h->node;
    node->nextState = node->field_1c9;
    SetIndexedSlot(this_, this_->slot, 0);
}
