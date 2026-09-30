/* Rotates the auxiliary hit vector from the action phase, advances the timer, starts reaction 0x12f
 * once, tests the actor sphere for hits, and advances the node when its asynchronous substate ends.
 */

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
} Ov190Actor;

typedef struct {
    char pad00[0xad];
    u8 active;
} Ov190ActionSubState;

typedef struct {
    Ov190Actor *actor;
    Ov190ActionSubState *subState;
    int field08;
    int hitContext;
    int heading;
    int field14;
    int timer;
    int field1c;
    VecFx32 motion;
    char pad2c[0x10];
    u8 effectStarted;
} Ov190ActionState;

typedef struct {
    char pad00[0x2c];
    int frameStep;
} Ov190ActionScene;

typedef struct {
    AI_TASK_FIELDS(Ov190ActionState)
} Ov190ActionNode;

extern short data_0203d210[];
extern const VecFx32 data_02041dc8;
extern void MTX_RotY33_(MtxFx33 *mtx, int sinVal, int cosVal);
extern void MTX_MultVec33(const VecFx32 *v, const MtxFx33 *m, VecFx32 *dst);
extern void func_ov107_020c0b90();
extern void Ov107_BuildAndSendUpdate(Ov190Actor *actor, int effect, int mode, int context);
extern void *Ov190_ProcessHitTargets(Ov190ActionState *state, unsigned int mask,
                                 Sphere *sphere, VecFx32 *direction, int strength);
extern void SetIndexedSlot(Ov190ActionNode *node, int slot, void *callback);
extern void Ov190_AiStep_QueueAction2OnAnimEnd_2(void);

void Ov190_UpdateRotatingHitAction(Ov190ActionNode *node)
{
    Ov190ActionState *state;
    MtxFx33 mtx;
    Sphere sphere;
    int idx;

    state = node->pState;
    idx = (unsigned short)(((long long)state->heading * 0x28be60db9391LL +
                            0x80000000000LL) >> 44) >> 4;
    MTX_RotY33_(&mtx, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
    MTX_MultVec33((const VecFx32 *)((char *)state->actor->auxResource + 0x2c),
                  &mtx, &state->motion);

    state->timer += ((Ov190ActionScene *)node->pList)->frameStep;
    if (state->effectStarted == 0) {
        if (state->timer >= 0x2a8) {
            state->effectStarted = 1;
            func_ov107_020c0b90(state->actor, 2, data_02041dc8, 0);
            Ov107_BuildAndSendUpdate(state->actor, 0x12f, 4, state->hitContext);
        }
    }

    if (state->timer >= 0xff0) {
        sphere = *(Sphere *)&state->actor->position;
        if (Ov190_ProcessHitTargets(state, 1, &sphere, 0, 0x1000) != 0) {
            state->motion = data_02041dc8;
        }
    }

    if (state->subState->active == 0) {
        Ov107_PostTagUpdate((Actor *)state->actor, 6, 0);
        SetIndexedSlot(node, node->slot, Ov190_AiStep_QueueAction2OnAnimEnd_2);
    }
}
