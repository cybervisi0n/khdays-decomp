/* Ground-strike enter tick of an ov257 state: the owner's +0x24 hook receives note 0 of
 * data_ov257_020d325c and the nearest target (020cab14) becomes +0x5c. With one, a ray is cast
 * from 10.0 above its +0x190 point 30.0 straight down through the world's +0x7c collision; on a
 * plain surface (no +8 flag) the ray is clipped to the plane (+0xc), the hit point is sent to the
 * hook in the 14-byte message of data_ov257_020d32f6 and reaction +0x408 mode 0x2b fires there.
 * Animation 0x10 plays, the +0x50 timer and +0x65 clear and the tick hands over to
 * Ov257_StrikeWindUpTick. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int value; } Fx32;
typedef struct { u16 lo; u16 hi; } Cmd4;
typedef struct { u16 id; u8 kind; u8 cmd; u8 flag; u8 pos[9]; } Cmd14;

#define PACK(cmd, dead, src, at)                                              \
    (dead) = (src);                                                           \
    ((u8 *)&(cmd))[at] = (u8)(((unsigned int)(dead).value >> 0x10 & 0x7f)     \
                              | ((unsigned int)(dead).value >> 0x18 & 0x80)); \
    ((u8 *)&(cmd))[(at) + 1] = (u8)((unsigned int)(dead).value >> 8);         \
    ((u8 *)&(cmd))[(at) + 2] = (u8)(dead).value

/* the note table seen as a word-aligned record: its note 1 follows a 4-byte word */
extern const Cmd4 data_ov257_020d325c;
extern const Cmd14 data_ov257_020d32f6;
extern int Ov107_FindNearestObject(int obj, int kind);
extern int *Collision_CastRay(void *collision, VecFx32 *origin, VecFx32 *dir);
extern void ScaleVec3Fixed27(int plane, VecFx32 *in, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov257_StrikeWindUpTick(int *node);

void Ov257_GroundStrikeEnterTick(int *node)
{
    int *state = (int *)node[1];
    Cmd4 note = data_ov257_020d325c;
    VecFx32 p;
    VecFx32 ray;
    Cmd14 msg;
    Fx32 scratchZ;
    Fx32 scratchY;
    Fx32 scratchX;

    if (*(void (**)(int, Cmd4 *, int))(*state + 0x24) != 0) {
        (*(void (**)(int, Cmd4 *, int))(*state + 0x24))(*state, &note, 4);
    }
    state[0x18] = Ov107_FindNearestObject(*state, 0);
    if (state[0x18] != 0) {
        int world = *(int *)(*state + 4);
        int *wall;

        p = *(VecFx32 *)(state[0x18] + 0x190);
        p.y += 0xa000;
        ray.x = 0;
        ray.z = 0;
        ray.y = -0x1e000;
        wall = Collision_CastRay(*(void **)(world + 0x7c), &p, &ray);
        if (wall != 0 && wall[2] == 0) {
            msg = data_ov257_020d32f6;
            ScaleVec3Fixed27(wall[3], &ray, &ray);
            VEC_Add(&p, &ray, &p);
            PACK(msg, scratchX, *(Fx32 *)&p.x, 5);
            PACK(msg, scratchY, *(Fx32 *)&p.y, 8);
            PACK(msg, scratchZ, *(Fx32 *)&p.z, 11);
            if (*(void (**)(int, Cmd14 *, int))(*state + 0x24) != 0) {
                (*(void (**)(int, Cmd14 *, int))(*state + 0x24))(*state, &msg, 0xe);
            }
            Ov107_BuildAndSendUpdate(*state, (short)*(int *)(*state + 0x408), 0x2b, &p);
        }
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x10, 0);
    state[0x15] = 0;
    *((u8 *)state + 0x76) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov257_StrikeWindUpTick);
}
