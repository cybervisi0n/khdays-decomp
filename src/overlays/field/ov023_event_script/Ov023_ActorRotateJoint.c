/* Ov023_ActorRotateJoint -- Ov023_ActorRotateJoint: start a rotation tween on one of the actor's
 * joints.  The joint is found by name on the entity's animation (+0x15e0; 02087510) and its
 * tween slot picked (Ov023_ActorFindRotTween 02087564 on the seven at +0x984): the joint,
 * duration and remaining count (+0x1c / +0x20) and easing mode (+0x24) are set, the current
 * target (+0) becomes the start (+0xc) and the new angles the target.  Flag bit 10 (+0x1a28)
 * is set and the entity's halfword at +4 gets bit 7. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct Ov023RotTween {
    VecFx32 vTarget;          /* 0x00 */
    VecFx32 vFrom;            /* 0x0c */
    int  nJoint;              /* 0x18 */
    int  nDuration;           /* 0x1c */
    int  nRemaining;          /* 0x20 */
    int  nMode;               /* 0x24 */
} Ov023RotTween;              /* 0x28 */

typedef struct Ov023Entity {
    int  nFlags;              /* 0x00 */
    u16  wFlags;              /* 0x04: the animation starts here */
} Ov023Entity;

typedef struct Ov023Actor {
    u8   pad_0000[0x984];
    Ov023RotTween aRotTween[7]; /* 0x0984 */
    u8   pad_0a9c[0x15e0 - 0xa9c];
    Ov023Entity *pEntity;     /* 0x15e0 */
    u8   pad_15e4[0x1a28 - 0x15e4];
    int  nFlags;              /* 0x1a28 */
} Ov023Actor;

extern int Ov023_SubmitRequestRecord(void *pAnim, char *pszJoint);       /* Ov023_FindAnimTrack */
extern int Ov023_ActorFindPending(Ov023Actor *pActor, int nJoint);    /* Ov023_ActorFindRotTween */

void Ov023_ActorRotateJoint(Ov023Actor *pActor, char *pszJoint, VecFx32 *pAngles, int nDuration, int nMode)
{
    int nJoint;
    Ov023RotTween *pTween;

    nJoint = Ov023_SubmitRequestRecord(&pActor->pEntity->wFlags, pszJoint);
    pTween = &pActor->aRotTween[Ov023_ActorFindPending(pActor, nJoint)];
    pTween->nJoint = nJoint;
    pTween->nRemaining = nDuration;
    pTween->nDuration = nDuration;
    pTween->nMode = nMode;
    pTween->vFrom = pTween->vTarget;
    pTween->vTarget = *pAngles;
    pActor->nFlags |= 0x400;
    pActor->pEntity->wFlags |= 0x80;
}
