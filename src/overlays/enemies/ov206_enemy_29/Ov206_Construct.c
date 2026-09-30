/* Constructor of the ov206 enemy (and its byte-identical twins): installs the handlers (+8
 * 020d0080, +0xc draw 020d00d8, +0x1c message 020d0330, +0x30 020d0ce8, +0x2c 020d0444, +0x34
 * tick 020d05f4, +0x1d0 on-hit 020d0d34, +0x1e0 release 020d105c, +0x1dc finish 020d0508),
 * seeds the +0x64 pose (scale 0x1d00, y 0x1d00) and bit 3 of +0x1ae, builds the primary item
 * from pool entry 0 (subscribed), attaches pool entry 1's animation to its +0x88 track through
 * the +0x388 block (0202a388 / b9ac, finalised), resolves six named joints (+0x3c8 / +0x3cc /
 * +0x3d0 / +0x3dc mode 1, +0x3d4 / +0x3d8 mode 3), the five sub-items of the
 * data_ov206_020d0550 kinds in a fresh 40-byte slot table (+0x3e0, attached, bit 1 on their
 * +0x5c), configures actions 0/2/1/4 (modes 2/3/2/2, the second with a (0, -0x1d00, 0) offset,
 * rates 0x1000 / 0x99a / 0xccd / 0x1000), keeps pool entry 0x14's data_ov206_020d06bc motion
 * handle (+0x3b4), and creates from the zero position: a 0.5 placement on the +0x22c list
 * (+0x3ac), a capsule (world Y axis, radius 1.0, height 0x1333) on the +0x22c list with 110
 * slots (+0x3b0), a capsule of height 0x1050 on the +0x144 list (+0x3b8) and placements of
 * scale 0xccc there (three) (+0x3bc / +0x3c0 / +0x3c4); sound 0x116 is loaded. */

#include "nitro/fx_types.h"

struct Ov206Pose {
    VecFx32 position;
    int scale;
};

struct Ov206Capsule {
    VecFx32 vPos;
    VecFx32 vUp;
    int nRadius;
    int nHeight;
};

struct Ov206Kinds { int w[5]; };

struct Ov206SubitemSlot {
    void *subitem;
    int pad;
};

extern struct Ov206Kinds data_ov206_020d0550;
extern VecFx32 data_02041dc8;
extern VecFx32 data_02042264;
extern const char data_ov206_020d066c[];
extern const char data_ov206_020d0678[];
extern const char data_ov206_020d0688[];
extern const char data_ov206_020d0698[];
extern const char data_ov206_020d06a0[];
extern const char data_ov206_020d06b0[];
extern const char data_ov206_020d06bc[];

extern void Ov206_Destroy(void);
extern void Ov206_DrawHandler(void);
extern void Ov206_HandleResourceMessage(void);
extern void Ov206_CreateRegistryEntryAndLink(void);
extern void Ov206_FinishPartTasksThenBase(void);
extern void Ov206_TickHandler(void);
extern void Ov206_HandleHit(void);
extern void Ov206_RequestSubState12IfIdleIn2Or4(void);
extern void Ov206_MessageArmEmitterTable(void);

extern void *Ov107_PackTextureHandle();
extern void *CreateSubitemInstance0xB4();
extern void RegisterSubscriberSlot();
extern void Snd_RegisterSeqAndBind();
extern void MainBlob_ResetSlotRows();
extern void RefreshObjectCallbacks();
extern char *InsertSortedEntryWithKey();
extern void *Ov107_CreateNamedResourceBinding();
extern void *CallocInstance();
extern void Ov107_EnqueueValue();
extern void Ov107_Actor_SetAttachSlot();
extern void *List_InsertSorted();
extern int Ov107_CloneResourceTransform();
extern int Ov107_Mover_New(struct Ov206Capsule *req);
extern void Res_RequestIdPair(int nId);

void Ov206_Construct(int param)
{
    struct Ov206Kinds kinds;
    struct Ov206Pose pose;
    struct Ov206Capsule capsule;
    VecFx32 offset;
    VecFx32 base;
    VecFx32 axis;
    int i;

    kinds = data_ov206_020d0550;

    *(void **)(param + 0x08) = Ov206_Destroy;
    *(void **)(param + 0x0c) = Ov206_DrawHandler;
    *(void **)(param + 0x1c) = Ov206_HandleResourceMessage;
    *(void **)(param + 0x30) = Ov206_CreateRegistryEntryAndLink;
    *(void **)(param + 0x2c) = Ov206_FinishPartTasksThenBase;
    *(void **)(param + 0x34) = Ov206_TickHandler;
    *(void **)(param + 0x1d0) = Ov206_HandleHit;
    *(void **)(param + 0x1e0) = Ov206_RequestSubState12IfIdleIn2Or4;
    *(void **)(param + 0x1dc) = Ov206_MessageArmEmitterTable;

    *(int *)(param + 0x70) = 0x1d00;
    *(int *)(param + 0x64) = 0;
    *(int *)(param + 0x68) = 0x1d00;
    *(int *)(param + 0x6c) = 0;
    *(unsigned short *)(param + 0x100 + 0xae) |= 8;

    {
        int *self = (int *)param;

        ((void **)self)[0xe1] = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
        RegisterSubscriberSlot(self[0x27], ((void **)self)[0xe1]);
        Snd_RegisterSeqAndBind(self + 0xe2, *(int *)(self[0xe1] + 0x88), Ov107_PackTextureHandle(self, 1), 0xc);
        MainBlob_ResetSlotRows(self[0xe1], self + 0xe2);
        RefreshObjectCallbacks(self[0xe1], 0);
        ((void **)self)[0xf2] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov206_020d066c);
        ((void **)self)[0xf3] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov206_020d0678);
        ((void **)self)[0xf4] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov206_020d0688);
        ((void **)self)[0xf5] = InsertSortedEntryWithKey(self[0xe1], 3, data_ov206_020d0698);
        ((void **)self)[0xf7] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov206_020d06a0);
        ((void **)self)[0xf6] = InsertSortedEntryWithKey(self[0xe1], 3, data_ov206_020d06b0);
        ((void **)self)[0xf8] = CallocInstance(0x28);

        for (i = 0; i < 5; i++) {
            ((struct Ov206SubitemSlot *)((void **)self)[0xf8])[i].subitem =
                CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, kinds.w[i]));
            Ov107_EnqueueValue(self,
                ((struct Ov206SubitemSlot *)((void **)self)[0xf8])[i].subitem);
            *(int *)((char *)((struct Ov206SubitemSlot *)
                ((void **)self)[0xf8])[i].subitem + 0x5c) |= 2;
        }

        offset.x = 0;
        offset.y = -0x1d00;
        offset.z = 0;
        Ov107_Actor_SetAttachSlot(self, 0, 2, 0, 0x1000);
        Ov107_Actor_SetAttachSlot(self, 2, 3, &offset, 0x99a);
        Ov107_Actor_SetAttachSlot(self, 1, 2, 0, 0xccd);
        Ov107_Actor_SetAttachSlot(self, 4, 2, 0, 0x1000);

        ((void **)self)[0xed] = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(self, 0x14), data_ov206_020d06bc);

        base = data_02041dc8;
        pose.position = base;
        pose.scale = 0x800;
        ((void **)self)[0xeb] = List_InsertSorted(self + 0x8b, 0x10, 100);
        *((int **)self)[0xeb] = Ov107_CloneResourceTransform(&pose);

        capsule.vPos = base;
        axis = data_02042264;
        capsule.vUp = axis;
        capsule.nRadius = 0x1000;
        capsule.nHeight = 0x1333;
        ((void **)self)[0xec] = List_InsertSorted(self + 0x8b, 0x10, 110);
        *((int **)self)[0xec] = Ov107_Mover_New(&capsule);

        capsule.vPos = base;
        capsule.vUp = axis;
        capsule.nRadius = 0x1000;
        capsule.nHeight = 0x1050;
        {
            int *p = List_InsertSorted(self + 0x51, 4, 100);
            self[0xee] = *p = Ov107_Mover_New(&capsule);
        }
        pose.position = base;
        pose.scale = 0xccc;
        {
            int *p = List_InsertSorted(self + 0x51, 4, 100);
            self[0xef] = *p = Ov107_CloneResourceTransform(&pose);
        }
        pose.position = base;
        pose.scale = 0xccc;
        {
            int *p = List_InsertSorted(self + 0x51, 4, 100);
            self[0xf0] = *p = Ov107_CloneResourceTransform(&pose);
        }
        pose.position = base;
        pose.scale = 0xccc;
        {
            int *p = List_InsertSorted(self + 0x51, 4, 100);
            self[0xf1] = *p = Ov107_CloneResourceTransform(&pose);
        }
        Res_RequestIdPair(0x116);
    }
}
