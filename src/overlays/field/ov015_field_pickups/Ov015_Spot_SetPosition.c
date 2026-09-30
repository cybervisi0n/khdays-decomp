/* Ov015_Spot_SetPosition -- push a 3-word transform through the node's matrix slots, ov015.
 * Copies src into the work slot (@+0x30), then work into the live slot (@+0x1c). */

#include "nitro/fx_types.h"

void Ov015_Spot_SetPosition(char *node, VecFx32 *src) {
    *(VecFx32 *)(node + 0x30) = *src;
    *(VecFx32 *)(node + 0x1c) = *(VecFx32 *)(node + 0x30);
}
