/* Place the widget: copy the caller's position into +0xa4 as one three-word
 * move, apply the anchor, drive sub-node 3 to the given value and refresh. */

#include "nitro/fx_types.h"

extern void Widget_SetTagWord(void *self, int anchor);
extern void Anim_SetFrameWrapped(void *self, int slot, int value);
extern void Scene_DrawNode(void *self);

void Ov002_PlaceWidget_2(char *self, const VecFx32 *pos, int anchor, int value) {
    *(VecFx32 *)(self + 0xa4) = *pos;
    Widget_SetTagWord(self, anchor);
    Anim_SetFrameWrapped(self, 3, value);
    Scene_DrawNode(self);
}
