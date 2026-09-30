/* Spawns a marker element from the pool at a position with a heading and puts it in its bucket. */

#include "nitro/fx_types.h"

extern void *Ov002_ClaimPoolEntry(void *pool, int index);
extern void Ov002_PushBucketNode(int idx, int *node);
extern void Ov014_Element_Tick(void);

void *Ov014_SpawnMarker(void *pool, unsigned short b, unsigned short c,
                          unsigned short d, unsigned char e, void *vec, int g) {
    VecFx32 *v = (VecFx32 *)vec;
    unsigned char *entry = (unsigned char *)Ov002_ClaimPoolEntry(pool, b);
    *(VecFx32 *)(entry + 0xd0) = *v;
    *(unsigned short *)(entry + 0xa8) = (unsigned short)g;
    *(unsigned short *)(entry + 0x2c) |= 0x20;
    *(unsigned short *)(entry + 0x18) = (unsigned short)g;
    *(VecFx32 *)(entry + 0x1c) = *v;
    *(int *)(entry + 0x28) = *(int *)((char *)pool + 0x68);
    entry[0x135] = 0;
    entry[0x134] = 0;
    entry[0x10] = (unsigned char)c;
    *(void **)(entry + 0xc) = Ov014_Element_Tick;
    *(unsigned short *)(entry + 0x12) |= 8;
    *(unsigned short *)(entry + 0x14) = d;
    entry[0x16] = (unsigned char)e;
    entry[0x17] = 3;
    Ov002_PushBucketNode(c, (int *)entry);
    return entry;
}
