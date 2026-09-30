/* Spawn of the ov244 leap entry: registers a 0x14-byte entry (CreateRegistryEntry, callbacks
 * ce4ac/ce590) holding the owner and the actor and places the owner at the actor's +0x3c8 item's
 * +0x14 point raised by 0x200 plus the actor's +0xb4 height. Returns the spawn result. */

#include "nitro/fx_types.h"

extern int CreateRegistryEntry(int list, int a, int b, void *cb2, void *cb1, int **out);
extern void Srt_SetTranslation(int dst, VecFx32 *src);
extern void Ov244_TaskTeardown_FlagOwner(void);
extern void Ov244_EnterLeap(void);

int Ov244_SpawnLeapEntry(int actor, int owner)
{
    int *entry;
    VecFx32 at;
    int r = CreateRegistryEntry(*(int *)(actor + 0x3c), 100, 0x14,
                          &Ov244_EnterLeap, &Ov244_TaskTeardown_FlagOwner, &entry);
    entry[0] = owner;
    entry[3] = actor;
    at = *(VecFx32 *)(*(int *)(entry[3] + 0x3c8) + 0x14);
    at.y = *(int *)(entry[3] + 0xb4) + 0x200;
    Srt_SetTranslation(*entry + 4, &at);
    return r;
}
