/* Constructor of the ov238 enemy. Installs the handlers (+8, +0xc, +0x1c message, +0x30, +0x28, +0x2c,
 * +0x34, +0x1d0 hit, +0x1dc), the +0x1fc bounds box, the +0x64 pose (scale 1.41) and bit 3 of +0x1ae;
 * builds the +0x388 rig from pose 0 (raised 0.03, owned, callback 020cfc08, subscribed to +0x9c) with
 * four bones (+0x3ec, +0x3f8, +0x3f0, +0x3f4), the +0x3e0 bone of pose 0x18 and the nine +0x404 slot
 * models (kinds of data_ov238_020d368c) attached and hidden; four distinct entries of the 12-entry
 * data_ov238_020d3674 table are drawn at random into +0x3fc; a placement at the origin with the pose
 * scale fills the +0x38c / +0x3dc handles, the +0x384 helper is created (020d2640) with flag 2 on its
 * body, and sound 0x12e loads. */

#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { short v[12]; } Order12;
typedef struct { int id[9]; } IdTable9;
typedef struct { VecFx32 min; VecFx32 max; } Bounds;
typedef struct { VecFx32 pos; int scale; } Placement;
struct Pair { int res; int handle; };
struct Ov238Parts { char pad[0x404]; struct Pair items[9]; };

extern void Ov238_Actor_Destroy(void);
extern void Ov238_TickWithChildRefresh(void);
extern void Ov238_OnSpawnMessage(void);
extern void Ov238_CreateNodeRegistryEntry(void);
extern void Ov238_ForwardRegionEventToPart(void);
extern void Ov238_NotifyPartThenBase(void);
extern void Ov238_CarrierTeardown(void);
extern void Ov238_PartnerOnDamage(void);
extern void Ov238_RebuildAnim(void);
extern void Ov238_ModelAnimTick(void);
extern void *Ov107_PackTextureHandle(char *self, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern void Srt_SetTranslationXYZ(void *srt, int x, int y, int z);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern int InsertSortedEntryWithKey(int item, int kind, const char *name);
extern int Ov107_CreateNamedResourceBinding(void *item, const char *name);
extern void Ov107_EnqueueValue(char *self, int item);
extern int RandNextScaled(int bound);
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern int Ov107_CloneResourceTransform(const Placement *placement);
extern int Ov238_Partner_New(char *self);
extern void Res_RequestIdPair(int resourceId);
extern const Order12 data_ov238_020d3674;
extern const IdTable9 data_ov238_020d368c;
extern const char data_ov238_020d370c[];
extern const char data_ov238_020d3718[];
extern const char data_ov238_020d3724[];
extern const char data_ov238_020d3730[];
extern const VecFx32 data_02041dc8;

void Ov238_Construct(char *self)
{
    Bounds bounds;
    Placement place;
    Order12 order = data_ov238_020d3674;
    IdTable9 ids = data_ov238_020d368c;
    int i;
    int count;
    int *slot;
    int node;

    bounds.min.x = -0xeea;
    bounds.min.y = 0x17;
    bounds.min.z = -0x858;
    bounds.max.x = bounds.min.x + 0x1dd3;
    bounds.max.y = bounds.min.y + 0x1c27;
    bounds.max.z = bounds.min.z + 0xd64;
    *(Callback *)(self + 0x8) = Ov238_Actor_Destroy;
    *(Callback *)(self + 0xc) = Ov238_TickWithChildRefresh;
    *(Callback *)(self + 0x1c) = Ov238_OnSpawnMessage;
    *(Callback *)(self + 0x30) = Ov238_CreateNodeRegistryEntry;
    *(Callback *)(self + 0x28) = Ov238_ForwardRegionEventToPart;
    *(Callback *)(self + 0x2c) = Ov238_NotifyPartThenBase;
    *(Callback *)(self + 0x34) = Ov238_CarrierTeardown;
    *(Callback *)(self + 0x1d0) = Ov238_PartnerOnDamage;
    *(Callback *)(self + 0x1dc) = Ov238_RebuildAnim;
    *(Bounds *)(self + 0x1fc) = bounds;
    *(int *)(self + 0x70) = 0x1680;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x1680;
    *(int *)(self + 0x6c) = 0;
    *(unsigned short *)(self + 0x100 + 0xae) |= 8;
    *(int *)(self + 0x388) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    Srt_SetTranslationXYZ((void *)(*(int *)(self + 0x388) + 4), 0, 0x80, 0);
    *(char **)(*(int *)(self + 0x388) + 0x84) = self;
    *(Callback *)(*(int *)(self + 0x388) + 0x68) = Ov238_ModelAnimTick;
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x388));
    *(int *)(self + 0x3ec) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 1, data_ov238_020d370c);
    *(int *)(self + 0x3f8) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 3, data_ov238_020d370c);
    *(int *)(self + 0x3f0) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 1, data_ov238_020d3718);
    *(int *)(self + 0x3f4) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 1, data_ov238_020d3724);
    place = *(Placement *)(self + 0x64);
    place.pos = data_02041dc8;
    *(int *)(self + 0x3e0) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(self, 0x18), data_ov238_020d3730);
    for (i = 0; i < 9; i++) {
        ((struct Ov238Parts *)self)->items[i].res = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ids.id[i]));
        Ov107_EnqueueValue(self, ((struct Ov238Parts *)self)->items[i].res);
        *(int *)(((struct Ov238Parts *)self)->items[i].res + 0x5c) |= 2;
    }
    count = 0;
    do {
        short k = RandNextScaled(0xc);

        if (order.v[k] != -1) {
            ((short *)(self + 0x3fc))[count] = order.v[k];
            order.v[k] = -1;
            count++;
        }
    } while (count < 4);
    *(int **)(self + 0x38c) = List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x38c) = Ov107_CloneResourceTransform(&place);
    slot = List_InsertSorted(self + 0x144, 4, 100);
    node = Ov107_CloneResourceTransform(&place);
    *(int *)(self + 0x3dc) = *slot = node;
    *(int *)(self + 0x384) = Ov238_Partner_New(self);
    *(int *)(*(int *)(*(int *)(self + 0x384) + 0x9c) + 0x5c) |= 4;
    Res_RequestIdPair(0x12e);
}
