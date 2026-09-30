/* Spawn of the ov244 pounce entry: registers a 0x14-byte entry (CreateRegistryEntry, callbacks
 * cebd8/cec70) linking the actor (entry[1]) and the owner (entry[0]); when the actor's +0x50
 * kind is 1 the owner gets the ce8bc handler at +0x74 and a back-link at +0x84; the owner is
 * then placed at the actor's +0x3cc item's +0x14 point raised by 0x200 plus the actor's +0xb4
 * height, with entry[3] holding its "DM002" joint handle. Returns the spawn result. */

#include "nitro/fx_types.h"

extern int CreateRegistryEntry(int list, int a, int b, void *cb2, void *cb1, int **out);
extern int FindResourceIndexByName(int owner, const char *name);
extern void Srt_SetTranslation(int dst, VecFx32 *src);
extern void Ov244_TaskTeardown_FlagOwner_2(void);
extern void Ov244_ResetChannelsA(void);
extern void Ov244_ArmSwingSweepA(void);
extern const char data_ov244_020d38a0[];

int Ov244_SpawnPounceEntry(int actor, int owner)
{
    int *entry;
    VecFx32 at;
    int r = CreateRegistryEntry(*(int *)(actor + 0x3c), 100, 0x14,
                          &Ov244_ResetChannelsA, &Ov244_TaskTeardown_FlagOwner_2, &entry);
    entry[1] = actor;
    entry[0] = owner;
    if (*(int *)(entry[1] + 0x50) == 1) {
        *(void **)(entry[0] + 0x74) = (void *)&Ov244_ArmSwingSweepA;
        *(int **)(entry[0] + 0x84) = entry;
    }
    at = *(VecFx32 *)(*(int *)(entry[1] + 0x3cc) + 0x14);
    at.y = *(int *)(entry[1] + 0xb4) + 0x200;
    entry[3] = FindResourceIndexByName(entry[0], data_ov244_020d38a0);
    Srt_SetTranslation(*entry + 4, &at);
    return r;
}
