/* Build the ov238 partner: its brain (020d2810), update (020d2834), spawn (020d29d8), message (020d290c)
 * and teardown (020d2978) callbacks are installed, the body is a 0.625 sphere with 0 x 1.0 speeds;
 * model 0x19 of the +0x398 owner's set becomes the +0x384 rig (subscribed to the scene, pose scaled
 * 1.3) and model 0x1a the attached, hidden +0x3a4 model. A collision cylinder from the sphere is
 * registered in the +0x22c pool (+0x1ae bit 2, cylinder flag 2), bits 1-3 and 6 of the +0x60 high
 * byte clear, a second cylinder goes to the +0x144 pool (+0x38c), and the owner's four +0x3fc
 * shorts are copied to +0x39c. The +0x60 clear is a plain bitfield `&=`: the hand-expanded
 * insert keeps the fca8 argument setup out of the update. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 center; int nRadius; } Sphere;
typedef struct { unsigned f : 8; } B8;
struct Ov238Limits { char pad[0x39c]; short limits[4]; };
struct Ov238Owner { char pad[0x3fc]; short limits[4]; };
struct Hw60 { unsigned short lo : 8; unsigned short hi : 8; };

extern void Ov238_Destroy(void);
extern void Ov238_Update(void);
extern void Ov238_CreateAiTask(void);
extern void Ov238_OnMessage(void);
extern void Ov238_Teardown(void);
extern void *Ov107_PackTextureHandle(int set, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void Srt_SetScaleUniform(void *srt, int scale);
extern void Ov107_EnqueueValue(char *self, int model);
extern int *List_InsertSorted(void *pool, int count, int size);
extern int Ov107_CloneResourceTransform(void *sphere);

void Ov238_Partner_Build(char *self)
{
    Sphere spare;
    int set = *(int *)(self + 0x398);
    int *cyl;
    int i;

    *(void **)(self + 8) = Ov238_Destroy;
    *(void **)(self + 0xc) = Ov238_Update;
    *(void **)(self + 0x30) = Ov238_CreateAiTask;
    *(void **)(self + 0x1c) = Ov238_OnMessage;
    *(void **)(self + 0x34) = Ov238_Teardown;
    *(int *)(self + 0x70) = 0xa00;
    *(int *)(self + 0x54) = 0;
    *(int *)(self + 0x58) = 0x1000;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(set, 0x19));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    Srt_SetScaleUniform(self + 0xa0, 0x14cd);
    *(int *)(self + 0x3a4) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(set, 0x1a));
    Ov107_EnqueueValue(self, *(int *)(self + 0x3a4));
    *(int *)(*(int *)(self + 0x3a4) + 0x5c) |= 2;
    *(int **)(self + 0x388) = List_InsertSorted(self + 0x22c, 0x10, 0x64);
    **(int **)(self + 0x388) = Ov107_CloneResourceTransform(self + 0x64);
    *(u16 *)(self + 0x1ae) |= 4;
    ((B8 *)(*(int *)(self + 0x388) + 8))->f |= 2;
    ((struct Hw60 *)(self + 0x60))->hi &= ~0x4e;
    cyl = List_InsertSorted(self + 0x144, 4, 0x64);
    *(int *)(self + 0x38c) = *cyl = Ov107_CloneResourceTransform(&spare);
    for (i = 0; i < 4; i++) {
        ((struct Ov238Limits *)self)->limits[i] = (*(struct Ov238Owner **)(self + 0x398))->limits[i];
    }
}
