/* Ov245_SlotSpawnMsg5 -- message hook for kind-5 messages: slots 0..2 spawn a kind-5 child (kind
 * 0x15 for slot 0) under the slot's +0x3b4 parent at the +4 transform of the +0x394 item (slot 2)
 * or of the +0x398 rider's +0x44c anchor (020c09a0), landing in +0x3b8; slot 0 also attaches
 * resource 0x15a of kind 0xb at the actor's +0xa0 placement (020cb040) into +0x3ac. Slot 4 resets
 * the +0x3a0 vector and starts motion 0 of the +0x39c item (020c9ee8). Then the state filter. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Ov245Slots { char pad[0x3b4]; struct { int pParent; int pChild; } slots[3]; };

extern int Ov245_FilterStateMsg(int self, unsigned char *msg, int extra);
extern const VecFx32 data_02041dc8;

int Ov245_SlotSpawnMsg5(int self, unsigned char *msg, int extra) {
    int at;

    switch (msg[2]) {
    case 5:
        switch (msg[3]) {
        case 0:
        case 1:
        case 2:
            if (msg[3] == 2) {
                at = *(int *)(self + 0x394) + 4;
            } else {
                at = *(int *)(*(int *)(self + 0x398) + 0x44c) + 4;
            }
            ((struct Ov245Slots *)self)->slots[msg[3]].pChild =
                Ov107_CreateNodeBodyTask(*(int *)(self + 0x3c), ((struct Ov245Slots *)self)->slots[msg[3]].pParent,
                                    (unsigned char)((msg[3] == 0 ? 0x10 : 0) | 5), (void *)at, 0, 0);
            if (msg[3] == 0) {
                *(int *)(self + 0x3ac) = Ov107_CreateSpawnTask(self, 0x15a, 0xb, 1, (void *)(self + 0xa0));
            }
            break;
        case 4:
            *(VecFx32 *)(self + 0x3a0) = data_02041dc8;
            Ov107_StartAnim(*(int *)(self + 0x39c), 0, 0);
            break;
        }
        break;
    }
    return Ov245_FilterStateMsg(self, msg, extra);
}
