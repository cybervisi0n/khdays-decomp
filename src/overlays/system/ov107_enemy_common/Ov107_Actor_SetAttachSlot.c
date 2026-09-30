/* Sets or clears attachment slot `slot` of an actor.
 *
 * kind 0 on a live slot destroys the slot's created item and frees the
 * 0x1c-byte slot record.  Otherwise the record is allocated on demand, tagged
 * with kind, given a position (the caller's, or the zero vector) and field_10,
 * and when the per-slot/per-kind table names a resource (not -1), an item is
 * created from a handle packed out of the actor manager's +0x88 sprite set and
 * that resource (the same packing as Ov107_PackTextureHandle), flagged with bit 1
 * at +0x5c, and the table's two parameter bytes are copied in.  Slot 2 is
 * mirrored into slot 3.
 *
 * The table is read-only (nothing in the ROM writes it); declaring it const is
 * what lets mwcc schedule its loads past the stores into the slot record. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct {
    char pad_00[0x5c];
    unsigned int flags_5c;
} CreatedItem;

typedef struct {
    unsigned int kind:4;
    VecFx32 pos;                 /* +0x04 */
    int field_10;             /* +0x10 */
    CreatedItem *item;        /* +0x14 */
    unsigned char field_18;   /* +0x18 */
    unsigned char field_19;   /* +0x19 */
} Slot;

typedef struct {
    char pad_00[0x350];
    Slot *slots[8];           /* +0x350 */
} Obj;

typedef struct {
    char pad_00[0x88];
    unsigned int spriteSet_88;
} ActorManager;

typedef struct {
    int resource;             /* -1: no item for this slot/kind */
    unsigned char field_4;
    unsigned char field_5;
    char pad_6[2];
} SlotKindInfo;

extern ActorManager *Ov107_GetActorManager(void);
extern void *CallocInstance(unsigned int size);
extern CreatedItem *CreateSubitemInstance0xB4(unsigned int handle);

extern const VecFx32 data_02041dc8;
extern const SlotKindInfo data_ov107_020cb9a4[][4];

void Ov107_Actor_SetAttachSlot(Obj *self, int slot, unsigned int kind, VecFx32 *pos, int field10)
{
    unsigned int spriteSet = Ov107_GetActorManager()->spriteSet_88;
    Slot *entry;
    VecFx32 v;
    int resource;

    if (kind == 0 && self->slots[slot] != 0) {
        if (self->slots[slot]->item != 0) {
            DestroyInstance(self->slots[slot]->item);
            self->slots[slot]->item = 0;
        }
        FreeInstanceMemory(self->slots[slot]);
        self->slots[slot] = 0;
        return;
    }

    if (self->slots[slot] == 0) {
        self->slots[slot] = CallocInstance(0x1c);
    }
    entry = self->slots[slot];
    entry->kind = kind;
    if (pos != 0) {
        v = *pos;
    } else {
        v = data_02041dc8;
    }
    entry->pos = v;
    entry->field_10 = field10;

    resource = data_ov107_020cb9a4[slot][kind].resource;
    if (resource != -1) {
        unsigned int mask = 0xfffffc;
        entry->item = CreateSubitemInstance0xB4((((spriteSet + 0x8000) & mask) << 7 | 0x80000000)
                                    | (resource & (mask >> 15)));
        entry->item->flags_5c |= 2;
        entry->field_18 = data_ov107_020cb9a4[slot][kind].field_4;
        entry->field_19 = data_ov107_020cb9a4[slot][kind].field_5;
    }

    if (slot == 2) {
        Ov107_Actor_SetAttachSlot(self, 3, kind, pos, field10);
    }
}
