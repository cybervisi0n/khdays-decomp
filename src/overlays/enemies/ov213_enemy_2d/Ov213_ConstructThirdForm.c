/* Constructor of the ov213 actor's third form: installs the handlers (+8 tick, +0xc draw,
 * +0x20 hook, +0x1c message, +0x30 callback, +0x1d0 position broadcast), raises bits 2-3 of
 * +0x1ae and flags 0x44 in the +0x60 high byte, queues pose 2 at +0x1c9, zeroes the +0x64
 * velocity, sets the +0x70 scale to 0.5 and bit 2 on the subscriber's +0x5c. The collision item
 * comes from pool entry 0x42 (subscribed, its four channels bound with (0, 1)), the two sub-items
 * from the data_ov213_020d2f3c entries into a fresh 16-byte slot table (+0x394, attached, bit 1
 * on their +0x5c), and a shape on the +0x144 list (+0x38c) is built from the +0x64 velocity. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);

struct Ov213SubitemSlot {
    int pItem;
    int pad4;
};

extern void Ov213_ThirdForm_Destroy(void);
extern void Ov213_TickSyncXformFirst(void);
extern void Ov213_SendMessage24(void);
extern void Ov213_CmdSpawnChildTwoSlots(void);
extern void Ov213_ThirdForm_CreateAiTask(void);
extern void Ov213_BroadcastPosition(void);
extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void SetSubitemState(int item, int channel, int a, int b);
extern void *CallocInstance(int size);
extern void Ov107_EnqueueValue(char *self, int item);
extern int *List_InsertSorted(char *list, int stride, int max);
extern int Ov107_CloneResourceTransform(char *pose);
extern const int data_ov213_020d2f3c[2];
extern const VecFx32 data_02041dc8;

void Ov213_ConstructThirdForm(char *self)
{
    int kinds[2];
    u16 hw;
    int i;
    int *slot;
    kinds[0] = data_ov213_020d2f3c[0];
    kinds[1] = data_ov213_020d2f3c[1];
    *(Callback *)(self + 0x8) = Ov213_ThirdForm_Destroy;
    *(Callback *)(self + 0xc) = Ov213_TickSyncXformFirst;
    *(Callback *)(self + 0x20) = Ov213_SendMessage24;
    *(Callback *)(self + 0x1c) = Ov213_CmdSpawnChildTwoSlots;
    *(Callback *)(self + 0x30) = Ov213_ThirdForm_CreateAiTask;
    *(Callback *)(self + 0x1d0) = Ov213_BroadcastPosition;
    *(u16 *)(self + 0x100 + 0xae) |= 0xc;
    hw = *(u16 *)(self + 0x60);
    *(u16 *)(self + 0x60) = (hw & ~0xff00) |
        ((((((unsigned int)hw << 0x10) >> 0x18) | 0x44) << 0x18) >> 0x10);
    *(unsigned char *)(self + 0x1c9) = 2;
    *(VecFx32 *)(self + 0x64) = data_02041dc8;
    *(int *)(self + 0x70) = 0x800;
    *(int *)(*(int *)(self + 0x9c) + 0x5c) |= 4;
    *(int *)(self + 0x388) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(*(int *)(self + 0x384), 0x42));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x388));
    SetSubitemState(*(int *)(self + 0x388), 0, 0, 1);
    SetSubitemState(*(int *)(self + 0x388), 2, 0, 1);
    SetSubitemState(*(int *)(self + 0x388), 4, 0, 1);
    SetSubitemState(*(int *)(self + 0x388), 1, 0, 1);
    *(void **)(self + 0x394) = CallocInstance(0x10);
    for (i = 0; i < 2; i++) {
        (*(struct Ov213SubitemSlot **)(self + 0x394))[i].pItem =
            CreateSubitemInstance0xB4(Ov107_PackTextureHandle(*(int *)(self + 0x384), kinds[i]));
        Ov107_EnqueueValue(self, (*(struct Ov213SubitemSlot **)(self + 0x394))[i].pItem);
        *(int *)((*(struct Ov213SubitemSlot **)(self + 0x394))[i].pItem + 0x5c) |= 2;
    }
    slot = List_InsertSorted(self + 0x144, 4, 0x64);
    *(int *)(self + 0x38c) = *slot = Ov107_CloneResourceTransform(self + 0x64);
}
