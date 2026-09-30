/* ov149 actor initializer: install the callback table, seed the camera pose,
 * build the actor's primary subitem, resolve two bone attachments, create the
 * action resource and five node subitems, configure four actions, seed the two
 * pose handles and open the sub-object.
 *
 * Same template as Ov121_InitializeActor with ov149's numbers: ten callbacks
 * instead of eight, two bone attachments instead of four, five node subitems
 * instead of three, action strength 0x2800 instead of 0x1800, camera scale
 * 0x1000 instead of 0x800, and a trailing Ov149_Actor_New whose result is
 * parked at +0x3c8 before the 0x14e notification.
 *
 * CODEGEN NOTE inherited from the ov121 twin -- the `+ 0x14` store into +0x2cc is
 * a SCOPE problem, not a scheduling one. Written at function scope, mwcc reserves
 * r0/r1 for the trailing Ov107_PackTextureHandle(self, 1) argument setup and the
 * store sinks below the two movs. Putting the tail in its own block with its own
 * `int *self` declaration frees r0 and mwcc emits the ROM's
 * `add r0,r0,#0x14 ; str r0,[r6,#0x2cc]`. */

#include "nitro/fx_types.h"

struct Ov149Pose {
    VecFx32 position;
    int scale;
};

struct Ov149SubitemSlot {
    void *subitem;
    int pad;
};

struct Ov149Kinds {
    int values[5];
};

extern struct Ov149Kinds data_ov149_020d0738;
extern VecFx32 data_02041dc8;
extern const unsigned short data_ov149_020d07ac[];
extern const unsigned short data_ov149_020d07b0[];
extern char data_ov149_020d07b8[];

extern void Ov149_ReleaseSubObjectsAndListThenNotify(void);
extern void Ov149_TickAndSyncMarkerSrt(void);
extern void Ov149_Ov149HandleCommand(void);
extern void Ov149_CreateRegistryEntryAndLink(void);
extern void Ov149_ForwardEventToChild(void);
extern void Ov149_NotifyPartThenBase(void);
extern void Ov149_ReleaseByStateAndSyncSrt(void);
extern void Ov149_Ov149TickStaggerAndFlipFacing(void);
extern void Ov149_RequestSubState8IfIdle(void);
extern void Ov149_Model_SetTrack0(void);

extern void *Ov107_PackTextureHandle();
extern void *CreateSubitemInstance0xB4();
extern void RegisterSubscriberSlot();
extern void *InsertSortedEntryWithKey();
extern void *Ov107_CreateNamedResourceBinding();
extern void *CallocInstance();
extern void Ov107_EnqueueValue();
extern void Ov107_Actor_SetAttachSlot();
extern void *List_InsertSorted();
extern int Ov107_CloneResourceTransform();
extern void *Ov149_Actor_New();
extern void Res_RequestIdPair(int nId);

void Ov149_Ov149ActorInit(int param)
{
    struct Ov149Kinds kinds;
    struct Ov149Pose pose;
    int i;
    int resource;

    kinds = data_ov149_020d0738;

    *(void **)(param + 0x08) = Ov149_ReleaseSubObjectsAndListThenNotify;
    *(void **)(param + 0x0c) = Ov149_TickAndSyncMarkerSrt;
    *(void **)(param + 0x1c) = Ov149_Ov149HandleCommand;
    *(void **)(param + 0x30) = Ov149_CreateRegistryEntryAndLink;
    *(void **)(param + 0x28) = Ov149_ForwardEventToChild;
    *(void **)(param + 0x2c) = Ov149_NotifyPartThenBase;
    *(void **)(param + 0x34) = Ov149_ReleaseByStateAndSyncSrt;
    *(void **)(param + 0x1d0) = Ov149_Ov149TickStaggerAndFlipFacing;
    *(void **)(param + 0x1e0) = Ov149_RequestSubState8IfIdle;
    *(void **)(param + 0x1dc) = Ov149_Model_SetTrack0;

    *(int *)(param + 0x70) = 0x1000;
    *(int *)(param + 0x64) = 0;
    *(int *)(param + 0x68) = 0x1000;
    *(int *)(param + 0x6c) = 0;

    {
        int *self = (int *)param;

        ((void **)self)[0xe1] = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
        RegisterSubscriberSlot(self[0x27], ((void **)self)[0xe1]);
        ((void **)self)[0xe5] = InsertSortedEntryWithKey(self[0xe1], 3, data_ov149_020d07ac);
        ((void **)self)[0xe6] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov149_020d07b0);
        ((char **)self)[0xb3] = (char *)((void **)self)[0xe6] + 0x14;
        ((void **)self)[0xf3] = Ov107_CreateNamedResourceBinding(
            Ov107_PackTextureHandle(self, 1), data_ov149_020d07b8);
        ((void **)self)[0xe4] = CallocInstance(0x28);

        for (i = 0; i < 5; i++) {
            ((struct Ov149SubitemSlot *)((void **)self)[0xe4])[i].subitem =
                CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, kinds.values[i]));
            Ov107_EnqueueValue(self,
                ((struct Ov149SubitemSlot *)((void **)self)[0xe4])[i].subitem);
            *(int *)((char *)((struct Ov149SubitemSlot *)
                ((void **)self)[0xe4])[i].subitem + 0x5c) |= 2;
        }

        Ov107_Actor_SetAttachSlot(self, 0, 1, 0, 0x2800);
        Ov107_Actor_SetAttachSlot(self, 1, 1, 0, 0x2800);
        Ov107_Actor_SetAttachSlot(self, 2, 1, 0, 0x2800);
        Ov107_Actor_SetAttachSlot(self, 4, 1, 0, 0x2800);

        pose.position = data_02041dc8;
        pose.scale = 0x1000;

        ((void **)self)[0xe2] = List_InsertSorted(self + 0x8b, 0x10, 100);
        *((int **)self)[0xe2] = Ov107_CloneResourceTransform(&pose);
        {
            int *p = List_InsertSorted(self + 0x51, 4, 100);
            resource = Ov107_CloneResourceTransform(&pose);
            *p = resource;
            self[0xe3] = resource;
        }
        ((void **)self)[0xf2] = Ov149_Actor_New(self);
        Res_RequestIdPair(0x14e);
    }
}
