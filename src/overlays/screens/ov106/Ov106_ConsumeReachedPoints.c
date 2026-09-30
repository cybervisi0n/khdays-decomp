/* Consume the ov106 scene's queued points that `target` reaches (020b7e38): each live one of the four
 * +0x8dd8 slots it hits is freed. */

#include "nitro/fx_types.h"

struct Ov106Scene { char pad[0x8dd8]; VecFx32 points[4]; int used[4]; };

extern struct Ov106Scene *data_ov106_020b8b60;
extern int Ov106_TestPointAgainstWidget(void *target, VecFx32 *point);

void Ov106_ConsumeReachedPoints(void *target)
{
    int i;

    for (i = 0; i < 4; i++) {
        if (data_ov106_020b8b60->used[i] != 0 && Ov106_TestPointAgainstWidget(target, &data_ov106_020b8b60->points[i]) != 0) {
            data_ov106_020b8b60->used[i] = 0;
        }
    }
}
