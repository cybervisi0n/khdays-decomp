
#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct Ov002TaskNodePath {
    void *pHook0;
    void *pHook1;
    void *pHook2;
    int nThreshold;
    char pad010[4];
    void *pfnStep;          /* per-kind step, null for any kind above 4 */
    u8 bOwner;
    char pad019[1];
    s16 h1a;
    s16 h1c;                /* seeded to -1 */
    char pad01e[2];
    int n20;
    int n24;
    int n28;
    VecFx32 place;
    char szName[0x10];
} Ov002TaskNodePath;

extern void *NNSi_FndAllocFromDefaultExpHeap(u32 nSize);
extern void strcpy(char *pDst, const char *pSrc);
extern void Ov002_FitsBudget(void);
extern void Ov002_TaskNodePathInRange(void);
extern void Ov002_IsWithinRangeHorizontal(void);
extern void Ov002_PointInBox3D(void);
extern void Ov002_PointInBox2D(void);
extern void Ov002_NodeWalkPlayersPhase(void);
extern void Ov002_ResolveActorModelAndBones(void);

/* Writes a hook slot only when the caller hands over something other than -1,
   which is how a caller says "leave this one alone".  A null is a real value
   here and does get written. */
static inline void Ov002_SetHooks(Ov002TaskNodePath *pNode, void *pA, void *pB,
                                  void *pC)
{
    if (pA != (void *)-1) {
        pNode->pHook0 = pA;
    }
    if (pB != (void *)-1) {
        pNode->pHook1 = pB;
    }
    if (pC != (void *)-1) {
        pNode->pHook2 = pC;
    }
}

/* Allocates a path node and fills it in from the nine arguments the submitter
   spreads out of its argument block.  Never checks the allocation. */
void *Ov002_CreateTaskNodePath(int nOwner, int nKind, int nValue,
                          const VecFx32 *pPlace, int n20, int n24, int n28,
                          const char *pName, int nThreshold)
{
    Ov002TaskNodePath *pNode;

    pNode = NNSi_FndAllocFromDefaultExpHeap(0x48);
    pNode->bOwner = (u8)nOwner;
    pNode->place = *pPlace;
    pNode->n20 = n20;
    pNode->n24 = n24;
    pNode->n28 = n28;
    pNode->h1a = (s16)nValue;
    pNode->h1c = -1;
    pNode->szName[0] = 0;
    pNode->nThreshold = nThreshold;
    if (pName != 0) {
        strcpy(pNode->szName, pName);
    }

    pNode->pfnStep = 0;
    switch (nKind) {
    case 0:
        pNode->pfnStep = (void *)Ov002_FitsBudget;
        break;
    case 1:
        pNode->pfnStep = (void *)Ov002_TaskNodePathInRange;
        break;
    case 2:
        pNode->pfnStep = (void *)Ov002_IsWithinRangeHorizontal;
        break;
    case 3:
        pNode->pfnStep = (void *)Ov002_PointInBox3D;
        break;
    case 4:
        pNode->pfnStep = (void *)Ov002_PointInBox2D;
        break;
    }

    Ov002_SetHooks(pNode, (void *)Ov002_NodeWalkPlayersPhase, (void *)0,
                   (void *)Ov002_ResolveActorModelAndBones);
    return pNode;
}
