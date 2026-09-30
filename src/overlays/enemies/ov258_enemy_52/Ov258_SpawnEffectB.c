/* Spawn an ov258 effect task (priority 100, 0x1c bytes, 020d0738 / 020d07e8): it keeps the owner,
 * the item and the start position (the item is placed there) and the kind byte at +0x1a; returns
 * the task handle. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct {
    int item;
    int owner;
    VecFx32 pos;
    char pad14[6];
    u8 kind;
} Ov258Spawn;

extern int CreateRegistryEntry(int list, int prio, int size, void *cb, void *init, Ov258Spawn **out);
extern void Srt_SetTranslation(int srt, VecFx32 *pos);
extern void Ov258_ShotEffectStart(void);
extern void Ov258_TaskTeardown_FlagOwner(void);

int Ov258_SpawnEffectB(int owner, int item, VecFx32 *pos, int kind)
{
    Ov258Spawn *spawn;
    VecFx32 at;
    int handle;

    handle = CreateRegistryEntry(*(int *)(owner + 0x3c), 100, 0x1c, Ov258_ShotEffectStart, Ov258_TaskTeardown_FlagOwner, &spawn);
    spawn->owner = owner;
    spawn->item = item;
    at = *pos;
    spawn->pos = *pos;
    Srt_SetTranslation(spawn->item + 4, &at);
    spawn->kind = kind;
    return handle;
}
