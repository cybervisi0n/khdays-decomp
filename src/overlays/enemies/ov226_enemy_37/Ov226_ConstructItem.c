/* Construction of the ov226 enemy's item: installs the handlers (+8 020d411c, +0xc 020d4140,
 * +0x1c 020d4178, +0x30 020d42d8), raises bits 1-3 and 6 of the +0x60 high byte and bit 2 of
 * +0x1ae and of the +0x9c parent's +0x5c, sets +0x70 to 0xbe6, clears +0x54/+0x58/+0x64/+0x6c
 * and copies +0x70 to +0x68 (the data_ov226_020d4b50 kind is copied to the stack meanwhile),
 * builds the +0x384 item from pose 0x1a of the +0x390 pool (channels 0 and 4 enabled),
 * subscribes it to the parent and finalises it, builds the +0x394 sub-item from the saved kind
 * (registered on the actor, bit 1 of +0x5c raised), and finally a +0x22c list slot takes the
 * +0x64 pose as +0x388 with bit 1 of its +8 low byte raised; +0x38c clears. */

#include "nitro/fx_types.h"

typedef struct {
    unsigned f : 8;
} B8;

struct Ov226Saved { int w; };
static inline void VEC_Set(VecFx32 *v, int x, int y, int z) { v->x = x; v->y = y; v->z = z; }

extern int Ov107_PackTextureHandle(int owner, int kind);
extern int CreateSubitemInstance0xB4(int a);
extern void SetSubitemState(int obj, int mode, int a, int b);
extern void RegisterSubscriberSlot(int a, int obj);
extern void RefreshObjectCallbacks(int obj, int a);
extern void Ov107_EnqueueValue(int self, int obj);
extern int List_InsertSorted(int a, int b, int c);
extern int Ov107_CloneResourceTransform(int a);
extern void Ov226_Destroy_2(void);
extern void Ov226_TickAndSyncModelXform(void);
extern void Ov226_ItemHandleMessage(void);
extern void Ov226_SpawnActorRegistryEntry(void);
extern const struct Ov226Saved data_ov226_020d4b50;

void Ov226_ConstructItem(char *self) {
    int owner;
    unsigned short v;
    struct Ov226Saved saved;
    int sub;

    owner = *(int *)(self + 0x390);
    *(void **)(self + 8) = (void *)Ov226_Destroy_2;
    *(void **)(self + 0xc) = (void *)Ov226_TickAndSyncModelXform;
    *(void **)(self + 0x1c) = (void *)Ov226_ItemHandleMessage;
    *(void **)(self + 0x30) = (void *)Ov226_SpawnActorRegistryEntry;

    saved = data_ov226_020d4b50;
    v = *(unsigned short *)(self + 0x60);
    *(unsigned short *)(self + 0x60) =
        (unsigned short)((v & ~0xff00)
                         | (((((unsigned int)v << 0x10) >> 0x18 | 0x4e) << 0x18) >> 0x10));

    *(unsigned short *)(self + 0x1ae) |= 4;
    *(int *)(self + 0x70) = 0xbe6;
    *(int *)(self + 0x54) = 0;
    *(int *)(self + 0x58) = 0;
    *(int *)(*(int *)(self + 0x9c) + 0x5c) |= 4;
    VEC_Set((VecFx32 *)(self + 0x64), 0, *(int *)(self + 0x70), 0);

    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(owner, 0x1a));
    SetSubitemState(*(int *)(self + 0x384), 0, 0, 1);
    SetSubitemState(*(int *)(self + 0x384), 4, 0, 1);
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    RefreshObjectCallbacks(*(int *)(self + 0x384), 0);

    sub = *(int *)(self + 0x394) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(owner, saved.w));
    Ov107_EnqueueValue((int)self, sub);
    *(int *)(*(int *)(self + 0x394) + 0x5c) |= 2;

    *(int *)(self + 0x388) = List_InsertSorted((int)(self + 0x22c), 0x10, 0x64);
    **(int **)(self + 0x388) = Ov107_CloneResourceTransform((int)(self + 0x64));
    ((B8 *)(*(int *)(self + 0x388) + 8))->f |= 2;
    *(int *)(self + 0x38c) = 0;
}
