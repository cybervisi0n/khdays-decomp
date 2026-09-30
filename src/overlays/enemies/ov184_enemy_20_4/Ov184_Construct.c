/* Constructor of the ov181 enemy (x4: ov181/182/183/184): installs the handlers (+8 tick, +0xc
 * draw, +0x20/+0x1c message pair, +0x30 hit callback, +0x1e0 release, +0x1d0 on-hit, +0x1dc
 * finish), seeds the +0x64 pose (scale 0xe00, y 0xe00) and bit 4 of +0x1ae, builds the primary
 * item from pool entry 0 (its +4 placement lifted by 0x80, subscribed), keeps the "B_Move"
 * motion handle (+0x390), the three sub-items of kinds 2/3/4 in a fresh 24-byte slot table
 * (+0x398, attached, bit 1 on their +0x5c), configures action 2 (mode 2, rate 0x3000) and
 * creates two placements from the actor's +0x64 pose: +0x388 on the +0x22c list and +0x38c on
 * the +0x144 list; sound 0x131 is loaded. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Ov181SubitemSlot {
    void *subitem;
    int pad;
};

extern VecFx32 data_ov184_020d4458;
extern const char data_ov184_020d44ac[];

extern void Ov184_Destroy(void);
extern void Ov184_TickAndSyncChildren(void);
extern void Ov184_SendMessage28(void);
extern void Ov184_HandleMessage(void);
extern void Ov184_CreateRegistryEntryAndLink(void);
extern void Ov184_ReactionRequestSubState11(void);
extern void Ov184_OnHit(void);
extern void Ov184_Model_SetTrack0(void);

extern void *Ov107_PackTextureHandle();
extern void *CreateSubitemInstance0xB4();
extern void Srt_SetTranslationXYZ();
extern void RegisterSubscriberSlot();
extern void *Ov107_CreateNamedResourceBinding();
extern void *CallocInstance();
extern void Ov107_EnqueueValue();
extern void Ov107_Actor_SetAttachSlot();
extern void *List_InsertSorted();
extern int Ov107_CloneResourceTransform();
extern void Res_RequestIdPair(int nId);

void Ov184_Construct(int param)
{
    VecFx32 kinds;
    int i;

    kinds = data_ov184_020d4458;

    *(void **)(param + 0x08) = Ov184_Destroy;
    *(void **)(param + 0x0c) = Ov184_TickAndSyncChildren;
    *(void **)(param + 0x20) = Ov184_SendMessage28;
    *(void **)(param + 0x1c) = Ov184_HandleMessage;
    *(void **)(param + 0x30) = Ov184_CreateRegistryEntryAndLink;
    *(void **)(param + 0x1e0) = Ov184_ReactionRequestSubState11;
    *(void **)(param + 0x1d0) = Ov184_OnHit;
    *(void **)(param + 0x1dc) = Ov184_Model_SetTrack0;

    *(int *)(param + 0x70) = 0xe00;
    *(int *)(param + 0x64) = 0;
    *(int *)(param + 0x68) = 0xe00;
    *(int *)(param + 0x6c) = 0;
    *(u16 *)(param + 0x100 + 0xae) |= 0x10;

    {
        int *self = (int *)param;

        ((void **)self)[0xe1] = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
        Srt_SetTranslationXYZ((char *)((void **)self)[0xe1] + 4, 0, 0x80, 0);
        RegisterSubscriberSlot(self[0x27], ((void **)self)[0xe1]);
        ((void **)self)[0xe4] = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(self, 1), data_ov184_020d44ac);
        ((void **)self)[0xe6] = CallocInstance(0x18);

        for (i = 0; i < 3; i++) {
            ((struct Ov181SubitemSlot *)((void **)self)[0xe6])[i].subitem =
                CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ((int *)&kinds)[i]));
            Ov107_EnqueueValue(self,
                ((struct Ov181SubitemSlot *)((void **)self)[0xe6])[i].subitem);
            *(int *)((char *)((struct Ov181SubitemSlot *)
                ((void **)self)[0xe6])[i].subitem + 0x5c) |= 2;
        }

        Ov107_Actor_SetAttachSlot(self, 2, 2, 0, 0x3000);

        ((void **)self)[0xe2] = List_InsertSorted(self + 0x8b, 0x10, 100);
        *((int **)self)[0xe2] = Ov107_CloneResourceTransform(self + 0x19);
        {
            int *p = List_InsertSorted(self + 0x51, 4, 100);
            self[0xe3] = *p = Ov107_CloneResourceTransform(self + 0x19);
        }
        Res_RequestIdPair(0x131);
    }
}
