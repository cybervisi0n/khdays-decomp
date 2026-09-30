/* Runs the wall-collision view's little state machine, one step per call.
 *
 * State 1 waits for the player to come within the query's radius, measured on
 * the ground plane -- the sampled position's y is dropped before the distance.
 * Once inside it hides every collision node the room carries, binds both wall
 * meshes at once so either can be shown, switches the layer and moves to 2.
 *
 * State 2 waits for every node to report its ready bit, then moves to 3.
 *
 * State 3 rebinds each track entry to the OTHER mesh -- the flag picks the
 * opposite of what the opener chose, so the view swaps to the far wall --
 * restores the layer pair and moves to 4, where nothing more happens.
 *
 * Two codegen notes. State 2 walks the room pointer itself rather than a
 * separate node cursor, and reads the count once instead of every pass. And
 * state 3 keeps its own block-scope group, entry, mesh and counter: sharing
 * the function-scope ones stretches their live ranges across all three arms
 * and rotates the three registers this arm uses.
 */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct Ov020WallQuery {
    VecFx32 vOrigin;                    /* 0x00 */
    int nRadius0c;                          /* 0x0c */
    int bUpperWall10;                       /* 0x10 */
    int nState14;                           /* 0x14 */
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

extern char data_ov020_020800e4[];
extern char data_ov020_020800f0[];

extern VecFx32 *func_ov022_020881f8(int id);
extern int VEC_Distance(const VecFx32 *a, const VecFx32 *b);
extern struct TrackEntryGroup *GetTrackEntryBase(int track);
extern void *FindEntryByExactName(void *entry, const char *name);
extern void RoomMesh_ForEachPrimOnPoint(void *entry, int a, void *mesh, void *hook, int d);
extern void Ov002_SetWidgetStateByte(int kind, int layer, int a);
extern void Ov002_SetWidgetHidden(void);

int Ov020_StepWallView(struct Ov020WallQuery *query, int unused,
                        struct Ov020Room *room)
{
    int i;
    struct TrackEntryGroup *group;
    struct Ov020WallNode *node;
    void *entry;
    VecFx32 *sample;
    VecFx32 vGround;
    void *upper;
    void *lower;
    int all;
    int n;

    if (query->nState14 == 1) {
        sample = func_ov022_020881f8(QueryActiveStateOrDelegate());
        vGround = *sample;
        vGround.y = 0;
        if (VEC_Distance(&vGround, &query->vOrigin) <= query->nRadius0c) {
            i = 0;
            if (room->bNodeCount110 > 0) {
                node = room->aNodes114;
                do {
                    SceneNode_Disable(node);
                    i++;
                    node->bFlags108 &= ~4;
                    node = (struct Ov020WallNode *)((char *)node + 0x10c);
                } while (i < room->bNodeCount110);
            }

            group = GetTrackEntryBase(room->bTrackId0d);
            i = 0;
            if ((int)group->wCount > 0) {
                do {
                    entry = group->apEntry[i];
                    upper = FindEntryByExactName(entry, data_ov020_020800e4);
                    lower = FindEntryByExactName(entry, data_ov020_020800f0);
                    RoomMesh_ForEachPrimOnPoint(entry, 2, lower,
                                  (void *)Ov002_SetWidgetHidden, 1);
                    RoomMesh_ForEachPrimOnPoint(entry, 2, upper,
                                  (void *)Ov002_SetWidgetHidden, 1);
                    i++;
                } while (i < (int)group->wCount);
            }

            Ov002_SetWidgetStateByte(query->bUpperWall10 ? 9 : 10, 0x4d, 1);
            query->nState14 = 2;
        }
    } else if (query->nState14 == 2) {
        n = room->bNodeCount110;
        all = 1;
        i = 0;
        if (n > 0) {
            do {
                if ((room->aNodes114[0].bFlags108 & 4) == 0) {
                    all = 0;
                }
                i++;
                room = (struct Ov020Room *)((char *)room + 0x10c);
            } while (i < n);
        }
        if (all) {
            query->nState14 = 3;
        }
    } else if (query->nState14 == 3) {
        {
            struct TrackEntryGroup *g = GetTrackEntryBase(room->bTrackId0d);
            void *e;
            char *m;
            int k = 0;

            if ((int)g->wCount > 0) {
                do {
                    m = query->bUpperWall10 ? data_ov020_020800f0 : data_ov020_020800e4;
                    e = g->apEntry[k];
                    RoomMesh_ForEachPrimOnPoint(e, 2, FindEntryByExactName(e, m),
                                  (void *)Ov002_SetWidgetHidden, 0);
                    k++;
                } while (k < (int)g->wCount);
            }
        }
        Ov002_SetWidgetStateByte(query->bUpperWall10 ? 9 : 10,
                            query->bUpperWall10 ? 0x46 : 7, 1);
        query->nState14 = 4;
    }
    return 0;
}
