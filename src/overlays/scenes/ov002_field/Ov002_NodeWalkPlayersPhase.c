
#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct Ov002TaskNode Ov002TaskNode;

typedef int (*Ov002NodeHook)(Ov002TaskNode *pNode);
/* Test a node runs against one player's position. */
typedef int (*Ov002NodePlayerTest)(Ov002TaskNode *pNode, VecFx32 *pPos);

/* Only the leading flag word of a player record matters here: bit 16 takes the
   player out of the walk. */
typedef struct Ov002PlayerRecord {
    unsigned long long nFlags;
} Ov002PlayerRecord;

struct Ov002TaskNode {
    Ov002NodeHook pHook0;
    Ov002NodeHook pHook1;
    Ov002NodeHook pHook2;
    int nThreshold;
    s8 nResult;
    s8 nLap;
    char pad012[2];
    Ov002NodePlayerTest pfnTest;
    s8 nMode;               /* 0 reports the first player to pass, 1 needs all */
    char pad019[3];
    s16 nWorld;             /* world the players have to be in */
};

/* Number of players the session currently holds; this caller reads the count
   as a signed int, which is what makes its loop bounds signed. */
extern int func_ov022_020882f8(void);
extern Ov002PlayerRecord *GetEntryField20ByIndex(int nPlayer);
extern VecFx32 *func_ov022_020881f8(int nPlayer);
extern int Ov022_GetEntryField66(int nPlayer);
/* Starts (bStart != 0) or stops a lap; returns the lap, negative on failure. */
extern int Ov002_SetLapRunning(int bStart, int nLap);
extern int Ov002_NodeFinishLap(Ov002TaskNode *pNode);
extern int Ov002_ResolveActorModelAndBones(Ov002TaskNode *pNode);
extern int Ov002_NodeGetResult(Ov002TaskNode *pNode);

/* Writes a hook slot unless the caller passes -1, which means "leave this one
   as it is". */
static inline void Ov002_SetHook(Ov002NodeHook *pSlot, Ov002NodeHook pHook)
{
    if ((int)pHook != -1) {
        *pSlot = pHook;
    }
}

/* Walks the players the session holds and runs the node's own test against
   each one that is still in play and in the wanted world.  Mode 0 reports the
   first player to pass; mode 1 succeeds only once every player has, and gives
   up as soon as one is in the wrong world or fails the test.  A node that
   comes out with an answer takes a lap of its own when it has a payload
   threshold and waits on the lap finisher, or moves straight to its follow-up
   hook. */
int Ov002_NodeWalkPlayersPhase(Ov002TaskNode *pNode)
{
    int i;
    VecFx32 *pPos;
    int nWorld;

    pNode->nResult = -2;
    if (pNode->nWorld < 0) {
        return -2;
    }

    switch (pNode->nMode) {
    case 0:
        for (i = 0; i < func_ov022_020882f8(); i++) {
            if ((GetEntryField20ByIndex(i)->nFlags & 0x10000) == 0) {
                pPos = func_ov022_020881f8(i);
                nWorld = Ov022_GetEntryField66(i);
                if (nWorld == pNode->nWorld) {
                    if (pNode->pfnTest(pNode, pPos) != 0) {
                        pNode->nResult = (s8)i;
                        break;
                    }
                }
            }
        }
        break;

    case 1:
        pNode->nResult = -1;
        for (i = 0; i < func_ov022_020882f8(); i++) {
            if ((GetEntryField20ByIndex(i)->nFlags & 0x10000) == 0) {
                pPos = func_ov022_020881f8(i);
                nWorld = Ov022_GetEntryField66(i);
                if (nWorld != pNode->nWorld) {
                    pNode->nResult = -2;
                    break;
                }
                if (pNode->pfnTest(pNode, pPos) == 0) {
                    pNode->nResult = -2;
                    break;
                }
            }
        }
        break;
    }

    if (pNode->nResult != -2) {
        if (pNode->nThreshold > 0) {
            pNode->nLap = (s8)Ov002_SetLapRunning(1, -1);
            if (pNode->nLap < 0) {
                return -2;
            }
            Ov002_SetHook(&pNode->pHook0, Ov002_NodeFinishLap);
            Ov002_SetHook(&pNode->pHook1, 0);
            Ov002_SetHook(&pNode->pHook2, Ov002_ResolveActorModelAndBones);
            return -2;
        }
        Ov002_SetHook(&pNode->pHook0, Ov002_NodeGetResult);
        Ov002_SetHook(&pNode->pHook1, 0);
        Ov002_SetHook(&pNode->pHook2, 0);
    }

    return pNode->nResult;
}
