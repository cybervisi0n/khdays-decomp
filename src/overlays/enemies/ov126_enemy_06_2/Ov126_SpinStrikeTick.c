/* Spin strike tick of the ov125 enemy (the Ov143_StepSlamStrike shape). The query the two
 * search modes share is the pool entry's own 32-byte block at +0x78. Mode 0 sweeps the actor
 * list and asks the shared checker whether each candidate is hit along the unit direction from
 * the pool entry's +4 anchor (scaled 0x800); the first acceptance ends the action. Mode 1
 * instead locks on, fills a request with the owner's id, the object's kind and the lock handle
 * (flags 0x2024) and, if the handle's +8 bit 0 is set and the lock is taken, ends the action.
 * Either ending broadcasts the overlay's 14-byte placement command with the anchor packed in
 * and fires reaction 0x11b/6 at it. With neither search producing anything the timer advances
 * by 0xc00 and the action ends -- plainly past 0xa000, or with the end command once the object
 * reports contact.
 *
 * Codegen as in ov143: coordinates are packed through Fx32 wrapper copies whose unread scratch
 * words are nine separate values declared z, y, x per site. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/actor.h"
#include "game/ai_task.h"

typedef struct { int value; } Fx32;

struct Ov125Cmd { u16 h[7]; };
struct Ov125Query { int w[8]; };

struct HitCommand {
    u32 flags00;
    VecFx32 vector04;
    u32 field10;
    u32 field14;
    void *hit18;
    int pad1c[4];
};

struct Ov125Contact { u8 bGrounded : 1, bBlocked : 1; };
struct Ov125Byte8 { u32 lo : 8, rest : 24; };

struct Ov125PoolEntry {
    int w00;
    Fx32 x;                      /* 0x04 */
    Fx32 y;
    Fx32 z;
    char pad10[0x68];
    struct Ov125Query query;     /* 0x78 */
};

struct Ov125Item {
    char pad000[0x24];
    void (*pMsgHook24)(struct Ov125Item *self, void *msg, int len);
    char pad028[0x268];
    u16 nId290;
};

struct Ov125Actor {
    Actor base;                  /* 0x000 */
    struct Ov125Item *pItem38c;
};

struct Ov125State {
    struct Ov125Actor *pActor;   /* 0x00 */
    char pad04[0x14];
    int nTimer18;                /* 0x18 */
    int nMode1c;                 /* 0x1c */
};

struct Ov125Node {
    AI_TASK_FIELDS(struct Ov125State)
};

extern VecFx32 data_02041dc8;
extern struct Ov125Cmd data_ov126_020d4046;
extern struct Ov125Cmd data_ov126_020d4038;
extern struct Ov125Cmd data_ov126_020d4054;

extern int Ov107_CollectSegmentOverlaps(struct Ov125Item *item, struct Ov125Query *query, int *results);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *src, VecFx32 *dst);
extern int Ov107_InvokeHitCallback(int ent, struct Ov125Actor *actor, struct Ov125Item *item, int mode, void *dir, int flag);
extern struct Ov125Actor *Ov107_FindEntityHitBySegment(struct Ov125Actor *actor, struct Ov125Query *query, void **out);
extern int Ov107_AiState_ApplyHit(struct Ov125Actor *lock, int param, struct HitCommand *req);
extern void Ov107_BuildAndSendUpdate(struct Ov125Item *item, int id, int a, void *at);
extern void SetIndexedSlot(struct Ov125Node *node, int slot, void *value);

#define PACK(cmd, dead, src, at)                                              \
    (dead) = (src);                                                           \
    ((u8 *)&(cmd))[at] = (u8)(((unsigned int)(dead).value >> 0x10 & 0x7f)     \
                              | ((unsigned int)(dead).value >> 0x18 & 0x80)); \
    ((u8 *)&(cmd))[(at) + 1] = (u8)((unsigned int)(dead).value >> 8);         \
    ((u8 *)&(cmd))[(at) + 2] = (u8)(dead).value

void Ov126_SpinStrikeTick(struct Ov125Node *node)
{
    struct Ov125State *state = node->pState;
    struct Ov125Query query;
    int results[4];
    VecFx32 dir;
    struct Ov125Cmd cmdHit;
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
    struct Ov125PoolEntry *anchor;
    struct Ov125Actor *lock;
    int i;
    int n;

    query = (*((struct Ov125PoolEntry **)state->pActor->base.pPoolEntry))->query;

    if (state->nMode1c == 0) {
        n = Ov107_CollectSegmentOverlaps(state->pActor->pItem38c, &query, results);
        i = 0;
        if (n > 0) {
            do {
                VEC_Subtract((char *)results[i] + 0x74, &(*((struct Ov125PoolEntry **)state->pActor->base.pPoolEntry))->x, &dir);
                dir.y = 0;
                VEC_Normalize(&dir, &dir);
                ScaleVec3Fx12(0x800, &dir, &dir);
                if (Ov107_InvokeHitCallback(results[i], state->pActor, state->pActor->pItem38c, 0, &dir, 0) != 0) {
                    cmdHit = data_ov126_020d4046;
                    anchor = *((struct Ov125PoolEntry **)state->pActor->base.pPoolEntry);
                    PACK(cmdHit, hitScratchX, anchor->x, 5);
                    PACK(cmdHit, hitScratchY, anchor->y, 8);
                    PACK(cmdHit, hitScratchZ, anchor->z, 11);
                    if (state->pActor->pItem38c->pMsgHook24 != 0) {
                        state->pActor->pItem38c->pMsgHook24(state->pActor->pItem38c, &cmdHit, 0xe);
                    }
                    Ov107_BuildAndSendUpdate(state->pActor->pItem38c, 0x11b, 6, &(*((struct Ov125PoolEntry **)state->pActor->base.pPoolEntry))->x);
                    state->pActor->base.nextState = 0;
                    SetIndexedSlot(node, node->slot, 0);
                    return;
                }
            } while (++i < n);
        }
    } else {
        struct HitCommand spare = { 0 };

        if ((lock = Ov107_FindEntityHitBySegment(state->pActor, &query, &handle)) != 0
            && (lock->base.field_1ac & 4) == 0) {
            struct HitCommand req = { 0 };

            req.flags00 = (req.flags00 & 0xffff0000) | 0x2024;
            req.vector04 = data_02041dc8;
            req.field10 = (req.field10 & 0xffff0000) | (u16)state->pActor->pItem38c->nId290;
            req.field14 = (req.field14 & 0xffff0000) | (u16)state->pActor->base.field_258;
            req.hit18 = handle;
            if ((((struct Ov125Byte8 *)((char *)handle + 8))->lo & 1) != 0
                && Ov107_AiState_ApplyHit(lock, ((int)state->pActor->base.field_25c), &req) != 0) {
                struct Ov125Cmd cmdLock;

                cmdLock = data_ov126_020d4038;
                anchor = *((struct Ov125PoolEntry **)state->pActor->base.pPoolEntry);
                PACK(cmdLock, lockScratchX, anchor->x, 5);
                PACK(cmdLock, lockScratchY, anchor->y, 8);
                PACK(cmdLock, lockScratchZ, anchor->z, 11);
                if (state->pActor->pItem38c->pMsgHook24 != 0) {
                    state->pActor->pItem38c->pMsgHook24(state->pActor->pItem38c, &cmdLock, 0xe);
                }
                Ov107_BuildAndSendUpdate(state->pActor->pItem38c, 0x11b, 6, &(*((struct Ov125PoolEntry **)state->pActor->base.pPoolEntry))->x);
                state->pActor->base.nextState = 0;
                SetIndexedSlot(node, node->slot, 0);
                return;
            }
        }
    }

    state->nTimer18 += 0xc00;
    if (state->nTimer18 > 0xa000) {
        state->pActor->base.nextState = 0;
        SetIndexedSlot(node, node->slot, 0);
        return;
    }
    if (((struct Ov125Contact *)&state->pActor->base.contact17a.raw)->bGrounded == 0
        && ((struct Ov125Contact *)&state->pActor->base.contact17a.raw)->bBlocked == 0) {
        return;
    }

    {
    struct Ov125Cmd cmdEnd;

    cmdEnd = data_ov126_020d4054;
    anchor = *((struct Ov125PoolEntry **)state->pActor->base.pPoolEntry);
    PACK(cmdEnd, endScratchX, anchor->x, 5);
    PACK(cmdEnd, endScratchY, anchor->y, 8);
    PACK(cmdEnd, endScratchZ, anchor->z, 11);
    if (state->pActor->pItem38c->pMsgHook24 != 0) {
        state->pActor->pItem38c->pMsgHook24(state->pActor->pItem38c, &cmdEnd, 0xe);
    }
    state->pActor->base.nextState = 0;
    SetIndexedSlot(node, node->slot, 0);
    }
}
