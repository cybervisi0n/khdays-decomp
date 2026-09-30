/* Scores the other players of the same group within range as lock-on candidates (visible, in line
 * of sight); returns the best distance. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct Ov022Actor {
    char pad_0000[0x12];
    u16 flags12;
    char pad_0014[0x52];
    s16 group66;
    char pad_0068[0x3fc];
    u64 flags464;
} Ov022Actor;

extern void *NNSi_FndGetCurrentRootHeap(void);
extern VecFx32 *func_ov022_020881f8(int index);
extern int func_ov022_020882f8(void);
extern void func_ov022_020ad44c(VecFx32 *out, Ov022Actor *actor);
extern int VEC_Distance(const VecFx32 *a, const VecFx32 *b);
extern void Ov022_ComputeApproachPoint(Ov022Actor *actor, const VecFx32 *target,
                                VecFx32 *result, int mode);
extern int func_ov022_02085cc0(u32 *state, const VecFx32 *result);
extern int Ov022_TestLineOfSight(int index, const VecFx32 *position);
extern void func_ov022_02084880(u32 *selectionFlags, Ov022Actor *candidate,
                                const VecFx32 *result);

int Ov022_ScoreCandidateByDistance(u32 *selectionFlags, int index, int bestDistance)
{
    VecFx32 candidatePosition;
    VecFx32 resultPosition;
    int resultDistance = bestDistance;
    VecFx32 *origin;
    Ov022Actor *actor;
    int i;

    NNSi_FndGetCurrentRootHeap();
    origin = func_ov022_020881f8(index);
    actor = (Ov022Actor *)GetEntryField20ByIndex(index);

    for (i = 0; i < func_ov022_020882f8(); i++) {
        Ov022Actor *candidate;

        if (i != QueryActiveStateOrDelegate()) {
            candidate = (Ov022Actor *)GetEntryField20ByIndex(i);
            if (candidate->flags12 != 0 &&
                candidate->group66 == actor->group66 &&
                (candidate->flags464 & 0x200000000ULL) == 0) {
                func_ov022_020ad44c(&candidatePosition, candidate);
                if (VEC_Distance(&candidatePosition, origin) <= bestDistance) {
                    Ov022_ComputeApproachPoint(actor, &candidatePosition,
                                        &resultPosition, 3);
                    if (func_ov022_02085cc0(&selectionFlags[7],
                                            &resultPosition) != 0 &&
                        Ov022_TestLineOfSight(index,
                                            &candidatePosition) != 0) {
                        func_ov022_02084880(selectionFlags, candidate,
                                            &resultPosition);
                        resultDistance = resultPosition.x;
                    }
                }
            }
        }
    }

    return resultDistance;
}

