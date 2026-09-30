

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/ai_task.h"
#include "game/engine.h"

typedef struct { int value; } Fx32;
typedef struct { Fx32 x, y, z; } FxVec;
struct Ov120AreaMsg { u16 h[7]; };
struct Ov107SweepQuery { FxVec vCentre; int nRadius; };

struct Ov120AttachBody {
    char pad000[0x14];
    VecFx32 vPos14;
};

struct Ov107SweepEntity {
    char pad000[2];
    u16 wId2;
};

struct Ov120BoneXform { int w[11]; };

struct Ov120Actor {
    char pad000[0x24];
    void (*pMsgHook24)(struct Ov120Actor *actor, void *msg, int len);
    char pad028[0x78];
    struct Ov120BoneXform xfm;
    char pad0cc[0x2c4];
    struct Ov120AttachBody *aBones390[4];
    void *pActionResource3a0;
};

struct Ov120ActionState {
    struct Ov120Actor *pOwner;
    char pad004[8];
    void *pEventAnchor;
    char pad010[0xc];
    VecFx32 vVelocity;
    VecFx32 vPos28;
    char pad034[0xc];
    int nElapsed40;
    char pad044[4];
    u8 *pGate48;
    u8 bOneShot4c;
    u8 bHitMask4d;
};

struct SceneFrameClock {
    char pad00[0x2c];
    int nDelta2c;
};

struct Ov120ActionNode {
    AI_TASK_FIELDS(struct Ov120ActionState)
};

extern const VecFx32 data_02041dc8;
extern struct Ov120AreaMsg data_ov121_020cfd22;
extern void Ov121_DecayCopyPosFireOnHitFlag(void);

extern int Ov107_ActionResource_GetOffsetAndScale(void *resource, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, void *src, void *dst);
extern void VEC_Add(void *a, void *b, void *out);
extern void func_ov107_020c0b90(struct Ov120Actor *actor, int mode, VecFx32 v,
                                int flag);
extern void Ov107_BuildAndSendUpdate(struct Ov120Actor *actor, int id, int mode, void *anchor);
extern int Ov107_CollectSphereOverlaps(struct Ov120Actor *actor, void *query, void *results);
extern int Ov107_InvokeHitCallback(struct Ov107SweepEntity *ent, struct Ov120Actor *a,
                               struct Ov120Actor *b, int mode, void *dir, int flag);
extern void SetIndexedSlot(void *node, int idx, void *value);

/*
 * Per-frame step of the two-point sweep attack action.
 *
 * Builds the swing offset from the action resource, rotates it into the actor
 * frame and scales it, then offsets bone attachments 1 and 2 by it to get the
 * two sweep points.  At 0x2a8 ticks it fires the one-shot effect and event.
 * From 0xff0 ticks it sweeps a 0x300 radius around each point, skips entities
 * already recorded in the per-action hit mask, packs the contact point into the
 * 14-byte area message and publishes it through the actor message hook.  Once
 * the gate byte clears it publishes the offset and advances the node to the
 * follow-up action.
 *
 * The two point cursors hold the same address throughout: the ROM keeps one for
 * the sweep query and the packed contact point and a second for the event
 * anchor.  Both counters are declared long and in this order on purpose -- long
 * and int are both 32 bits here, so the code shape is unchanged, but the
 * allocation order is what places the loop counter and the two cursors in the
 * registers the ROM uses.
 *
 * Coordinates are held in a one-value wrapper type (Fx32), a tentative
 * reconstruction of the original's coordinate type.  Copying a wrapped value is
 * a struct copy, which mwcc keeps, and that is the ROM's unread stack copy of
 * the contact point.
 */
void Ov121_SweepAttack_Step(struct Ov120ActionNode *node)
{
    struct Ov107SweepEntity *aResults[4];
    FxVec aPoints[2];
    VecFx32 vLocalOffset;
    struct Ov107SweepQuery query;
    struct Ov120AreaMsg msg;
    struct Ov120AreaMsg tmpl;
    VecFx32 vZero;
    FxVec vContact;
    struct Ov120ActionState *state;
    long nFound;
    long i;
    FxVec *pSweepPoint;
    FxVec *pEventPoint;
    int nPointIndex;
    int scale;

    state = node->pState;
    scale = Ov107_ActionResource_GetOffsetAndScale(state->pOwner->pActionResource3a0, &vLocalOffset);
    Vec3TransformViaTempMtx(&state->vVelocity, &state->pOwner->xfm, &vLocalOffset);
    ScaleVec3Fx12(scale, &state->vVelocity, &state->vVelocity);
    VEC_Add(&state->pOwner->aBones390[1]->vPos14, &state->vVelocity, &aPoints[0]);
    VEC_Add(&state->pOwner->aBones390[2]->vPos14, &state->vVelocity, &aPoints[1]);
    state->nElapsed40 += ((struct SceneFrameClock *)node->pList)->nDelta2c;
    if (state->bOneShot4c == 0 && state->nElapsed40 >= 0x2a8) {
        state->bOneShot4c = 1;
        func_ov107_020c0b90(state->pOwner, 2, data_02041dc8, 0);
        Ov107_BuildAndSendUpdate(state->pOwner, 0x11a, 4, state->pEventAnchor);
    }
    if (state->nElapsed40 >= 0xff0) {
        pSweepPoint = aPoints;
        pEventPoint = aPoints;
        nPointIndex = 0;
        tmpl = data_ov121_020cfd22;
        vZero = data_02041dc8;
        do {
            query.vCentre = *pSweepPoint;
            query.nRadius = 0x300;
            nFound = Ov107_CollectSphereOverlaps(state->pOwner, &query, aResults);
            i = 0;
            if (nFound > 0) {
                do {
                    if ((state->bHitMask4d & (1 << aResults[i]->wId2)) == 0) {
                        if (Ov107_InvokeHitCallback(aResults[i], state->pOwner, state->pOwner,
                                                1, &state->vVelocity, 0) != 0) {
                            msg = tmpl;

                            vContact.x = pSweepPoint->x;
                            ((u8 *)&msg)[7] = (u8)vContact.x.value;
                            ((u8 *)&msg)[5] = (u8)(((unsigned int)vContact.x.value >> 0x10 & 0x7f)
                                                   | ((unsigned int)vContact.x.value >> 0x18 & 0x80));
                            ((u8 *)&msg)[6] = (u8)((unsigned int)vContact.x.value >> 8);

                            vContact.y = pSweepPoint->y;
                            ((u8 *)&msg)[10] = (u8)vContact.y.value;
                            ((u8 *)&msg)[8] = (u8)(((unsigned int)vContact.y.value >> 0x10 & 0x7f)
                                                   | ((unsigned int)vContact.y.value >> 0x18 & 0x80));
                            ((u8 *)&msg)[9] = (u8)((unsigned int)vContact.y.value >> 8);

                            vContact.z = pSweepPoint->z;
                            ((u8 *)&msg)[13] = (u8)vContact.z.value;
                            ((u8 *)&msg)[11] = (u8)(((unsigned int)vContact.z.value >> 0x10 & 0x7f)
                                                    | ((unsigned int)vContact.z.value >> 0x18 & 0x80));
                            ((u8 *)&msg)[12] = (u8)((unsigned int)vContact.z.value >> 8);

                            if (state->pOwner->pMsgHook24 != 0) {
                                state->pOwner->pMsgHook24(state->pOwner, &msg, 0xe);
                            }
                            state->bHitMask4d |= 1 << aResults[i]->wId2;
                            state->vVelocity = vZero;
                            Ov107_BuildAndSendUpdate(state->pOwner, 0x11a, 5, pEventPoint);
                        }
                    }
                } while (++i < nFound);
            }
            pSweepPoint++;
            pEventPoint++;
        } while (++nPointIndex < 2);
    }
    if (*state->pGate48 == 0) {
        state->vPos28 = state->vVelocity;
        SetIndexedSlot(node, node->slot, Ov121_DecayCopyPosFireOnHitFlag);
    }
}
