/* Walks the owner's point list up to the stored index and leaves that point in
 * the caller's slot at +0x1c, then clears the progress word at +0x28. The list
 * is the one the reloader fills; the walk stops early when the list is shorter
 * than the index. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void *List_First(void *list);

void Ov292_SeekPointAtIndex(char *state)
{
    VecFx32 *point;
    int i;

    point = List_First(*(char **)state + 0x394);
    i = 0;
    while (point != 0) {
        *(VecFx32 *)(state + 0x1c) = *point;
        if (i >= *(int *)(state + 0x34)) {
            break;
        }
        point = (VecFx32 *)List_Next(*(char **)state + 0x394);
        i++;
    }
    *(int *)(state + 0x28) = 0;
}
