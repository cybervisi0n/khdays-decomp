/* Scores the hit parts of a candidate as lock-on targets (enabled, within range, reachable and in
 * line of sight); returns whether one was selected. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct Ov022LowByte32 {
    unsigned int lowByte : 8;
    unsigned int rest : 24;
} Ov022LowByte32;

typedef struct Ov022Candidate {
    char pad_0000[0x179];
    u8 special179;
    char pad_017a[0xb2];
    char list22c[4];
} Ov022Candidate;

typedef struct Ov022PartNode {
    void *item;
    char pad_0004[4];
    u32 flags8;
} Ov022PartNode;

typedef struct Ov022OriginLimit {
    VecFx32 origin;
    int distance;
} Ov022OriginLimit;

extern VecFx32 *func_ov022_020881f8(int index);
extern void *GetEntryField20ByIndex(int index);
extern Ov022PartNode *List_First(void *list);
extern int VEC_Distance(const VecFx32 *a, const VecFx32 *b);
extern int Ov107_HitShape_TestSphere(void *item, const Ov022OriginLimit *limit,
                               int mode);
extern void Ov022_ComputeApproachPoint(void *actor, const VecFx32 *target,
                                VecFx32 *result, int mode);
extern int func_ov022_02085cc0(u32 *state, const VecFx32 *result);
extern int Ov022_TestLineOfSight(int index, const VecFx32 *position);
extern void func_ov022_0208484c(u32 *selectionFlags,
                                Ov022Candidate *candidate,
                                Ov022PartNode *node,
                                const VecFx32 *result);
extern Ov022PartNode *List_Next(void *list);

int Ov022_ScoreCandidatePart(u32 *selectionFlags, int index,
                         Ov022Candidate *candidate, int bestDistance)
{
    VecFx32 resultPosition;
    Ov022OriginLimit originLimit;
    int result = 0;
    Ov022PartNode *node;
    VecFx32 *origin;
    void *actor;

    origin = func_ov022_020881f8(index);
    actor = GetEntryField20ByIndex(index);
    node = List_First(candidate->list22c);
    while (node != 0) {
        int valid = 0;

        if ((((Ov022LowByte32 *)&node->flags8)->lowByte & 2) == 0 &&
            (((Ov022LowByte32 *)&node->flags8)->lowByte & 1) != 0) {
            if (candidate->special179 != 1) {
                if (VEC_Distance(
                        (VecFx32 *)((char *)node->item + 4), origin) >
                    bestDistance) {
                    goto next_node;
                }
                originLimit.origin = *origin;
                originLimit.distance = bestDistance;
                if (Ov107_HitShape_TestSphere(node->item, &originLimit, 0) != 0) {
                    valid = 1;
                }
            } else {
                valid = 1;
            }

            if (valid != 0) {
                Ov022_ComputeApproachPoint(
                    actor, (VecFx32 *)((char *)node->item + 4),
                    &resultPosition, 1);
                if (func_ov022_02085cc0(&selectionFlags[7],
                                        &resultPosition) != 0) {
                    if (candidate->special179 == 1 ||
                        Ov022_TestLineOfSight(
                            index, (VecFx32 *)((char *)node->item + 4)) != 0) {
                        func_ov022_0208484c(selectionFlags, candidate, node,
                                            &resultPosition);
                        result = 1;
                    }
                }
            }
        }

    next_node:
        node = List_Next(candidate->list22c);
    }

    return result;
}

