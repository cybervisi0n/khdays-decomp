/* Spawn an ov256 shard task (020d0d18 / 020d0e44, 0x20 bytes) under the owner's +0x3c scene: it
 * records the owner and `model`, starts at `pos` (+0xc) and the model is placed there. Returns the
 * task. */

#include "nitro/fx_types.h"

extern int CreateRegistryEntry(int scene, int kind, int size, void *cb, void *cb2, int **out);
extern void Srt_SetTranslation(void *transform, const VecFx32 *translation);
extern void Ov256_ShellDropStart(void);
extern void Ov256_TaskTeardown_FlagOwner(void);

int Ov256_SpawnShard(int owner, int model, VecFx32 *pos)
{
    int *entry;
    VecFx32 at;
    int task = CreateRegistryEntry(*(int *)(owner + 0x3c), 100, 0x20, Ov256_ShellDropStart, Ov256_TaskTeardown_FlagOwner, &entry);

    entry[1] = owner;
    entry[0] = model;
    at = *pos;
    *(VecFx32 *)(entry + 3) = *pos;
    Srt_SetTranslation((void *)(entry[0] + 4), &at);
    return task;
}
