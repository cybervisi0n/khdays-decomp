/* Spawn an ov252 shard for `owner` (0203c5c0: 0x64/0x28 node, tick 020d2e70, class 020d2f54): it
 * remembers the spawner and the owner, starts at the spawner's +0x570 model point with velocity `vel`,
 * its model moves there (0203ca30), the spawner plays effect 3 there and the shard keeps its slot.
 * Returns the node handle. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Shard { int owner; char *spawner; VecFx32 pos; VecFx32 vel; char pad20[4]; u8 slot; };

extern int CreateRegistryEntry(int scene, int kind, int size, void *cb, void *cls, struct Shard **out);
extern void Srt_SetTranslation(void *srt, const VecFx32 *v);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov252_ShardStart(void);
extern void Ov252_TaskTeardown_FlagOwner(void);

int Ov252_SpawnShard(char *self, int owner, VecFx32 *vel, u8 slot)
{
    struct Shard *shard;
    int handle;

    handle = CreateRegistryEntry(*(int *)(self + 0x3c), 0x64, 0x28, Ov252_ShardStart, Ov252_TaskTeardown_FlagOwner, &shard);
    shard->spawner = self;
    shard->owner = owner;
    shard->pos = *(VecFx32 *)(*(int *)(self + 0x570) + 0x14);
    shard->vel = *vel;
    Srt_SetTranslation((void *)(shard->owner + 4), (VecFx32 *)(*(int *)(self + 0x570) + 0x14));
    func_ov107_020c0b90((int)shard->spawner, 3, *(VecFx32 *)(*(int *)(self + 0x570) + 0x14), 0);
    shard->slot = slot;
    return handle;
}
