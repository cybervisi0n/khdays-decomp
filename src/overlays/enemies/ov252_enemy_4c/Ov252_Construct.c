/* Constructor of the ov252 boss: installs the handlers (+8 tick, +0xc draw, +0x20 / +0x1c / +0x30
 * callbacks, +0x34 part placement 020cd044, +0x1d0 hit, +0x1dc finish), the +0x1fc bounds
 * (data_ov252_020d4258), kind byte 2 at +0x1c9, scale 11.0 at +0x70, the 0xe0 bits of the +0x60 high
 * byte and bits 3-4 of +0x1ae. Builds the body model from pool entry 0 (+0x384, owner at +0x84,
 * callback 020cbfc8 at +0x68, subscribed) with ten named joints, clears the four +0x39c armour
 * states, builds the wing model (entry 0x3a, motion 0x3b on the +0x430 track, joints sw_L02 / sw_R02)
 * and the four tail models (entries 0x6a / 0x99 / 0xc8 / 0xf7 with the next entry as motion on the
 * +0x454 tracks, joints B3_03tail01..04), the B3move motion (+0x574), the 49 hidden parts at +0x63c
 * (poses data_ov252_020d4270), and nine hit volumes: two capsules (radius 10.0, then 9.0 twice), four
 * sphere pairs (8.0 / 4.0) and two boxes (13.0 / 8.0 by 15.0), each on the +0x22c (+0x4e8) and +0x144
 * (+0x50c) lists. Finally arms the +0x504 / +0x508 volumes, shows the four armour pieces (020cc134)
 * and loads sound 0x148. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { int min[3]; int max[3]; } Bounds;
typedef struct { int id[49]; } PartPoses;
typedef struct { VecFx32 pos; int radius; } Sphere;
typedef struct { VecFx32 pos; VecFx32 axis; int w; int radius; } Capsule;
typedef struct { VecFx32 pos; VecFx32 a; VecFx32 b; VecFx32 c; int w0; int w1; int w2; } Box;
struct w8 { unsigned int lo : 8, rest : 24; };
struct Ov252Track { char pad[0x88]; int track; };
struct Ov252Part { int item; int pad; };

extern void Ov252_Actor_Destroy(void);
extern void Ov252_TickWithChildRefresh(void);
extern void Ov252_ReportArmour(void);
extern void Ov252_HandleMessage(void);
extern void Ov252_CreateAiTask(void);
extern void Ov252_PlaceBodyParts(void);
extern void Ov252_HitFilter(void);
extern void Ov252_RebuildArmour(void);
extern void Ov252_ModelAnimTick(void);
extern void Ov252_ShowArmourPiece(char *self, int i, int show);
extern void *Ov107_PackTextureHandle(char *self, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void Snd_RegisterSeqAndBind(void *track, int model, void *resource, int slot);
extern void MainBlob_ResetSlotRows(int item, void *track);
extern int InsertSortedEntryWithKey(int item, int kind, const char *name);
extern void Ov107_EnqueueValue(char *self, int item);
extern int Ov107_CreateNamedResourceBinding(void *item, const char *name);
extern int *List_InsertSorted(void *list, int stride, int max);
extern int Ov107_CloneResourceTransform(Sphere *sphere);
extern int Ov107_Mover_New(Capsule *capsule);
extern int Ov107_HitShape_NewBox(Box *box);
extern void Res_RequestIdPair(int id);
extern const Bounds data_ov252_020d4258;
extern const PartPoses data_ov252_020d4270;
extern const char data_ov252_020d452c[];
extern const char data_ov252_020d4538[];
extern const char data_ov252_020d4544[];
extern const char data_ov252_020d454c[];
extern const char data_ov252_020d4558[];
extern const char data_ov252_020d4564[];
extern const char data_ov252_020d4570[];
extern const char data_ov252_020d457c[];
extern const char data_ov252_020d4588[];
extern const char data_ov252_020d4590[];
extern const char data_ov252_020d459c[];
extern const char data_ov252_020d45a8[];
extern const char data_ov252_020d45b4[];
extern const char data_ov252_020d45c0[];
extern const char data_ov252_020d45cc[];
extern const char data_ov252_020d45d8[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042264;
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042258;

void Ov252_Construct(char *self)
{
    PartPoses ids = data_ov252_020d4270;
    Sphere spheres[2];
    Capsule cap;
    Box box;
    char *track;
    int *slot;
    int bCapsule;
    int i;

    *(Callback *)(self + 0x8) = Ov252_Actor_Destroy;
    *(Callback *)(self + 0xc) = Ov252_TickWithChildRefresh;
    *(Callback *)(self + 0x20) = Ov252_ReportArmour;
    *(Callback *)(self + 0x1c) = Ov252_HandleMessage;
    *(Callback *)(self + 0x30) = Ov252_CreateAiTask;
    *(Callback *)(self + 0x34) = Ov252_PlaceBodyParts;
    *(Callback *)(self + 0x1d0) = Ov252_HitFilter;
    *(Callback *)(self + 0x1dc) = Ov252_RebuildArmour;
    *(Bounds *)(self + 0x1fc) = data_ov252_020d4258;
    *(u8 *)(self + 0x1c9) = 2;
    *(int *)(self + 0x70) = 0xb000;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0;
    *(int *)(self + 0x6c) = 0;
    {
        u16 hw = *(u16 *)(self + 0x60);
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0xe0) << 0x18) >> 0x10);
    }
    *(u16 *)(self + 0x100 + 0xae) |= 0x18;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    *(char **)(*(int *)(self + 0x384) + 0x84) = self;
    *(Callback *)(*(int *)(self + 0x384) + 0x68) = Ov252_ModelAnimTick;
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    *(int *)(self + 0x530) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov252_020d452c);
    *(int *)(self + 0x534) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov252_020d4538);
    *(int *)(self + 0x554) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov252_020d4544);
    *(int *)(self + 0x55c) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov252_020d454c);
    *(int *)(self + 0x558) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov252_020d454c);
    *(int *)(self + 0x560) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov252_020d4558);
    *(int *)(self + 0x564) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov252_020d4564);
    *(int *)(self + 0x568) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov252_020d4570);
    *(int *)(self + 0x538) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov252_020d457c);
    *(int *)(self + 0x570) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov252_020d4588);
    for (i = 0; i < 4; i++) {
        *(signed char *)(self + i + 0x39c) = -1;
    }
    *(int *)(self + 0x388) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0x3a));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x388));
    Snd_RegisterSeqAndBind((void *)(self + 0x430), ((struct Ov252Track *)*(int *)(self + 0x388))->track,
                  Ov107_PackTextureHandle(self, 0x3b), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x388), (void *)(self + 0x430));
    *(int *)(self + 0x54c) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 3, data_ov252_020d4590);
    *(int *)(self + 0x550) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 3, data_ov252_020d459c);
    *(u16 *)(self + 0x57a) = 0;
    *(u16 *)(self + 0x57c) = 0;
    *(u16 *)(self + 0x57e) = 0;
    track = self + 0x54 + 0x400;
    for (i = 0; i < 4; i++) {
        switch (i) {
        case 0:
            ((int *)(self + 0x38c))[i] = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0x6a));
            Snd_RegisterSeqAndBind(track, ((struct Ov252Track *)((int *)(self + 0x38c))[i])->track, Ov107_PackTextureHandle(self, 0x6b), 0xc);
            *(int *)(self + 0x53c) = InsertSortedEntryWithKey(((int *)(self + 0x38c))[i], 3, data_ov252_020d45a8);
            break;
        case 1:
            ((int *)(self + 0x38c))[i] = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0x99));
            Snd_RegisterSeqAndBind(track, ((struct Ov252Track *)((int *)(self + 0x38c))[i])->track, Ov107_PackTextureHandle(self, 0x9a), 0xc);
            *(int *)(self + 0x540) = InsertSortedEntryWithKey(((int *)(self + 0x38c))[i], 3, data_ov252_020d45b4);
            break;
        case 2:
            ((int *)(self + 0x38c))[i] = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0xc8));
            Snd_RegisterSeqAndBind(track, ((struct Ov252Track *)((int *)(self + 0x38c))[i])->track, Ov107_PackTextureHandle(self, 0xc9), 0xc);
            *(int *)(self + 0x544) = InsertSortedEntryWithKey(((int *)(self + 0x38c))[i], 3, data_ov252_020d45c0);
            break;
        case 3:
            ((int *)(self + 0x38c))[i] = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0xf7));
            Snd_RegisterSeqAndBind(track, ((struct Ov252Track *)((int *)(self + 0x38c))[i])->track, Ov107_PackTextureHandle(self, 0xf8), 0xc);
            *(int *)(self + 0x548) = InsertSortedEntryWithKey(((int *)(self + 0x38c))[i], 3, data_ov252_020d45cc);
            break;
        }
        RegisterSubscriberSlot(*(int *)(self + 0x9c), ((int *)(self + 0x38c))[i]);
        MainBlob_ResetSlotRows(((int *)(self + 0x38c))[i], track);
        track += 0x24;
    }
    *(int *)(self + 0x574) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(self, 0x126), data_ov252_020d45d8);
    for (i = 0; i < 49; i++) {
        ((struct Ov252Part *)(self + 0x63c))[i].item = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ids.id[i]));
        Ov107_EnqueueValue(self, ((struct Ov252Part *)(self + 0x63c))[i].item);
        *(int *)(((struct Ov252Part *)(self + 0x63c))[i].item + 0x5c) |= 2;
    }
    for (i = 0; i < 2; i++) {
        spheres[i].pos = data_02041dc8;
        spheres[i].radius = 0x1000;
    }
    cap.pos = data_02041dc8;
    cap.axis = data_02042264;
    cap.w = 0x1000;
    for (i = 0; i < 9; i++) {
        bCapsule = 0;
        ((int **)(self + 0x4e8))[i] = List_InsertSorted(self + 0x22c, 0x10, 100);
        slot = List_InsertSorted(self + 0x144, 4, 100);
        switch (i) {
        case 0:
            cap.radius = 0x2800;
            bCapsule = 1;
            break;
        case 1:
            spheres[0].radius = spheres[1].radius = 0x2800;
            cap.radius = 0x2400;
            bCapsule = 1;
            break;
        case 2:
            spheres[0].radius = spheres[1].radius = 0x2800;
            cap.radius = 0x2400;
            bCapsule = 1;
            break;
        case 7:
        case 8:
            box.pos = data_02041dc8;
            box.a = data_02042270;
            box.b = data_02042264;
            box.c = data_02042258;
            box.w0 = 0x3400;
            box.w1 = 0x3c00;
            box.w2 = 0x1000;
            ((int *)(self + 0x50c))[i] = *slot = Ov107_HitShape_NewBox(&box);
            box.w0 = 0x2000;
            *((int **)(self + 0x4e8))[i] = Ov107_HitShape_NewBox(&box);
            continue;
        case 3:
        case 4:
        case 5:
        case 6:
            spheres[0].radius = 0x2000;
            spheres[1].radius = 0x1000;
            break;
        }
        *((int **)(self + 0x4e8))[i] = bCapsule && i != 1 ? Ov107_Mover_New(&cap) : Ov107_CloneResourceTransform(&spheres[0]);
        ((int *)(self + 0x50c))[i] = *slot = bCapsule ? Ov107_Mover_New(&cap) : Ov107_CloneResourceTransform(&spheres[1]);
    }
    *(int *)(self + 0x584) = 1;
    ((struct w8 *)(*(int *)(self + 0x504) + 8))->lo |= 2;
    ((struct w8 *)(*(int *)(self + 0x508) + 8))->lo |= 2;
    for (i = 0; i < 4; i++) {
        Ov252_ShowArmourPiece(self, i, 1);
    }
    Res_RequestIdPair(0x148);
}
