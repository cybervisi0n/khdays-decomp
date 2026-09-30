/* Creates a script entity: asks ov002 for the record, clears its offset
 * vector, loads the default projection into its camera block, stamps the kind,
 * id and flag the caller passed, installs the state callback and registers the
 * whole thing under its kind.
 *
 * The offset vector is cleared through a local that is zeroed field by field
 * and then copied across, not through a declaration initialiser: the
 * initialiser form runs the aggregate-fill helper and addresses the local
 * through a scratch register, while the original stores the zero straight to
 * the stack slots. The local sits after the allocation call because that is
 * where the original builds it.
 */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

struct Ov020Entity {
    char pad00[0x0c];
    void *pOnState0c;                       /* 0x0c */
    signed char bKind10;                    /* 0x10 */
    char pad11[3];
    u16 hId14;                              /* 0x14 */
    u8 bFlag16;                             /* 0x16 */
    u8 bPhase17;                            /* 0x17 */
    char pad18[0xa8];
    VecFx32 vOffsetC0;                  /* 0xc0 */
    char padcc[0x58];
    char aCamera124[4];                     /* 0x124 */
};

extern struct Ov020Entity *Ov002_ClaimPoolEntry(int a, int b);
extern void Ov002_PushBucketNode(int kind, struct Ov020Entity *entity);
extern void Ov020_DrawEntity(void);

struct Ov020Entity *Ov020_CreateEntity(int a, int b, int kind, u16 id, u8 flag)
{
    struct Ov020Entity *entity;

    entity = Ov002_ClaimPoolEntry(a, b);
    {
        VecFx32 zero;

        zero.x = 0;
        zero.y = 0;
        zero.z = 0;
        entity->vOffsetC0 = zero;
    }
    Projection_LoadDefaults(entity->aCamera124);

    entity->bKind10 = (signed char)kind;
    entity->pOnState0c = (void *)Ov020_DrawEntity;
    entity->hId14 = id;
    entity->bFlag16 = flag;
    entity->bPhase17 = 0;
    Ov002_PushBucketNode(kind, entity);
    return entity;
}
