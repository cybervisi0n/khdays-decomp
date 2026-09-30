/* Spawns an element from the pool: places its node, sets its position, heading and motion, puts it
 * in its bucket and sets its height per mode. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct {
    int kind;
    int paramB;
    int paramA;
    int paramC;
    int angle;
} PlaceParams;

typedef struct PoolDesc PoolDesc;

typedef struct {
    char pad00[0x1c];
    VecFx32 position;
    int value28;
} Entry;

extern void *Ov002_ClaimPoolEntry(PoolDesc *pool, int index);
extern int Ov002_PlaceElementNode(void *obj, int node, PlaceParams *out,
                                int entryByte, int kind, int paramA, int paramB,
                                int paramC, int angle, int flag);
extern void Ov002_BuildSpawnPosition(VecFx32 *out, const VecFx32 *pos,
                                 const PlaceParams *params);
extern void Ov002_PushBucketNode(int idx, int *node);
extern int FX_Mul(int a, int b);
extern void Ov014_TickHeightMotion(void);
extern unsigned char data_0204c240;

void *Ov014_SpawnElement(PoolDesc *pool, unsigned short node,
                          unsigned short bucket, unsigned short kind,
                          unsigned char entryByte, VecFx32 *pos, short angle, int index)
{
    Entry *entry = (Entry *)Ov002_ClaimPoolEntry(pool, node);
    VecFx32 start;
    PlaceParams params;
    int height;

    Ov002_PlaceElementNode(entry, (int)((char *)entry + 0x2c), &params, node,
                        *(signed char *)((char *)pool + 0x6c),
                        *(short *)((char *)pool + 0x6e),
                        *(short *)((char *)pool + 0x70),
                        *(short *)((char *)pool + 0x72), angle, 1);
    Ov002_BuildSpawnPosition(&start, pos, &params);
    Actor_SetVecAndSyncChild((int *)((char *)entry + 0x38), pos);
    *(short *)((char *)entry + 0x18) = angle;
    entry->position = start;
    entry->value28 = params.paramA;
    *(unsigned char *)((char *)entry + 0x10) = (unsigned char)bucket;
    *(void **)((char *)entry + 0x0c) = Ov014_TickHeightMotion;
    *(unsigned short *)((char *)entry + 0x12) |= 0x48;
    *(unsigned short *)((char *)entry + 0x14) = kind;
    *(unsigned char *)((char *)entry + 0x16) = entryByte;
    *(unsigned char *)((char *)entry + 0x17) = 0;
    Ov002_PushBucketNode(bucket, (int *)entry);
    *(unsigned char *)((char *)entry + 0x1b1) = 0;
    height = (data_0204c240 & 4) ? (index >> 16) : (index & 0xffff);
    *(short *)((char *)entry + 0x1d0) = (short)height;
    *(int *)((char *)entry + 0x1cc) = 0;
    *(unsigned char *)((char *)entry + 0x1b3) = 0;
    *(int *)((char *)entry + 0x1b4) = 0x46bd;
    *(int *)((char *)entry + 0x1b8) = FX_Mul(*(int *)((char *)pool + 0x74), *(int *)((char *)entry + 0x1b4));
    *(int *)((char *)entry + 0x1bc) = FX_Mul(*(int *)((char *)pool + 0x78), *(int *)((char *)entry + 0x1b4));
    *(int *)((char *)entry + 0x1c0) = FX_Mul(*(int *)((char *)pool + 0x7c), *(int *)((char *)entry + 0x1b4));
    *(int *)((char *)entry + 0x1c8) = *(int *)((char *)entry + 0x1b4);
    Actor_SetBindingByte((char *)entry + 0x148, 1, 3);
    return entry;
}
