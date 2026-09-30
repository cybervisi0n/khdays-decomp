
#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

extern void *Ov002_LookupChannelEntry(char *pChannel);
extern int Ov002_GetLastPositiveSlotValue(char *pNode);
extern void Ov002_ElementRetire(char *pElement);

/* Bring a timed element on screen.
 *
 * Binds the owner's channel entry to the element's object, remembers the
 * element's own width the first time the node is placed, moves the node to the
 * spawn position built at +0xe0, and then reads the element's game-state field
 * three times: once for the track index, once for the visibility bit the object
 * follows, and once for the bit that forces it hidden. Slot 2 is never forced.
 */
void Ov002_ElementAttach(char *pElement)
{
    char *pOwner;
    VecFx32 vec;
    void *pEntry;
    unsigned int nState;
    u16 wWidth;

    pOwner = *(char **)(pElement + 8);
    vec = *(VecFx32 *)(pElement + 0xe0);

    pEntry = Ov002_LookupChannelEntry(pOwner + 0x58);
    Entity_Register(pElement + 0x2c, pEntry, 1, 4);

    wWidth = *(u16 *)(pElement + 0x18);
    if ((*(int *)(pElement + 0x38) & 0x20) == 0) {
        *(u16 *)(pElement + 0xb8) = wWidth;
        *(u16 *)(pElement + 0x3c) |= 0x20;
    }

    Actor_SetVecAndSyncChild(pElement + 0x38, &vec);

    *(unsigned char *)(pElement + 0x1ba) =
        (unsigned char)Ov002_GetLastPositiveSlotValue(pElement + 0x3c);

    nState = GameState_GetField(*(u16 *)(pElement + 0x14), *(unsigned char *)(pElement + 0x16));
    *(unsigned char *)(pElement + 0x1b8) =
        (unsigned char)((((nState & 0xfffe) << 15) >> 16) & 1);

    *(u16 *)(pElement + 0x12) |= 4;

    Ov002_ElementRetire(pElement);

    nState = GameState_GetField(*(u16 *)(pElement + 0x14), *(unsigned char *)(pElement + 0x16));
    Obj_SetFlagBit3(pElement + 0x2c, (nState & 1) != 0);

    if (*(unsigned char *)(pElement + 0x16) == 2) {
        nState = 0;
    } else {
        nState = ((unsigned int)(GameState_GetField(*(u16 *)(pElement + 0x14),
                                               *(unsigned char *)(pElement + 0x16))
                                 & 0xfffe) << 15) >> 16;
        nState &= 2;
    }

    if (nState != 0) {
        *(unsigned char *)(pElement + 0x1bb) |= 0x40;
        Obj_SetFlagBit3(pElement + 0x2c, 0);
    } else {
        *(unsigned char *)(pElement + 0x1bb) &= ~0x40;
    }

    if (*(short *)(pOwner + 0x7e) >= 0) {
        Res_RequestIdPair(*(short *)(pOwner + 0x7e));
    }

    if (*(int *)(pOwner + *(unsigned char *)(pElement + 0x1b8) * 4 + 0x74) == 0) {
        *(u16 *)(pElement + 0x12) &= ~8;
    } else {
        *(u16 *)(pElement + 0x12) |= 8;
    }
}
