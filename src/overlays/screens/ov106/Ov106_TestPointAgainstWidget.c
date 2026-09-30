/* Whether the ov106 +0x8cd0 widget misses `point` seen through `cam` (0202a818 with a 1.5 margin in
 * mode 1, else 1.0): on a hit test of 0 the widget moves to the projected point (+0x8d74) and
 * refreshes. Returns the test result. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern char *data_ov106_020b8b60;
extern void Ov106_ProjectToScreen(VecFx32 *out, const VecFx32 *point, void *cam);
extern unsigned short Sequence_UpdateTracks(void *p, int a);
extern void Scene_DrawNode(void *widget);

int Ov106_TestPointAgainstWidget(void *cam, VecFx32 *point)
{
    VecFx32 pos;
    int margin = GetFrameRateMode() == 1 ? 0x1800 : 0x1000;
    int hit;

    Ov106_ProjectToScreen(&pos, point, cam);
    hit = Sequence_UpdateTracks(data_ov106_020b8b60 + 0x8cd0, margin);
    if (hit == 0) {
        *(VecFx32 *)(data_ov106_020b8b60 + 0x8d74) = pos;
        Scene_DrawNode(data_ov106_020b8b60 + 0x8cd0);
    }
    return hit;
}
