/* Whether a position lies between the current turn and the bound on the selector's side (left or
 * right of the local player's facing); returns the new bound. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

extern const VecFx32 *func_ov022_020881f8(int index);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);

int Ov022_TestSelectorTurn(const VecFx32 *position, const VecFx32 *basis,
                        int turn, int bound, int selectorId, int *newBound)
{
    VecFx32 delta;
    int candidateTurn;
    int result = 0;

    VEC_Subtract(position, func_ov022_020881f8(QueryActiveStateOrDelegate()), &delta);
    candidateTurn = (int)(((s64)delta.x * basis->z + 0x800) >> 12) -
                    (int)(((s64)delta.z * basis->x + 0x800) >> 12);

    if (selectorId == 0x200) {
        if (candidateTurn > turn && candidateTurn < bound) {
            result = 1;
        }
    } else if (candidateTurn < turn && candidateTurn > bound) {
        result = 1;
    }

    *newBound = candidateTurn;
    return result;
}
