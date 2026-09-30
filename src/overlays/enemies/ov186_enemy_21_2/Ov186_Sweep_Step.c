
#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/actor.h"
#include "game/engine.h"

struct Quat { int x, y, z, w; };
struct Hw60 { u16 lo : 8; u16 hi : 8; };

struct TargetFlags40 {
    int b0 : 1;
    int bSolid : 1;
    int rest : 30;
};

struct Ov185Target {
    char pad000[0x40];
    struct TargetFlags40 flags40;
    char pad044[0x1c];
    u16 hw60;
    char pad062[0x12];
    VecFx32 vPos74;
    int nRadius80;
};

struct ListNode {
    struct Ov185Target *pItem;
};

struct Ov185Scene {
    char pad000[0xa8];
    char listA8[0x10];
};

struct Ov185ActionState {
    Actor *pOwner;
    char pad004[0x04];
    struct Quat qRot08;
    struct Quat qTarget18;
    int nBlend28;
    VecFx32 vForward2c;
    char pad038[0x04];
    int nTimer3c;
    int nAngle40;
    char pad044[0x20];
    int nCooldown64;
    char pad068[0x05];
    u8 bEffect6d;
};

struct Ov185Frame {
    char pad000[0x2c];
    int nDelta2c;
};

struct Ov185ActionNode {
    struct Ov185Frame *pFrame;
    struct Ov185ActionState *pState;
};

extern const short data_0203d210[];
extern const VecFx32 data_02041dc8;

extern void Quat_Slerp(struct Quat *out, int t, struct Quat *a, struct Quat *b);
extern void Srt_SetRotationQuat(struct Quat *dst, struct Quat *src);
extern struct ListNode *List_First(void *list);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *unit);
extern int Ov107_InvokeHitCallback(struct Ov185Target *candidate, Actor *owner,
                               Actor *source, int mode,
                               const VecFx32 *v, int flags);
extern void func_ov107_020c0b90(Actor *owner, int mode, VecFx32 v,
                                int flag);

/*
 * Per-frame step of the sweep attack.
 *
 * Every frame it blends the stored rotation towards its target and publishes the
 * result to the actor.  While the actor is in action 2 or 4 it advances a
 * fixed-point angle by the frame delta and adds a twentieth of that angle's sine
 * to the vertical speed, which is the hover.  Every 0x800 of accumulated time it
 * walks the scene list once: any solid, enabled entity whose distance minus the
 * two radii is within 0xc00 takes a hit, and the contact effect starts on the
 * first hit of the sweep.  A sweep that finds nothing, or the actor moving on to
 * action 5, stops the effect again.  Whatever the action, a positive cooldown
 * counts down, the accumulated velocity is published to the actor, and the
 * accumulator is cleared.
 *
 * The angle-to-table-index step is the shared 48-bit multiply by one over two
 * pi, the same one the bone spin action uses.
 *
 * Two things pin the register assignment, and both are the shape of a live
 * range rather than anything visible in the output.  The owner is cached for the
 * action test but the tail of the else-if reaches through the state again, which
 * ends the cached copy's live range at the test; written the other way the
 * compiler keeps it alive to the end and the three registers that carry the hit
 * flag, the sweep's own owner and the scene rotate by one.  The declaration
 * order then settles which of those three gets which register.
 */
void Ov186_Sweep_Step(struct Ov185ActionNode *node)
{
    int nAngle;
    int nIndex;
    int nDist;
    Actor *owner;
    struct Ov185ActionState *state;
    int bHit;
    struct Ov185Scene *scene;
    struct Ov185Target *target;
    VecFx32 vDelta;
    struct ListNode *pNode;

    state = node->pState;
    Quat_Slerp(&state->qRot08, state->nBlend28, &state->qRot08, &state->qTarget18);
    Srt_SetRotationQuat(((struct Quat *)&state->pOwner->srt.rotation), &state->qRot08);
    owner = state->pOwner;
    if (owner->state == 2 || owner->state == 4) {
        bHit = 0;
        nAngle = state->nAngle40 + node->pFrame->nDelta2c;
        state->nAngle40 = nAngle;
        nIndex = (u16)((nAngle * 4 * 0x28BE60DB9391LL + 0x80000000000LL) >> 44) >> 4;
        state->vForward2c.y += data_0203d210[nIndex * 2] / 20;

        state->nTimer3c = state->nTimer3c + node->pFrame->nDelta2c;
        if (state->nTimer3c >= 0x800) {
            owner = state->pOwner;
            scene = owner->pScene;
            state->nTimer3c = 0;
            pNode = List_First(scene->listA8);
            target = pNode == 0 ? 0 : pNode->pItem;
            while (target != 0) {
                if (target->flags40.bSolid
                    && (((struct Hw60 *)&target->hw60)->lo & 1) != 0) {
                    VEC_Subtract(&target->vPos74, &owner->sphere.center, &vDelta);
                    nDist = VEC_Normalize(&vDelta, &vDelta);
                    if (nDist - (owner->sphere.radius + target->nRadius80) <= 0xc00) {
                        Ov107_InvokeHitCallback(target, state->pOwner, state->pOwner, 0,
                                            &data_02041dc8, 0x10);
                        bHit = 1;
                        if (state->bEffect6d == 0) {
                            func_ov107_020c0b90(state->pOwner, 0, data_02041dc8, 0);
                            state->bEffect6d = 1;
                        }
                    }
                }
                pNode = (struct ListNode *)List_Next(scene->listA8);
                target = pNode == 0 ? 0 : pNode->pItem;
            }
            if (bHit == 0 && state->bEffect6d != 0) {
                func_ov107_020c0b90(state->pOwner, 1, data_02041dc8, 0);
                state->bEffect6d = 0;
            }
        }
    } else if (owner->state == 5 && state->bEffect6d != 0) {
        func_ov107_020c0b90(state->pOwner, 1, data_02041dc8, 0);
        state->bEffect6d = 0;
    }
    if (state->nCooldown64 > 0) {
        state->nCooldown64 -= node->pFrame->nDelta2c;
    }
    state->pOwner->vPendingMove = state->vForward2c;
    state->vForward2c = data_02041dc8;
}
