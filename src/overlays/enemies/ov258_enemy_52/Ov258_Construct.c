/* Constructor of the ov258 enemy. Installs the handlers (+8, +0xc, +0x1c message, +0x20, +0x28, +0x2c,
 * +0x30, +0x34 update, +0x1d0 damage, +0x1dc), a 6.56 radius and the +0x64 pose (27.1 up), sets bits 3-4
 * of +0x1ae and bits 2 and 5 of the +0x60 high byte; builds the +0x384 body rig (pose 0, offset, work
 * list 1 in +0x388) and the +0x3ac head rig (pose 0x11, work list 0x12 in +0x3b0), ten bone attachments
 * (+0x42c..+0x450), the 43 hidden sub-items of data_ov258_020d1774 in the +0x464 pair table and two
 * 020cfd58 hands (+0x458). Shapes: eight 15.0-long upright capsules / placements (radii 1.875, 1.3125,
 * 1.5; placements 2.625 / 4.125) at +0x3d8, the first also mirrored on the +0x22c pool (+0x3d4). The
 * +0x460 partner flag comes from bit 2 of the save flags and picks sound 0x180 or 0x17b. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { int id[43]; } IdTable43;
typedef struct { VecFx32 pos; int scale; } Placement;
typedef struct { VecFx32 pos; VecFx32 axis; int length; int radius; } Capsule;
struct Pair { int res; int handle; };
struct Ov258Parts { char pad[0x3d8]; int parts[8]; char pad3f8[0x60]; int hands[2]; char pad460[4]; struct Pair items[43]; };

extern void Ov258_Actor_Destroy(void);
extern void func_ov258_020cc4f0(void);
extern void Ov258_ForwardEvent28ToHandler(void);
extern void Ov258_OnMessage(void);
extern void Ov258_CreateAiTask(void);
extern void Ov258_AddItemsToScene(void);
extern void Ov258_RemoveItemsFromScene(void);
extern void Ov258_Update(void);
extern void Ov258_OnDamage(void);
extern void Ov258_PlayRigMove(void);
extern void *Ov107_PackTextureHandle(char *self, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern void Srt_SetTranslationXYZ(int transform, int x, int y, int z);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void Snd_RegisterSeqAndBind(void *slot, int bank, void *record, int d);
extern void MainBlob_ResetSlotRows(int rig, void *slot);
extern int InsertSortedEntryWithKey(int item, int kind, void *name);
extern void Ov107_EnqueueValue(char *self, int item);
extern int Ov258_New(char *self, int side);
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern int Ov107_Mover_New(const Capsule *capsule);
extern int Ov107_CloneResourceTransform(const Placement *placement);
extern void Res_RequestIdPair(int resourceId);
extern const IdTable43 data_ov258_020d1774;
extern char data_ov258_020d186c[];
extern char data_ov258_020d1878[];
extern char data_ov258_020d1884[];
extern char data_ov258_020d1890[];
extern char data_ov258_020d18a0[];
extern char data_ov258_020d18ac[];
extern char data_ov258_020d18b8[];
extern char data_ov258_020d18c8[];
extern char data_ov258_020d18d4[];
extern char data_ov258_020d18dc[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042264;
extern u8 data_0204c240[];

void Ov258_Construct(char *self)
{
    IdTable43 ids = data_ov258_020d1774;
    Capsule cap;
    Placement place;
    VecFx32 zero;
    signed char i;
    int *slot;

    *(Callback *)(self + 0x8) = Ov258_Actor_Destroy;
    *(Callback *)(self + 0xc) = func_ov258_020cc4f0;
    *(Callback *)(self + 0x20) = Ov258_ForwardEvent28ToHandler;
    *(Callback *)(self + 0x1c) = Ov258_OnMessage;
    *(Callback *)(self + 0x30) = Ov258_CreateAiTask;
    *(Callback *)(self + 0x28) = Ov258_AddItemsToScene;
    *(Callback *)(self + 0x2c) = Ov258_RemoveItemsFromScene;
    *(Callback *)(self + 0x34) = Ov258_Update;
    *(Callback *)(self + 0x1d0) = Ov258_OnDamage;
    *(Callback *)(self + 0x1dc) = Ov258_PlayRigMove;
    /* overwritten defaults: spend the scheduling budget (keeps the ROM argument order below) */
    *(int *)(self + 0x70) = 0x1000;
    *(int *)(self + 0x70) = 0x6900;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x1b200;
    *(int *)(self + 0x6c) = 0x1000;
    *(int *)(self + 0x6c) = 0;
    *(u16 *)(self + 0x1ae) |= 8;
    {
        u16 hw = *(u16 *)(self + 0x60);

        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x20) << 0x18) >> 0x10);
    }
    *(u16 *)(self + 0x1ae) |= 0x10;
    {
        u16 hw = *(u16 *)(self + 0x60);

        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 4) << 0x18) >> 0x10);
    }
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    Srt_SetTranslationXYZ(*(int *)(self + 0x384) + 4, 0, 0x80, 0);
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    Snd_RegisterSeqAndBind(self + 0x388, *(int *)(*(int *)(self + 0x384) + 0x88), Ov107_PackTextureHandle(self, 1), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x384), self + 0x388);
    *(int *)(self + 0x3ac) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0x11));
    Srt_SetTranslationXYZ(*(int *)(self + 0x3ac) + 4, 0, 0x80, 0);
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x3ac));
    Snd_RegisterSeqAndBind(self + 0x3b0, *(int *)(*(int *)(self + 0x3ac) + 0x88), Ov107_PackTextureHandle(self, 0x12), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x3ac), self + 0x3b0);
    *(int *)(self + 0x42c) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov258_020d186c);
    *(int *)(self + 0x430) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov258_020d1878);
    *(int *)(self + 0x434) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov258_020d1884);
    *(int *)(self + 0x438) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov258_020d1890);
    *(int *)(self + 0x43c) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov258_020d18a0);
    *(int *)(self + 0x440) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov258_020d18ac);
    *(int *)(self + 0x444) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov258_020d18b8);
    *(int *)(self + 0x448) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov258_020d18c8);
    *(int *)(self + 0x44c) = InsertSortedEntryWithKey(*(int *)(self + 0x3ac), 1, data_ov258_020d18d4);
    *(int *)(self + 0x450) = InsertSortedEntryWithKey(*(int *)(self + 0x3ac), 1, data_ov258_020d18dc);
    for (i = 0; i < 0x2b; i++) {
        ((struct Ov258Parts *)self)->items[i].res = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ids.id[i]));
        Ov107_EnqueueValue(self, ((struct Ov258Parts *)self)->items[i].res);
        *(int *)(((struct Ov258Parts *)self)->items[i].res + 0x5c) |= 2;
    }
    for (i = 0; i < 2; i++) {
        ((struct Ov258Parts *)self)->hands[i] = Ov258_New(self, (u8)i);
    }
    place = *(Placement *)(self + 0x64);
    zero = data_02041dc8;
    place.pos = zero;
    cap.pos = zero;
    cap.axis = data_02042264;
    cap.length = 0xf000;
    for (i = 0; i < 8; i++) {
        switch (i) {
        case 0:
            cap.radius = 0x1e00;
            break;
        case 1:
        case 3:
            cap.radius = 0x1500;
            break;
        case 2:
        case 4:
            cap.radius = 0x1800;
            break;
        case 5:
        case 6:
            place.scale = 0x2a00;
            break;
        case 7:
            place.scale = 0x4200;
            break;
        }
        if (i == 0) {
            *(int *)(self + 0x3d4) = (int)List_InsertSorted(self + 0x22c, 0x10, 100);
        }
        slot = List_InsertSorted(self + 0x144, 4, 100);
        if (i == 0) {
            **(int **)(self + 0x3d4) = Ov107_Mover_New(&cap);
        }
        ((struct Ov258Parts *)self)->parts[i] = *slot = i < 5 ? Ov107_Mover_New(&cap) : Ov107_CloneResourceTransform(&place);
    }
    *(int *)(self + 0x460) = data_0204c240[0] & 4;
    Res_RequestIdPair(*(int *)(self + 0x460) != 0 ? 0x180 : 0x17b);
}
