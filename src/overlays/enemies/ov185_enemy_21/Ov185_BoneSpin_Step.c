
#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/ai_task.h"
#include "game/enemy_common.h"

struct Hw60 { u16 lo : 8; u16 hi : 8; };

struct Ov185Bone {
    char pad000[0x60];
    u16 hw60;
    char pad062[0x4e];
    VecFx32 vPosb0;
    char pad0bc[0x1c];
    VecFx32 vDird8;
};

struct Ov185Actor {
    char pad000[0x1c7];
    u8 bActionState1c7;
    char pad1c8[0x1c8];
    struct Ov185Bone **aBones390;
};

struct Ov185ActionState {
    struct Ov185Actor *pOwner;
    char pad004[0x64];
    int nSpin68;
};

struct Ov185ActionNode {
    AI_TASK_FIELDS(struct Ov185ActionState)
};

extern const short data_0203d210[];

extern int VEC_Normalize(VecFx32 *v, VecFx32 *unit);
extern void ScaleVec3Fx12(int scale, void *src, void *dst);
extern void SetIndexedSlot(void *node, int idx, void *value);

/*
 * Per-frame step of the four-bone spin action.
 *
 * Each of the four bone attachments gets a direction taken from the shared
 * sin/cos table a quarter turn apart.  The running angle is in fixed-point
 * radians, so the table index is the angle times one over two pi, done as a
 * 48-bit multiply.  Each direction is normalised and scaled by the current
 * spin speed, and a bone whose low flag bit is set publishes its world
 * position.  A second pass ends the action once all four bones have gone
 * inactive; otherwise the spin speed decays by a fifth.
 *
 * The angle accumulator is unsigned and the quarter is taken through a signed
 * reinterpretation on purpose.  With a plain signed accumulator the compiler
 * can prove it never goes negative, folds the division into the loop induction
 * and drops the rounding entirely; the cast is what keeps the real division.
 */
void Ov185_BoneSpin_Step(struct Ov185ActionNode *node)
{
    VecFx32 vPos;
    struct Ov185ActionState *state;
    struct Ov185Bone **aBones;
    int i;
    struct Ov185Actor *owner;
    int nIdle;
    struct Ov185Bone *bone;
    int j;
    unsigned int nAngle;
    int nIndex;
    int nQuarter;

    state = node->pState;
    nIdle = 0;
    i = 0;
    nAngle = 0;
    do {
        bone = state->pOwner->aBones390[i];
        nQuarter = ((int)nAngle + (int)((unsigned int)((int)nAngle >> 1) >> 30)) >> 2;
        nIndex = (u16)((nQuarter * 0x28BE60DB9391LL + 0x80000000000LL) >> 44) >> 4;
        bone->vDird8.x = data_0203d210[nIndex * 2];
        bone->vDird8.y = 0;
        bone->vDird8.z = data_0203d210[nIndex * 2 + 1];
        VEC_Normalize(&bone->vDird8, &bone->vDird8);
        ScaleVec3Fx12(state->nSpin68, &bone->vDird8, &bone->vDird8);
        if ((((struct Hw60 *)&bone->hw60)->lo & 1) != 0) {
            vPos = bone->vPosb0;
            Ov107_MoveNodeAndRelayout((Actor *)state->pOwner, &vPos);
        }
        nAngle += 0x6488;
    } while (++i < 4);

    owner = state->pOwner;
    aBones = owner->aBones390;
    j = 0;
    do {
        if ((((struct Hw60 *)&aBones[j]->hw60)->lo & 1) == 0 && ++nIdle == 4) {
            owner->bActionState1c7 = 3;
            SetIndexedSlot(node, node->slot, 0);
            return;
        }
    } while (++j < 4);
    state->nSpin68 += -state->nSpin68 / 5;
}
