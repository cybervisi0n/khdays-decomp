/* Rotates the action motion vector, starts resource 0x169 once at timer 0x2a8, applies repeated
 * area hits from 0xff0, clears motion after a hit, and advances when the substate ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/ai_task.h"
#include "game/enemy_common.h"

typedef struct {
    VecFx32 center;
    int radius;
} Sphere;

typedef struct {
    int m[9];
} MtxFx33;

typedef struct {
    char pad00[0x74];
    VecFx32 position;
    int radius;
    char pad84[0x33c];
    void *auxResource;
} Ov281Actor;

typedef struct {
    char pad00[0xad];
    u8 active;
} Ov281ActionSubState;

typedef struct {
    Ov281Actor *actor;
    Ov281ActionSubState *subState;
    int field08;
    int hitContext;
    int heading;
    int field14;
    int timer;
    int field1c;
    VecFx32 motion;
    char pad2c[0x10];
    u8 effectStarted;
} Ov281ActionState;

typedef struct {
    char pad00[0x2c];
    int frameStep;
} Ov281ActionScene;

typedef struct {
    AI_TASK_FIELDS(Ov281ActionState)
} Ov281ActionNode;

extern short data_0203d210[];
extern const VecFx32 data_02041dc8;
extern void MTX_RotY33_(MtxFx33 *mtx, int sinVal, int cosVal);
extern void MTX_MultVec33(const VecFx32 *v, const MtxFx33 *m, VecFx32 *dst);
extern void func_ov107_020c0b90();
extern void Ov107_BuildAndSendUpdate(Ov281Actor *actor, int effect, int mode, int context);
extern void *Ov281_ApplyAreaHit(Ov281ActionState *state, unsigned int mask,
                                 Sphere *sphere, VecFx32 *direction, int strength);
extern void SetIndexedSlot(Ov281ActionNode *node, int slot, void *callback);
extern void Ov281_AiStep_QueueAction2OnAnimEnd_2(void);

void Ov281_UpdateRotatingHitAction(Ov281ActionNode *node)
{
    Ov281ActionState *state;
    MtxFx33 mtx;
    Sphere sphere;
    int idx;

    state = node->pState;
    idx = (unsigned short)(((long long)state->heading * 0x28be60db9391LL +
                            0x80000000000LL) >> 44) >> 4;
    MTX_RotY33_(&mtx, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
    MTX_MultVec33((const VecFx32 *)((char *)state->actor->auxResource + 0x2c),
                  &mtx, &state->motion);

    state->timer += ((Ov281ActionScene *)node->pList)->frameStep;
    if (state->effectStarted == 0) {
        if (state->timer >= 0x2a8) {
            state->effectStarted = 1;
            func_ov107_020c0b90(state->actor, 2, data_02041dc8, 0);
            Ov107_BuildAndSendUpdate(state->actor, 0x169, 4, state->hitContext);
        }
    }

    if (state->timer >= 0xff0) {
        sphere = *(Sphere *)&state->actor->position;
        if (Ov281_ApplyAreaHit(state, 1, &sphere, 0, 0x1000) != 0) {
            state->motion = data_02041dc8;
        }
    }

    if (state->subState->active == 0) {
        Ov107_PostTagUpdate((Actor *)state->actor, 6, 0);
        SetIndexedSlot(node, node->slot, Ov281_AiStep_QueueAction2OnAnimEnd_2);
    }
}
