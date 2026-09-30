/* Pick the next move target: while the +0x45c partner is latched, the actor's +0x3dc target is
 * re-picked; with one the +0x64 / +0x68 approach (0, 30.0) and +0x7c are set (returns 1),
 * otherwise a new waypoint is chosen (020cd5f4). Unlatched, the +0x6c-th point of the actor's
 * +0x434 route list is copied to +0x18, +0x6c advances modulo the +0x454 count and 1 is returned;
 * 0 when the route is empty. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern int Ov107_FindNearestObject(int obj, int flag);
extern int Ov254_ChooseWaypoint(int *state, int a, int *out);
extern VecFx32 *List_First(void *list);

int Ov254_PickRoutePoint(int *state)
{
    VecFx32 *p;
    int i;

    state[0x1f] = 0;
    if (*(int *)(*(int *)(*state + 0x45c) + 0x38c) != 0) {
        *(int *)(*state + 0x3dc) = Ov107_FindNearestObject(*state, 0);
        if (*(int *)(*state + 0x3dc) != 0) {
            state[0x19] = 0;
            state[0x1a] = 0x1e000;
            state[0x1f] = 1;
            return 1;
        }
        return Ov254_ChooseWaypoint(state, 1, 0);
    }
    if (*(int *)(*state + 0x454) > 0) {
        p = List_First((void *)(*state + 0x434));
        i = 0;
        while (p != 0) {
            if (i == state[0x1b]) {
                *(VecFx32 *)(state + 6) = *p;
                state[0x1b] = (state[0x1b] + 1) % *(int *)(*state + 0x454);
                return 1;
            }
            p = (VecFx32 *)List_Next((void *)(*state + 0x434));
            i++;
        }
    }
    return 0;
}
