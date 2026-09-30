/* Reset the ov106 scene's +0x8cd0 widget: it is placed on data_ov106_020b8ae0 at the origin (depth
 * 2.0x, style 5, 020b7758), refreshed (020b7794) and the four queued points are freed. */

#include "nitro/fx_types.h"

struct Ov106Scene { char pad[0x8dd8]; VecFx32 points[4]; int used[4]; };

extern struct Ov106Scene *data_ov106_020b8b60;
extern char data_ov106_020b8ae0[];
extern void Ov106_PlaceWidget(void *self, void *owner, const VecFx32 *pos, int depth, int style);
extern void Ov106_BindNodeTracks02(void *self);

void Ov106_ResetTargetWidget(void)
{
    VecFx32 origin = {0, 0, 0};
    int i;

    Ov106_PlaceWidget((char *)data_ov106_020b8b60 + 0x8cd0, data_ov106_020b8ae0, &origin, 0x20000, 5);
    Ov106_BindNodeTracks02((char *)data_ov106_020b8b60 + 0x8cd0);
    for (i = 0; i < 4; i++) {
        data_ov106_020b8b60->used[i] = 0;
    }
}
