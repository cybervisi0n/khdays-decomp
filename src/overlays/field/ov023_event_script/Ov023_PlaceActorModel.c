/* Ov023_PlaceActorModel -- Ov023_PlaceActorModel: bind an actor of the event to entity nActor
 * with model nModel and place it.  The entity (0202bfcc) goes into the actor's entity slot
 * (0202d968 on +0x15e0) and the model id into +0x1626; the position is pPos, offset by the
 * named spot of the model's group when one is given (0202bfa0, 0202b0b8), and is applied at
 * once (0202b450); the entity's heading becomes the actor's angle and target (+0x1a30 /
 * +0x1a34) and is pushed back onto it unless locked.  The actor is then reset
 * (Ov023_ActorInit 02088714) and, unless it already had flag bit 13 (+0x1a28), given its five
 * 0x24-byte animation slots (+0x1a20, 0xb4 bytes from the default heap, cleared). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct Ov023Entity {
    int  nFlags;              /* 0x00 */
    u16  wFlags;              /* 0x04 */
    u8   pad_06[0x80 - 0x06];
    u16  nAngle;              /* 0x80 */
} Ov023Entity;

typedef struct Ov023AnimSlot {
    int  aField[3];           /* 0x00 */
    int  nHandle;             /* 0x0c */
    int  aField10[5];         /* 0x10 */
} Ov023AnimSlot;              /* 0x24 */

typedef struct Ov023Actor {
    u8   pad_0000[0x15e0];
    Ov023Entity *pEntity;     /* 0x15e0 */
    u8   pad_15e4[0x1626 - 0x15e4];
    u16  wModel;              /* 0x1626 */
    u8   pad_1628[0x1a20 - 0x1628];
    Ov023AnimSlot *pSlots;    /* 0x1a20 */
    u8   pad_1a24[4];
    int  nFlags;              /* 0x1a28 */
    u8   pad_1a2c[4];
    int  nAngle;              /* 0x1a30 */
    int  nAngleTarget;        /* 0x1a34 */
} Ov023Actor;

extern void *GetTrackEntryBase(int nGroup);                             /* the actor group by id */
extern Ov023Entity *ArrayEntryPtrD0(int nEntity);                     /* Entity_Get */
extern void  Actor_InitEntityLink(Ov023Entity **ppSlot, Ov023Entity *pEntity); /* bind an entity slot */
extern int   Collision_ProbeGround(void *pGroup, char *pszSpot, VecFx32 *pOut); /* Group_GetSpotPosition */
extern void  VEC_Add(const VecFx32 *pA, const VecFx32 *pB, VecFx32 *pOut);
extern void  Actor_SetVecAndSyncChild(Ov023Entity *pEntity, VecFx32 *pPos);   /* Entity_SetPositionNow */
extern void  Ov023_ActorInit(Ov023Actor *pActor, int nModel, int nEntity); /* Ov023_ActorInit */
extern void *NNSi_FndAllocFromDefaultExpHeap(int nSize);
extern void  MI_CpuFill8(void *pDst, u32 nValue, u32 nSize);

/* Give an entity a heading unless it is locked (bit 5 of its flags). */
static inline void Ov023_EntitySetAngle(Ov023Entity *pEntity, int nAngle)
{
    if (!(pEntity->nFlags & 0x20)) {
        pEntity->nAngle = nAngle;
        pEntity->wFlags |= 0x20;
    }
}

void Ov023_PlaceActorModel(Ov023Actor *pActor, char *pszSpot, VecFx32 *pPos, int nModel, int nActor)
{
    VecFx32 vPos;
    void *pGroup;
    Ov023Entity *pEntity;
    int nFlags;
    int i;

    pGroup = GetTrackEntryBase((u16)nModel);
    pEntity = ArrayEntryPtrD0((u16)nActor);
    nFlags = pActor->nFlags;
    vPos.x = vPos.y = vPos.z = 0;
    Actor_InitEntityLink(&pActor->pEntity, pEntity);
    pActor->wModel = nModel;
    if (pszSpot != 0) {
        Collision_ProbeGround(pGroup, pszSpot, &vPos);
    }
    VEC_Add(&vPos, pPos, &vPos);
    Actor_SetVecAndSyncChild(pActor->pEntity, &vPos);
    pActor->nAngleTarget = pActor->pEntity->nAngle;
    pActor->nAngle = pActor->nAngleTarget;
    Ov023_EntitySetAngle(pActor->pEntity, pActor->nAngle);
    Ov023_ActorInit(pActor, nModel, nActor);
    if (!(nFlags & 0x2000)) {
        pActor->pSlots = NNSi_FndAllocFromDefaultExpHeap(0xb4);
        MI_CpuFill8(pActor->pSlots, 0, 0xb4);
        for (i = 0; i < 5; i++) {
            pActor->pSlots[i].nHandle = 0;
        }
    }
}
