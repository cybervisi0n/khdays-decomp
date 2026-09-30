/* ov022: run one frame of an actor's state, with a chance to follow up.
 *
 * A flag goes up on the first player's machine before anything else and comes
 * back down when the step is over, so it marks the step itself.
 *
 * The recoil is clamped while the action bit is not held: a sentinel means none
 * is pending, and anything past the floor is held at the floor.
 *
 * Then the follow-up. It wants the animation late enough to be committed, the
 * action bit held, a test on the follow block to pass, an aim to exist and one
 * button to be down. All five and the actor goes straight into the follow state
 * without stepping at all.
 *
 * Otherwise the ordinary step: the drift's horizontal part folded into the
 * push, the state's own hook, and the same two ways of ending it. The drift is
 * zeroed on the way out, which the plain step does not do.
 *
 * A state that actually changed gets one more call, and one word is cleared
 * however the frame went.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

#define ACTION_BIT 4
#define NODE_NO_ANIM 0x20
#define SENTINEL 0x80000000
#define RECOIL_FLOOR (-0x420)
#define ANIM_LATE 0xf000
#define FOLLOW_BUTTON 0x800
#define STATE_FOLLOW 8

#define FLAGS_DONE 0x2000000000000ULL
#define FLAGS_SKILL 0x1000000000ULL
#define FLAGS2_IN_STEP 0x100ULL
#define FLAGS2_FOLLOWED 0x40000000ULL
#define FLAGS2_REQUEST 0x2ULL
#define FLAGS2_SKILL 0x80ULL

/* Ov022ActorNode */
struct ActorNode {
    u32 nFlags;                  /* 0x00 */
    u16 nAnimFlags;              /* 0x04 */
    u8 pad06[2];
};

struct Actor;

typedef void (*PreDrawFn)(struct Actor *pActor, int nReplay);
typedef int (*PostDrawFn)(struct Actor *pActor);

/* Ov022Actor */
struct Actor {
    u64 nFlags;                  /* 0x0000 */
    u8 pad0008[0x12];
    u16 nButtons2;               /* 0x001a */
    u8 pad001c[4];
    struct ActorNode *pNode;     /* 0x0020 */
    u32 nInputMask;              /* 0x0024 */
    u8 pad0028[0x30];
    /* volatile: the ROM reads it again for the floor test instead of
     * reusing the value it just compared against the sentinel */
    volatile int nRecoil;        /* 0x0058 */
    u8 pad005c[0x408];
    u64 nFlags2;                 /* 0x0464 */
    u8 pad046c[0x10];
    VecFx32 vecDrift;     /* 0x047c */
    u8 pad0488[0x10];
    VecFx32 vecStep;      /* 0x0498 */
    u8 pad04a4[0x1b8];
    int nStepScratch;            /* 0x065c */
    u8 pad0660[4];
    PreDrawFn pfnPreDraw;        /* 0x0664 */
    PostDrawFn pfnPostDraw;      /* 0x0668 */
    u8 pad066c[0x28];
    u8 bSuppressDraw : 1;        /* 0x0694 bit 0 */
    u8 bStateOver : 1;           /* bit 1 */
    u8 nSpare694 : 1;
    u8 bActionLocked : 1;        /* bit 3 */
    u8 nFlags694High : 4;
    u8 pad0695[0x11b];
    int nAnimFrame;              /* 0x07b0 */
    u8 pad07b4[0x9d8];
    u8 blkFollow;                /* 0x118c */
    u8 pad118d[0x192f];
    int nAimAngle;               /* 0x2abc */
};

extern int Session_GetLocalPlayerIndex(void);
extern int Ov022_IsBit0Set_2(u8 *pBlk);
extern void Ov022_TakeDriftStep(struct Actor *pActor, VecFx32 *pOutStep);
/* Ov022_ActorSetState */
extern int Ov022_ActorSetState(struct Actor *pActor, int nState);
extern void func_ov022_0209c700(struct Actor *pActor);
/* SceneNode_Enable */
extern void SceneNode_Enable(u16 *pAnimFlags);
extern void VEC_Add(const VecFx32 *pA, const VecFx32 *pB,
                    VecFx32 *pOut);

int Ov022_StepActorFollow(struct Actor *pActor)
{
    VecFx32 vecDrift;
    VecFx32 vecMove;
    int nRet;

    nRet = 0;
    if (Session_GetLocalPlayerIndex() == 0) {
        pActor->nFlags2 = pActor->nFlags2 | FLAGS2_IN_STEP;
    }
    if ((pActor->nInputMask & ACTION_BIT) == 0) {
        if (pActor->nRecoil == SENTINEL) {
            pActor->nRecoil = 0;
        } else if (pActor->nRecoil < RECOIL_FLOOR) {
            pActor->nRecoil = RECOIL_FLOOR;
        }
    }
    if (pActor->nAnimFrame >= ANIM_LATE
        && (pActor->nInputMask & ACTION_BIT) != 0
        && Ov022_IsBit0Set_2(&pActor->blkFollow) != 0
        && pActor->nAimAngle >= 0
        && (pActor->nButtons2 & FOLLOW_BUTTON) != 0) {
        if (Session_GetLocalPlayerIndex() == 0) {
            pActor->nFlags2 = pActor->nFlags2 | FLAGS2_FOLLOWED;
        }
        nRet = Ov022_ActorSetState(pActor, STATE_FOLLOW);
    } else {
        Ov022_TakeDriftStep(pActor, &vecDrift);
        vecMove = vecDrift;
        vecMove.y = 0;
        VEC_Add(&pActor->vecStep, &vecMove, &pActor->vecStep);
        pActor->bStateOver = pActor->pfnPostDraw(pActor);
        if (pActor->bStateOver != 0) {
            pActor->nFlags = pActor->nFlags | FLAGS_DONE;
            if ((pActor->pNode->nFlags & NODE_NO_ANIM) == 0) {
                SceneNode_Enable(&pActor->pNode->nAnimFlags);
            }
            if (Session_GetLocalPlayerIndex() == 0) {
                pActor->nFlags2 = pActor->nFlags2 | FLAGS2_REQUEST;
            }
        }
        if ((pActor->nFlags2 & FLAGS2_REQUEST) != 0) {
            pActor->vecDrift.z = 0;
            pActor->vecDrift.y = 0;
            pActor->vecDrift.x = 0;
            if ((pActor->nInputMask & ACTION_BIT) != 0
                && ((pActor->nFlags & FLAGS_SKILL) == 0
                    || (pActor->nFlags2 & FLAGS2_SKILL) == 0)) {
                pActor->pfnPreDraw(pActor, 0);
                nRet = Ov022_ActorSetState(pActor, 0);
            } else {
                nRet = Ov022_ActorSetState(pActor, 2);
            }
            pActor->nFlags2 = pActor->nFlags2 & ~FLAGS2_IN_STEP;
        }
    }
    if (nRet != 0) {
        func_ov022_0209c700(pActor);
    }
    pActor->nStepScratch = 0;
    return nRet;
}
