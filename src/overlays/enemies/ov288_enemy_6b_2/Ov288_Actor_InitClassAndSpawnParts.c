/*
 * Ov288_Actor_InitClassAndSpawnParts -- Ov288_Actor_InitClassAndSpawnParts.
 *
 * Class init for the ov287/ov288/ov289 actor triplet, installed at +0x18c by the
 * constructor at 020d37b8. It installs the nine class handlers, raises flags 0x100 and
 * 0x8000, sets the state byte at +0x1c9 to 2, ORs 0x44 into the high byte of the flags
 * halfword at +0x60 and 0xc into the flags at +0x1ae, loads the two unit-scale pairs at
 * +0x64..+0x70, spawns three subitems -- the first registered with the owner and its node
 * list initialised, the other two attached with bit 1 raised on their +0x5c -- then builds
 * an identity basis at unit 0x800 scale, allocates the 0x10 x 100 pool at +0x22c and starts
 * resource 0x15b on the node it creates.
 *
 * Ghidra: Ov288_Actor_InitClassAndSpawnParts(Ov287Actor *pActor), types /khdays/Ov287Actor
 * and /khdays/ActorSpawnBasis, with the handler slots at 0x1c/0x20/0x24/0x38 carved out of
 * /khdays/Actor's former padding.
 *
 * Byte-exact codegen notes (mwccarm 3.0/139), all three needed:
 *  - The two later subitems are stored with a chained assignment through the object field,
 *    `sub = *(void **)(obj + off) = f(...)`. That stores straight out of r0 before the next
 *    argument setup overwrites it. Writing it as two statements costs the allocator lr, and
 *    the halfword temp twenty-five instructions earlier moves out of lr with it.
 *  - Three callees are declared int rather than void even though the results are discarded.
 *    A non-void return on the callee keeps r0 reserved across the call, so the following
 *    load lands in r1 and the scheduler can hoist the next call's argument setup into the
 *    block. It only works with all three changed together.
 *  - The unit scale is written `scale = obj->nScale070 = 0x1000;` and read back for +0x68.
 *    Chaining it into a local makes mwcc materialise the constant at block entry, which the
 *    four plain assignments do not (measured: a plain constant floats exactly fifteen
 *    instructions, never the thirty-eight this function needs), while keeping the stores in
 *    the ROM's 0x70, 0x64, 0x68, 0x6c order that a two-target chain would collapse.
 */

/* the 0x3c basis block built on the stack and handed to Ov107_HitShape_NewBox */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct ActorSpawnBasis {
    int scale;
    int field04;
    int field08;
    VecFx32 right;
    VecFx32 up;
    VecFx32 forward;
    int scaleX;
    int scaleY;
    int scaleZ;
};

struct Actor {
    u16 flags000;
    char pad002[6];
    void *pfnHandler008;
    void *pfnHandler00c;
    char pad010[0xc];
    void *pfnHandler01c;
    void *pfnHandler020;
    void *pfnHandler024;
    char pad028[8];
    void *pfnHandler030;
    char pad034[4];
    void *pfnHandler038;
    char pad03c[0x24];
    u16 flags060;
    u16 field062;
    int field064;
    int scale068;
    int field06c;
    int scale070;
    char pad074[0x28];
    int owner09c;
    char pad0a0[0x18c];
    u16 flags22c;
    char pad22e[0xfa];
    void *pfnHandler328;
};

extern void *Ov107_PackTextureHandle(void *actor, int index);
extern void *CreateSubitemInstance0xB4(void *node);
extern int RegisterSubscriberSlot(int owner, void *sub);
extern int List_Init(void *list);
extern int Ov107_EnqueueValue(void *actor, void *sub);
extern u32 List_InsertSorted(int pool, int itemSize, int count);
extern void *Ov107_HitShape_NewBox(struct ActorSpawnBasis *basis);
extern void Res_RequestIdPair(int n);

extern void Ov288_TeardownActorSlotsAndDoubleList(void);
extern void Ov288_CopyBlockThenNotify(void);
extern void Ov288_Actor_HandleEvent(void);
extern void Ov288_SpawnActorRegistryEntry(void);
extern void Ov288_LoadWaypoints(void);
extern void Ov288_SendMessage26(void);
extern void Ov288_StampField394ToMsgAndForward(void);
extern void Ov288_Actor_ResolveHit(void);
extern void Ov288_Actor_SetSubStateAndNotify(void);

extern const VecFx32 data_02042270;
extern const VecFx32 data_02042264;
extern const VecFx32 data_02042258;

void Ov288_Actor_InitClassAndSpawnParts(struct Actor *actor)
{
    struct ActorSpawnBasis basis;
    void *sub;
    void **entry;
    void *node;
    unsigned short *hw;
    unsigned int h;
    int scale;

    actor->flags000 |= 0x100;
    actor->pfnHandler008 = (void *)Ov288_TeardownActorSlotsAndDoubleList;
    actor->pfnHandler00c = (void *)Ov288_CopyBlockThenNotify;
    actor->pfnHandler01c = (void *)Ov288_Actor_HandleEvent;
    actor->pfnHandler030 = (void *)Ov288_SpawnActorRegistryEntry;
    actor->pfnHandler038 = (void *)Ov288_LoadWaypoints;
    actor->pfnHandler020 = (void *)Ov288_SendMessage26;
    actor->pfnHandler024 = (void *)Ov288_StampField394ToMsgAndForward;
    *(void **)((char *)actor + 0x1d0) = (void *)Ov288_Actor_ResolveHit;
    *(void **)((char *)actor + 0x1dc) = (void *)Ov288_Actor_SetSubStateAndNotify;

    *(u8 *)((char *)actor + 0x1c9) = 2;
    /* flags060.hi |= 0x44 -- explicit-shift form; a bitfield |= adds a redundant mask */
    hw = (unsigned short *)((char *)actor + 0x60);
    h = *hw;
    *hw = h & ~0xff00 | (((((unsigned int)h << 0x10) >> 0x18 | 0x44) << 0x18) >> 0x10);
    *(u16 *)((char *)actor + 0x1ae) |= 0xc;

    actor->flags000 |= 0x8000;

    scale = actor->scale070 = 0x1000;
    actor->field064 = 0;
    actor->scale068 = scale;
    actor->field06c = 0;

    sub = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(actor, 0));
    *(void **)((char *)actor + 0x384) = sub;
    RegisterSubscriberSlot(actor->owner09c, *(void **)((char *)actor + 0x384));
    List_Init((char *)actor + 0x398);

    sub = *(void **)((char *)actor + 0x3c0) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(actor, 1));
    Ov107_EnqueueValue(actor, sub);
    *(u32 *)((char *)sub + 0x5c) |= 2;

    sub = *(void **)((char *)actor + 0x3c8) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(actor, 2));
    Ov107_EnqueueValue(actor, sub);
    *(u32 *)((char *)sub + 0x5c) |= 2;

    basis.right = data_02042270;
    basis.up = data_02042264;
    basis.forward = data_02042258;
    basis.scaleZ = 0x800;
    basis.scaleY = 0x800;
    basis.scaleX = 0x800;
    basis.field04 = 0x800;
    basis.scale = 0;
    basis.field08 = 0;

    *(u32 *)((char *)actor + 0x388) = List_InsertSorted((int)((char *)actor + 0x22c), 0x10, 100);
    node = Ov107_HitShape_NewBox(&basis);
    entry = *(void ***)((char *)actor + 0x388);
    *entry = node;
    Res_RequestIdPair(0x15b);
}
