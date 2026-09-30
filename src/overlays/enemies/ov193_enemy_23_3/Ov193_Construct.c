/* Big constructor of the ov193 enemy (x3: ov193/192/193): raises bit 8 of the +0 flags, installs
 * the eleven handlers (+8 tick, +0xc/+0x10 draw pair, +0x1c message, +0x28/+0x2c/+0x30/+0x34
 * hit callbacks, +0x1d0 on-hit, +0x1dc finish, +0x1e0 release), seeds the +0x64 pose
 * (scale 0xc00, y 0xc00) and bit 4 of the +0x1ae flags, builds the primary item from pool entry
 * 0, resolves the "head02" and "headcon" bones (+0x398/+0x39c), the four sub-items of kinds
 * 1/2/3/6 (+0x3a0, attached, bit 1 on their +0x5c), configures actions 0/1/2/4 with the (0, 0x400,
 * 0) offset and the 0xd99/0x6cc/0x2ecc/0x2ecc rates, then creates four placements: two on the
 * +0x144 list (+0x394 at the origin, scale 0xc00; +0x390 at y 0xc00, scale 0xc00) and two on the
 * +0x22c list (+0x38c at the origin, scale 0xa00; +0x388 at y 0x800, scale 0x800, bit 1 on its
 * +8 flags), fills the +0x3a4 table with four Ov193_Actor_New records and loads sound 0x133. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Ov193Pose {
    VecFx32 position;
    int scale;
};

struct Ov193Kinds {
    int a;
    int b;
    int c;
    int d;
};

struct Ov193SubitemSlot {
    void *subitem;
    int pad;
};

struct bf {
    unsigned b : 8;
};

extern struct Ov193Kinds data_ov193_020d69a0;
extern VecFx32 data_ov193_020d6994;
extern const VecFx32 data_02041dc8;
extern const char data_ov193_020d6a2c[];
extern const char data_ov193_020d6a34[];

extern void Ov193_DestroyArrayObjectsAndBuffersThenNotify(void);
extern void func_ov193_020d3e60(void);
extern void Ov193_RefreshPose(void);
extern void Ov193_HandleSpawnMessage(void);
extern void Ov193_ForwardRegionEventToParts(void);
extern void Ov193_NotifyPartsThenBase(void);
extern void Ov193_CreateRegistryEntryAndLink(void);
extern void Ov193_Tick(void);
extern void Ov193_OnHit(void);
extern void Ov193_Model_SetTracks0And3(void);
extern void Ov193_TryBeginSubState7(void);
extern void *Ov193_Actor_New(int *self);

extern void *Ov107_PackTextureHandle();
extern void *CreateSubitemInstance0xB4();
extern void RegisterSubscriberSlot();
extern void *InsertSortedEntryWithKey();
extern void *CallocInstance();
extern void Ov107_EnqueueValue();
extern void Ov107_Actor_SetAttachSlot();
extern void *List_InsertSorted();
extern int Ov107_CloneResourceTransform();
extern void Res_RequestIdPair(int nId);

void Ov193_Construct(int param)
{
    struct Ov193Kinds kinds;
    struct Ov193Pose pose;
    VecFx32 offset;
    int i;
    int resource;

    kinds = data_ov193_020d69a0;
    offset = data_ov193_020d6994;

    *(u16 *)param |= 0x100;
    *(void **)(param + 0x08) = Ov193_DestroyArrayObjectsAndBuffersThenNotify;
    *(void **)(param + 0x1c) = Ov193_HandleSpawnMessage;
    *(void **)(param + 0x30) = Ov193_CreateRegistryEntryAndLink;
    *(void **)(param + 0x28) = Ov193_ForwardRegionEventToParts;
    *(void **)(param + 0x2c) = Ov193_NotifyPartsThenBase;
    *(void **)(param + 0x0c) = func_ov193_020d3e60;
    *(void **)(param + 0x10) = Ov193_RefreshPose;
    *(void **)(param + 0x34) = Ov193_Tick;
    *(void **)(param + 0x1e0) = Ov193_TryBeginSubState7;
    *(void **)(param + 0x1d0) = Ov193_OnHit;
    *(void **)(param + 0x1dc) = Ov193_Model_SetTracks0And3;

    *(int *)(param + 0x70) = 0xc00;
    *(int *)(param + 0x64) = 0;
    *(int *)(param + 0x68) = 0xc00;
    *(int *)(param + 0x6c) = 0;
    *(u16 *)(param + 0x100 + 0xae) |= 0x10;

    {
        int *self = (int *)param;

        ((void **)self)[0xe1] = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
        RegisterSubscriberSlot(self[0x27], ((void **)self)[0xe1]);
        ((void **)self)[0xe6] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov193_020d6a2c);
        ((void **)self)[0xe7] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov193_020d6a34);
        ((void **)self)[0xe8] = CallocInstance(0x20);

        for (i = 0; i < 4; i++) {
            ((struct Ov193SubitemSlot *)((void **)self)[0xe8])[i].subitem =
                CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ((int *)&kinds)[i]));
            Ov107_EnqueueValue(self,
                ((struct Ov193SubitemSlot *)((void **)self)[0xe8])[i].subitem);
            *(int *)((char *)((struct Ov193SubitemSlot *)
                ((void **)self)[0xe8])[i].subitem + 0x5c) |= 2;
        }

        Ov107_Actor_SetAttachSlot(self, 0, 1, &offset, 0x5d99);
        Ov107_Actor_SetAttachSlot(self, 1, 1, &offset, 0x2ecc);
        Ov107_Actor_SetAttachSlot(self, 2, 1, &offset, 0x2ecc);
        Ov107_Actor_SetAttachSlot(self, 4, 1, &offset, 0x2ecc);

        pose.position = data_02041dc8;
        pose.scale = 0xc00;
        {
            int *p = List_InsertSorted(self + 0x51, 4, 100);
            resource = Ov107_CloneResourceTransform(&pose);
            *p = resource;
            self[0xe5] = resource;
        }

        pose.position = data_02041dc8;
        pose.scale = 0xa00;
        {
            int **placementSlot = &((int **)self)[0xe3];
            int *placement;
            placement = List_InsertSorted(self + 0x8b, 0x10, 100);
            *placementSlot = placement;
            **placementSlot = Ov107_CloneResourceTransform(&pose);
        }

        pose.scale = 0xc00;
        pose.position.x = 0;
        pose.position.y = 0xc00;
        pose.position.z = 0;
        {
            int *p = List_InsertSorted(self + 0x51, 4, 100);
            resource = Ov107_CloneResourceTransform(&pose);
            *p = resource;
            self[0xe4] = resource;
        }

        pose.scale = 0x800;
        pose.position.x = 0;
        pose.position.y = 0x800;
        pose.position.z = 0;
        ((void **)self)[0xe2] = List_InsertSorted(self + 0x8b, 0x10, 100);
        *((int **)self)[0xe2] = Ov107_CloneResourceTransform(&pose);
        ((struct bf *)(((char **)self)[0xe2] + 8))->b |= 2;

        ((void **)self)[0xe9] = CallocInstance(0x10);
        for (i = 0; i < 4; i++) {
            ((void **)((void **)self)[0xe9])[i] = Ov193_Actor_New(self);
        }
        Res_RequestIdPair(0x133);
    }
}
