/* Spawn of the ov115 enemy's action entry (and its byte-identical twins): registers a 0x20-byte
 * entry (CreateRegistryEntry, callbacks cc8f8/cc994) holding the spawner, the owner actor and an
 * optional target, sets bit 2 of the owner's +0x5c flags and places the owner at the target's
 * +0x190 point (or its own +0x74 position without a target). Returns the spawn result. */

#include "nitro/fx_types.h"

extern int CreateRegistryEntry(int list, int a, int b, void *cb2, void *cb1, int **out);
extern void Srt_SetTranslation(int dst, VecFx32 *src);
extern void Ov116_ChildTaskTeardown(void);
extern void Ov116_ActionEntry(void);

static inline void PlaceAt(int obj, VecFx32 at)
{
    Srt_SetTranslation(obj + 4, &at);
}

int Ov116_SpawnActionEntry(int owner, int spawner, int target)
{
    int *entry;
    VecFx32 at;
    int r = CreateRegistryEntry(*(int *)(owner + 0x3c), 100, 0x20,
                          &Ov116_ActionEntry, &Ov116_ChildTaskTeardown, &entry);
    entry[1] = owner;
    entry[0] = spawner;
    entry[2] = target;
    *(unsigned int *)(*entry + 0x5c) |= 4;
    at = entry[2] != 0 ? *(VecFx32 *)(entry[2] + 0x190) : *(VecFx32 *)(entry[1] + 0x74);
    PlaceAt(*entry, at);
    return r;
}
