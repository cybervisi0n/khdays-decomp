/* Reset the ov106 scene's +0x8bc4 widget position: while +0x8ccc is set, its +0x8c68 position takes the
 * default (data_ov106_020b8a78) and the widget refreshes (0202aa9c). */

#include "nitro/fx_types.h"

extern char *data_ov106_020b8b60;
extern const VecFx32 data_ov106_020b8a78;
extern void Scene_DrawNode(void *widget);

void Ov106_ResetMarkerWidget(void)
{
    char *scene = data_ov106_020b8b60;

    if (*(int *)(scene + 0x8ccc) == 0) {
        return;
    }
    *(VecFx32 *)(scene + 0x8c68) = data_ov106_020b8a78;
    Scene_DrawNode(scene + 0x8bc4);
}
