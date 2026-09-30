/* ov141 actor initializer: install the callback table, seed the camera pose,
 * build the actor's rig, resolve two bone attachments, create the action
 * resource and five node subitems, configure four actions, seed the two pose
 * handles and open the actor's own sub-object.
 *
 * The two bones resolved through InsertSortedEntryWithKey are "7" (kind 3) and "root"
 * (kind 1); each returns a 0x30-byte attachment record. Field 0x2cc keeps the
 * body of the "root" record (its +0x14 block).
 *
 * The tail lives in its own block with its own `int *self` declaration. That is
 * not cosmetic: at function scope mwcc reserves r0/r1 for the trailing
 * Ov107_PackTextureHandle(self, 1) argument setup, the +0x14 offset lands in r2 and
 * the store sinks below the two movs. The block frees r0 and the original's
 * `add r0,r0,#0x14 ; str r0,[r6,#0x2cc]` comes back.
 */

#include "nitro/fx_types.h"

struct Ov142Pose {
    VecFx32 position;
    int scale;
};

struct Ov142SubitemSlot {
    void *subitem;
    int pad;
};

struct Ov142KindTable {
    int kind[5];
};

extern struct Ov142KindTable data_ov142_020d25fc;
extern VecFx32 data_02041dc8;
extern const char data_ov142_020d268c[];
extern const char data_ov142_020d2690[];
extern char data_ov142_020d2698[];

extern void Ov142_ReleaseSubObjectsAndListThenNotify(void);
extern void Ov142_TickAndSyncMarkerSrt(void);
extern void Ov142_HandleCommand(void);
extern void Ov142_CreateRegistryEntryAndLink(void);
extern void Ov142_ForwardEventToChild(void);
extern void Ov142_NotifyPartThenBase(void);
extern void Ov142_ReleaseByStateAndSyncSrt(void);
extern void Ov142_ApplyHit(void);
extern void Ov142_RequestSubState8IfIdle(void);
extern void Ov142_Model_SetTrack0(void);

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
extern void *Ov142_AllocLinkChild3a0();
extern void Res_RequestIdPair(int nId);

void Ov142_InitializeActor(int param)
{
    struct Ov142KindTable kinds;
    struct Ov142Pose pose;
    int i;
    int resource;

    kinds = data_ov142_020d25fc;

    *(void **)(param + 0x08) = Ov142_ReleaseSubObjectsAndListThenNotify;
    *(void **)(param + 0x0c) = Ov142_TickAndSyncMarkerSrt;
    *(void **)(param + 0x1c) = Ov142_HandleCommand;
    *(void **)(param + 0x30) = Ov142_CreateRegistryEntryAndLink;
    *(void **)(param + 0x28) = Ov142_ForwardEventToChild;
    *(void **)(param + 0x2c) = Ov142_NotifyPartThenBase;
    *(void **)(param + 0x34) = Ov142_ReleaseByStateAndSyncSrt;
    *(void **)(param + 0x1d0) = Ov142_ApplyHit;
    *(void **)(param + 0x1e0) = Ov142_RequestSubState8IfIdle;
    *(void **)(param + 0x1dc) = Ov142_Model_SetTrack0;

    *(int *)(param + 0x70) = 0x800;
    *(int *)(param + 0x64) = 0;
    *(int *)(param + 0x68) = 0x800;
    *(int *)(param + 0x6c) = 0;

    {
        int *self = (int *)param;

        ((void **)self)[0xe1] = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
        RegisterSubscriberSlot(self[0x27], ((void **)self)[0xe1]);
        ((void **)self)[0xe5] = InsertSortedEntryWithKey(self[0xe1], 3, data_ov142_020d268c);
        ((void **)self)[0xe6] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov142_020d2690);
        ((char **)self)[0xb3] = (char *)((void **)self)[0xe6] + 0x14;
        ((void **)self)[0xf3] = Ov107_CreateNamedResourceBinding(
            Ov107_PackTextureHandle(self, 1), data_ov142_020d2698);
        ((void **)self)[0xe4] = CallocInstance(0x28);

        for (i = 0; i < 5; i++) {
            ((struct Ov142SubitemSlot *)((void **)self)[0xe4])[i].subitem =
                CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, kinds.kind[i]));
            Ov107_EnqueueValue(self,
                ((struct Ov142SubitemSlot *)((void **)self)[0xe4])[i].subitem);
            *(int *)((char *)((struct Ov142SubitemSlot *)
                ((void **)self)[0xe4])[i].subitem + 0x5c) |= 2;
        }

        Ov107_Actor_SetAttachSlot(self, 0, 1, 0, 0x1f33);
        Ov107_Actor_SetAttachSlot(self, 1, 1, 0, 0x1f33);
        Ov107_Actor_SetAttachSlot(self, 2, 1, 0, 0x1f33);
        Ov107_Actor_SetAttachSlot(self, 4, 1, 0, 0x1f33);

        pose.position = data_02041dc8;
        pose.scale = 0x800;

        ((void **)self)[0xe2] = List_InsertSorted(self + 0x8b, 0x10, 100);
        *((int **)self)[0xe2] = Ov107_CloneResourceTransform(&pose);
        {
            int *p = List_InsertSorted(self + 0x51, 4, 100);
            resource = Ov107_CloneResourceTransform(&pose);
            *p = resource;
            self[0xe3] = resource;
        }
        ((void **)self)[0xf2] = Ov142_AllocLinkChild3a0(self);
        Res_RequestIdPair(0x11e);
    }
}
