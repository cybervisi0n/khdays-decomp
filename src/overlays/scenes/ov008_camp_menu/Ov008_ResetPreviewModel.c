#include "nitro/fx_types.h"
#include "game/engine.h"

extern char *data_ov008_02090fac;
extern char data_ov008_02090dd8;
extern void BindAnimTrack(void *dst, int kind, void *src, short value);

/* Resets the preview model: default camera distances, no tint, and the idle pose. */
void Ov008_ResetPreviewModel(void) {
    char *obj = *(char **)&data_ov008_02090fac + 0xbfb8;
    VecFx32 zero;
    Projection_LoadDefaults(obj);
    *(int *)(obj + 0x18) = 0xe00;
    *(int *)(obj + 0x24) = 0x1100;
    *(int *)(obj + 0x28) = 0x1800;
    *(int *)(obj + 0x140) = 0;
    RegisterSeqAndInit(obj + 0x38, &data_ov008_02090dd8, 1, 0xf);
    zero.z = 0;
    zero.y = 0;
    zero.x = 0;
    *(VecFx32 *)(obj + 0xdc) = zero;
    BindAnimTrack(obj + 0x38, 0, obj + 0x118, 0);
}
