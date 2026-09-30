/* Spawn an ov252 gem at `pos` for `owner` (0203c5c0: 0x64/0x40 node, tick 020d326c, class 020d3454):
 * it remembers the spawner, the owner and the point, its model moves there (0203ca30), and it keeps
 * its slot, its kind and a live flag. Returns the node handle. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Gem { int owner; char *spawner; char pad8[4]; VecFx32 pos; char pad18[0x1c]; u8 slot; u8 kind; u8 live; };

extern int CreateRegistryEntry(int scene, int kind, int size, void *cb, void *cls, struct Gem **out);
extern void Srt_SetTranslation(void *srt, const VecFx32 *v);
extern void Ov252_GemShotStart(void);
extern void Ov252_TaskTeardown_FlagOwner_2(void);

int Ov252_SpawnGem(char *self, int owner, VecFx32 *pos, signed char slot, u8 kind)
{
    struct Gem *gem;
    int handle;

    handle = CreateRegistryEntry(*(int *)(self + 0x3c), 0x64, 0x40, Ov252_GemShotStart, Ov252_TaskTeardown_FlagOwner_2, &gem);
    gem->spawner = self;
    gem->owner = owner;
    gem->pos = *pos;
    Srt_SetTranslation((void *)(gem->owner + 4), pos);
    gem->slot = slot;
    gem->kind = kind;
    gem->live = 1;
    return handle;
}
