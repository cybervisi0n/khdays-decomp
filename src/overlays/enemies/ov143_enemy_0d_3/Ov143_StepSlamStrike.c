/* The slam's follow-through (Ghidra: Ov143_StepSlamStrike).
 *
 * Every frame it re-aims the sub-object at its stored facing and rescales the
 * velocity, then builds the query the two search modes share: the anchor
 * position, the facing, the speed and a fixed 0x800 range.
 *
 * Mode 0 sweeps the actor list and asks the shared checker whether each
 * candidate is hit; the first acceptance ends the action. Mode 1 instead locks
 * on, fills a request with the owner's id, the object's kind and the lock
 * handle, and ends the action if the lock is taken. Either way the ending
 * broadcasts a type 5 action 0 placement command and fires reaction 0x53.
 *
 * With neither search producing anything the timer advances by the speed, and
 * the action still ends -- with a type 5 action 1 command and no sound -- once
 * the object reports contact or the timer passes 0xa000.
 *
 * Three codegen notes, because the frame layout is what this function is really
 * about.
 *
 * Coordinates are held in a one-value wrapper type (Fx32). This is a tentative
 * reconstruction of the original's coordinate type, not a proven one: copying a
 * wrapped value is a struct copy, which mwcc keeps, and that is the ROM's unread
 * scratch word for each anchor component.
 *
 * Those nine scratch words are nine separate one-word values, not three vectors.
 * Spelled as three twelve-byte vectors they were parked above the two requests
 * instead of below them, and every slot in the function shifted by the 0x24
 * bytes they occupy.
 *
 * Within one group the slots are handed out in reverse declaration order, which
 * is why the three components of each site are declared z, y, x, and why the
 * loop bound is declared after the loop counter.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/ai_task.h"

typedef struct { int value; } Fx32;
typedef struct { Fx32 x, y, z; } FxVec;

struct Ov143Cmd { u16 h[7]; };
struct Ov143Quat { int q[4]; };

struct Ov143Query {
    FxVec vAnchor;
    VecFx32 vFacing;
    int nSpeed;
    int nRange;
};

/* The same 44-byte command the four sibling overlays build for this entry
   point, kept field-for-field identical to them. */
struct HitCommand {
    u32 flags00;
    VecFx32 vector04;
    u32 field10;
    u32 field14;
    void *hit18;
    int pad1c[4];
};

struct Ov143Contact { u8 bGrounded : 1, bBlocked : 1; };

struct Ov143Owner {
    char pad000[0x290];
    u16 nId290;
};

struct Ov143SubObj {
    char pad000[0x24];
    void (*pMsgHook24)(struct Ov143SubObj *self, void *msg, int len); /* 0x024 */
    char pad028[0x78];
    char aSrtA0[0x2c];                                             /* 0x0a0 */
    char pad0cc[0xae];
    u8 bContact17a;                                                /* 0x17a */
    char pad17b[0x4c];
    u8 bSubState1c7;                                               /* 0x1c7 */
    char pad1c8[0x90];
    int nKind258;                                                  /* 0x258 */
    int nLockParam25c;                                             /* 0x25c */
    char pad260[0x138];
    struct Ov143Owner *pOwner398;                                  /* 0x398 */
};

struct Ov143StepState {
    struct Ov143SubObj *pSelf;   /* 0x00 */
    FxVec *pAnchor;              /* 0x04 */
    VecFx32 vVelocity08;     /* 0x08 */
    VecFx32 vFacing14;       /* 0x14 */
    int nMode20;                 /* 0x20 */
    int nTimer24;                /* 0x24 */
    int nSpeed28;                /* 0x28 */
};

struct Ov143StepNode {
    AI_TASK_FIELDS(struct Ov143StepState)
};

extern VecFx32 data_02042258;
extern VecFx32 data_02041dc8;
extern struct Ov143Cmd data_ov143_020d6270;
extern struct Ov143Cmd data_ov143_020d628c;
extern struct Ov143Cmd data_ov143_020d629a;

extern void Quat_FromTwoVectors(struct Ov143Quat *out, const VecFx32 *from,
                          const VecFx32 *to);
extern void Srt_SetRotationQuat(void *srt, const struct Ov143Quat *rot);
extern void ScaleVec3Fx12(int scale, const VecFx32 *src, VecFx32 *dst);
extern int Ov107_CollectSegmentOverlaps(struct Ov143Owner *owner, struct Ov143Query *query,
                               int *results);
extern int Ov107_InvokeHitCallback(int ent, struct Ov143SubObj *self,
                               struct Ov143Owner *owner, int mode, void *dir,
                               int flag);
extern void *Ov107_FindEntityHitBySegment(struct Ov143SubObj *self, struct Ov143Query *query,
                                 void **out);
extern int Ov107_AiState_ApplyHit(void *lock, int param, struct HitCommand *req);
extern void Ov107_BuildAndSendUpdate(struct Ov143Owner *owner, int a, int id, void *at);
extern void SetIndexedSlot(struct Ov143StepNode *node, int slot, void *value);

#define PACK(cmd, dead, src, at)                                              \
    (dead) = (src);                                                           \
    ((u8 *)&(cmd))[at] = (u8)(((unsigned int)(dead).value >> 0x10 & 0x7f)     \
                              | ((unsigned int)(dead).value >> 0x18 & 0x80)); \
    ((u8 *)&(cmd))[(at) + 1] = (u8)((unsigned int)(dead).value >> 8);         \
    ((u8 *)&(cmd))[(at) + 2] = (u8)(dead).value

void Ov143_StepSlamStrike(struct Ov143StepNode *node)
{
    struct Ov143StepState *state = node->pState;
    struct Ov143Query query;
    struct Ov143Quat rot;
    int results[4];
    struct Ov143Cmd cmdHit;
    void *handle;
    Fx32 hitScratchZ;
    Fx32 hitScratchY;
    Fx32 hitScratchX;
    Fx32 lockScratchZ;
    Fx32 lockScratchY;
    Fx32 lockScratchX;
    Fx32 endScratchZ;
    Fx32 endScratchY;
    Fx32 endScratchX;
    FxVec *anchor;
    void *lock;
    int i;
    int n;

    Quat_FromTwoVectors(&rot, &data_02042258, &state->vFacing14);
    Srt_SetRotationQuat(state->pSelf->aSrtA0, &rot);
    ScaleVec3Fx12(state->nSpeed28, &state->vFacing14, &state->vVelocity08);

    query.vAnchor = *state->pAnchor;
    query.vFacing = state->vFacing14;
    query.nSpeed = state->nSpeed28;
    query.nRange = 0x800;

    if (state->nMode20 == 0) {
        n = Ov107_CollectSegmentOverlaps(state->pSelf->pOwner398, &query, results);
        i = 0;
        if (n > 0) {
            do {
                if (Ov107_InvokeHitCallback(results[i], state->pSelf,
                                        state->pSelf->pOwner398, 0,
                                        &state->vVelocity08, 0) != 0) {
                    cmdHit = data_ov143_020d6270;
                    anchor = state->pAnchor;
                    PACK(cmdHit, hitScratchX, anchor->x, 5);
                    PACK(cmdHit, hitScratchY, anchor->y, 8);
                    PACK(cmdHit, hitScratchZ, anchor->z, 11);
                    if (state->pSelf->pMsgHook24 != 0) {
                        state->pSelf->pMsgHook24(state->pSelf, &cmdHit, 0xe);
                    }
                    Ov107_BuildAndSendUpdate(state->pSelf->pOwner398, 0, 0x53,
                                        state->pAnchor);
                    state->pSelf->bSubState1c7 = 0;
                    SetIndexedSlot(node, node->slot, 0);
                    return;
                }
            } while (++i < n);
        }
    } else {
        struct HitCommand spare = { 0 };

        if ((lock = Ov107_FindEntityHitBySegment(state->pSelf, &query, &handle)) != 0) {
            struct HitCommand req = { 0 };

            req.flags00 = (req.flags00 & 0xffff0000) | 0x2004;
            req.vector04 = data_02041dc8;
            req.field10 = (req.field10 & 0xffff0000)
                  | (u16)state->pSelf->pOwner398->nId290;
            req.field14 = (req.field14 & 0xffff0000)
                  | (u16)state->pSelf->nKind258;
            req.hit18 = handle;
            if (Ov107_AiState_ApplyHit(lock, state->pSelf->nLockParam25c, &req) != 0) {
                struct Ov143Cmd cmdLock;

                cmdLock = data_ov143_020d628c;
                anchor = state->pAnchor;
                PACK(cmdLock, lockScratchX, anchor->x, 5);
                PACK(cmdLock, lockScratchY, anchor->y, 8);
                PACK(cmdLock, lockScratchZ, anchor->z, 11);
                if (state->pSelf->pMsgHook24 != 0) {
                    state->pSelf->pMsgHook24(state->pSelf, &cmdLock, 0xe);
                }
                Ov107_BuildAndSendUpdate(state->pSelf->pOwner398, 0, 0x53,
                                    state->pAnchor);
                state->pSelf->bSubState1c7 = 0;
                SetIndexedSlot(node, node->slot, 0);
                return;
            }
        }
    }

    state->nTimer24 += state->nSpeed28;
    if (((struct Ov143Contact *)&state->pSelf->bContact17a)->bGrounded == 0
        && ((struct Ov143Contact *)&state->pSelf->bContact17a)->bBlocked == 0
        && state->nTimer24 <= 0xa000) {
        return;
    }

    {
    struct Ov143Cmd cmdEnd;

    cmdEnd = data_ov143_020d629a;
    anchor = state->pAnchor;
    PACK(cmdEnd, endScratchX, anchor->x, 5);
    PACK(cmdEnd, endScratchY, anchor->y, 8);
    PACK(cmdEnd, endScratchZ, anchor->z, 11);
    if (state->pSelf->pMsgHook24 != 0) {
        state->pSelf->pMsgHook24(state->pSelf, &cmdEnd, 0xe);
    }
    state->pSelf->bSubState1c7 = 0;
    SetIndexedSlot(node, node->slot, 0);
    }
}
