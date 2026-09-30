/* Opens the wall-collision view for a room.
 *
 * Kind 9 means this is the upper wall, and that one flag picks all three of the
 * layer id, the mesh (col_wall10 against col_wall09) and the placement marker
 * ("up" against "down"). Every collision node the room carries is flagged and
 * shown, each track entry is rebound to the chosen mesh through the shared
 * hook, and the query block is filled from the marker's origin with its y
 * forced to zero.
 *
 * Two codegen notes: the entry count is tested signed, and the loop counter is
 * declared before the node pointer so the two land in the original's registers.
 */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct Ov020WallQuery {
    VecFx32 vOrigin;                    /* 0x00 */
    int nScale0c;                           /* 0x0c */
    int bUpperWall10;                       /* 0x10 */
    int bReady14;                           /* 0x14 */
    int pad18[2];
};

struct Ov020WallNode {
    char pad000[0x108];
    unsigned char bFlags108;                /* 0x108 */
    char pad109[3];
};

struct Ov020Room {
    char pad00[0xd];
    signed char bTrackId0d;                 /* 0x0d */
    char pad0e[0x102];
    signed char bNodeCount110;              /* 0x110 */
    char pad111[3];
    struct Ov020WallNode aNodes114[1];      /* 0x114 */
};

struct TrackEntryGroup {
    unsigned short pad00;
    unsigned short wCount;                  /* 0x02 */
    void **apEntry;                         /* 0x04 */
};

struct Ov020Placement {
    char pad00[8];
    VecFx32 vOrigin08;                  /* 0x08 */
};

extern char data_ov020_020800d8[];
extern char data_ov020_020800e4[];
extern char data_ov020_020800f0[];
extern char data_ov020_020800fc[];
extern char data_ov020_02080100[];

extern void Ov002_RebindGroupAnimations(const char *name, int a, int b, void *handle);
extern void MIi_CpuClearFast(int value, void *dst, unsigned int size);
extern int Ov002_GetSlotTableByte(void *handle);
extern void Ov002_SetWidgetStateByte(int kind, int layer, int a);
extern struct TrackEntryGroup *GetTrackEntryBase(int track);
extern void *FindEntryByExactName(void *entry, const char *name);
extern void RoomMesh_ForEachPrimOnPoint(void *entry, int a, void *mesh, void *hook, int d);
extern struct Ov020Placement *EntityMgr_FindCollEntry(int track, const char *name);
extern void Ov002_SetWidgetHidden(void);

void Ov020_OpenWallView(struct Ov020WallQuery *query, void *handle,
                         struct Ov020Room *room)
{
    struct TrackEntryGroup *group;
    struct Ov020Placement *placement;
    char *mesh;
    void *entry;
    int kind;
    int i;
    struct Ov020WallNode *node;

    Ov002_RebindGroupAnimations(data_ov020_020800d8, 8, 1, handle);
    MIi_CpuClearFast(0, query, 0x20);

    kind = Ov002_GetSlotTableByte(handle);
    query->bUpperWall10 = (kind == 9);
    Ov002_SetWidgetStateByte(kind, query->bUpperWall10 ? 7 : 0x46, 1);

    i = 0;
    if (room->bNodeCount110 > 0) {
        node = room->aNodes114;
        do {
            node->bFlags108 |= 2;
            SceneNode_Enable(node);
            i++;
            node = (struct Ov020WallNode *)((char *)node + 0x10c);
        } while (i < room->bNodeCount110);
    }

    group = GetTrackEntryBase(room->bTrackId0d);
    mesh = query->bUpperWall10 ? data_ov020_020800e4 : data_ov020_020800f0;

    i = 0;
    if ((int)group->wCount > 0) {
        do {
            entry = group->apEntry[i];
            RoomMesh_ForEachPrimOnPoint(entry, 2, FindEntryByExactName(entry, mesh),
                          (void *)Ov002_SetWidgetHidden, 0);
            i++;
        } while (i < (int)group->wCount);
    }

    placement = EntityMgr_FindCollEntry(room->bTrackId0d,
                              query->bUpperWall10 ? data_ov020_020800fc
                                                : data_ov020_02080100);
    query->vOrigin = placement->vOrigin08;
    query->vOrigin.y = 0;
    query->nScale0c = 0x4ccd;
    query->bReady14 = 1;
}
