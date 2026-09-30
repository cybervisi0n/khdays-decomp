/* Pillar +0x28 handler: sets stance bit 0, registers in the region and snaps to the source anchor.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/actor.h"

extern void Ov107_RegisterChildInRegion(Actor *node, int region);
extern void Ov107_MoveNodeAndRelayout(Actor *node, VecFx32 *anchor);

void Ov107_Pillar_EnterRegion(Actor *node, int region) {
    node->flags60.raw = (u16)((node->flags60.raw & 0xffff00ff) |
                  ((((u32)node->flags60.raw << 0x10) >> 0x18 | 1) << 0x18) >> 0x10);
    Ov107_RegisterChildInRegion(node, region);
    if (node->field_18c != 0) {
        node->vChaseTarget = *(VecFx32 *)(((char *)node->field_18c) + 0x48c);
        Ov107_MoveNodeAndRelayout(node, &node->vChaseTarget);
    }
}
