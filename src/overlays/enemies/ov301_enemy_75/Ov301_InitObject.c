/* Initialise the ov301 object: install its seven handler pointers, seed the camera block
 * and the bounding box, then create the two pooled sub-objects.
 *
 * The box is built in a LOCAL and copied to self->box in one go, with each max derived from
 * the min FIELD just stored (`box.max.x = box.min.x + 0x1dd3`).  Deriving it from the local
 * that fed the min instead lets mwcc constant-fold to 0xee9 and materialise that cheaply
 * from another live constant, three instructions short.
 *
 * THE LAST LEVER, and it is one that was not in the catalogue: `register void *fn2c`.
 * The function was instruction-for-instruction identical to the ROM -- 92/92, all 16 relocs
 * matching by name and position -- for two sessions, differing only in which registers three
 * short-lived constants landed in.  The ROM spends r3, the register pushed purely for stack
 * alignment, on one of them; mwcc reached for callee-saved r6/r7 instead.  Twelve spellings
 * had been tried against that: every declaration order, the constants inlined, an explicit
 * local for 0x17, four different parameter counts, the box block moved, the box written
 * straight through a pointer.  None moved it, because none of them changes what mwcc is
 * WILLING to allocate -- only `register` does.
 *
 * Also load-bearing, and cheap to lose in a tidy-up: the fn2c pointer is held in that
 * register across the whole box construction and stored AFTER it, and the three mins are
 * assigned x, z, y in that order.
 *
 * With this, ov301 is 16/16.
 */

#include "nitro/fx_types.h"

extern int Ov301_OnDespawn, Ov301_PropagateBlockToLinkedNodes, Ov301_CreateRegistryEntryForActor;
extern int func_ov301_020cc1d8, func_ov301_020cc1e4;
extern int Ov301_OnHitQueueAction3, Ov301_SetSubitemStatesAndConfig;
extern int data_02041dc8[], data_02042264[];
extern int Ov107_PackTextureHandle(int p, int a);
extern int CreateSubitemInstance0xB4(int r0);
extern void RegisterSubscriberSlot(int a, int b);
extern int *List_InsertSorted(void *p, int sz, int n);
extern int Ov107_Mover_New(void *p);
extern int Ov107_CloneResourceTransform(void *p);

struct Box  { VecFx32 min, max; };
struct WorkBlock { int v[8]; };

struct Ov301Obj {
    char      pad_00[0x08];
    void     *fn08;          /* 0x08 */
    void     *fn0c;          /* 0x0c */
    char      pad_10[0x18];
    void     *fn28;          /* 0x28 */
    void     *fn2c;          /* 0x2c */
    void     *fn30;          /* 0x30 */
    char      pad_34[0x30];
    int       cam[4];        /* 0x64 */
    char      pad_74[0x28];
    int       p9c;           /* 0x9c */
    char      pad_a0[0xa4];
    char      pool144[0x6a]; /* 0x144 */
    unsigned short flags1ae; /* 0x1ae */
    char      pad_1b0[0x19];
    char      b1c9;          /* 0x1c9 */
    char      pad_1ca[0x06];
    void     *fn1d0;         /* 0x1d0 */
    char      pad_1d4[0x08];
    void     *fn1dc;         /* 0x1dc */
    char      pad_1e0[0x1c];
    struct Box box;          /* 0x1fc */
    char      pad_214[0x18];
    char      pool22c[0x158];/* 0x22c */
    int       p384;          /* 0x384 */
    int      *p388;          /* 0x388 */
    int       p38c;          /* 0x38c */
};

void Ov301_InitObject(struct Ov301Obj *self)
{
    int minX;
    int minY;
    int minZ;
    register void *fn2c;

    struct WorkBlock work;
    struct Box box;
    int *slot;

    minX = 0xfffff116;
    minZ = 0xfffff7a8;
    minY = 0x17;

    self->fn08 = &Ov301_OnDespawn;
    self->fn0c = &Ov301_PropagateBlockToLinkedNodes;
    self->fn30 = &Ov301_CreateRegistryEntryForActor;
    self->fn28 = &func_ov301_020cc1d8;

    fn2c = &func_ov301_020cc1e4;

    box.min.x = minX;
    box.min.y = minY;
    box.min.z = minZ;

    box.max.x = box.min.x + 0x1dd3;
    box.max.y = box.min.y + 0x1c27;
    box.max.z = box.min.z + 0xd64;

    self->fn2c = fn2c;
    self->fn1d0 = &Ov301_OnHitQueueAction3;
    self->fn1dc = &Ov301_SetSubitemStatesAndConfig;

    self->b1c9 = 2;

    self->cam[3] = 0x1900;
    self->cam[0] = 0;
    self->cam[1] = 0x400;
    self->cam[2] = 0;

    self->box = box;

    self->p384 = CreateSubitemInstance0xB4(Ov107_PackTextureHandle((int)self, 0));
    RegisterSubscriberSlot(self->p9c, self->p384);

    self->flags1ae |= 0x10;

    *(VecFx32 *)&work.v[0] = *(VecFx32 *)data_02041dc8;
    *(VecFx32 *)&work.v[3] = *(VecFx32 *)data_02042264;
    work.v[6] = 0x4000;
    work.v[7] = 0x1900;

    self->p388 = List_InsertSorted(&self->pool22c[0], 0x10, 0x64);
    *self->p388 = Ov107_Mover_New(&work);

    slot = List_InsertSorted(&self->pool144[0], 4, 0x64);
    *slot = Ov107_CloneResourceTransform(&self->cam[0]);
    self->p38c = *slot;
}
