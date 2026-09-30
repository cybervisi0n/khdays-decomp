/* Spawn an ov252 bomb (class 020d3cf8, update 020d3bdc) for `owner` at `pos`: it avoids the
 * other nine live bombs of the +0x774 pair table (skipping slot `slot`): when one lies within 6.0
 * the bomb is moved to the first of the eight ring offsets around its point (6.0 on each axis
 * and diagonal) whose ground, probed 3.125 down from 16.0 up, sits within 0x10. The bomb then
 * rests at height 0 on the owner's +4 transform and records its slot (+0x24) and kind (+0x25).
 * Returns the spawn handle. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 v[8]; } Ring8;
typedef struct { void *a; void *b; void *c; int d; } CollisionHit;
struct Bomb { int owner; char *spawner; VecFx32 pos; char pad14[0x10]; u8 slot; u8 kind; };
struct BombPair { int obj; int active; };
struct Spawner { char pad[0x774]; struct BombPair pair[10]; };

extern int CreateRegistryEntry(int scene, int kind, int size, void *cb, void *cls, struct Bomb **out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern CollisionHit *Collision_CastRay(void *collision, VecFx32 *origin, VecFx32 *direction);
extern void Srt_SetTranslation(void *srt, const VecFx32 *v);
extern void Ov252_GemStart(void);
extern void Ov252_TaskTeardown_FlagOwner_3(void);
extern const VecFx32 data_02041dc8;
extern const Ring8 data_ov252_020d44ac;
extern const VecFx32 data_ov252_020d44a0;

int Ov252_SpawnBomb(char *self, int owner, VecFx32 *pos, signed char slot, u8 kind)
{
    VecFx32 p;
    VecFx32 d;
    VecFx32 a;
    VecFx32 b;
    struct Bomb *bomb;
    int handle;
    signed char i;
    signed char j;
    int item;
    int depth;
    int diff;

    handle = CreateRegistryEntry(*(int *)(self + 0x3c), 0x64, 0x28, Ov252_GemStart, Ov252_TaskTeardown_FlagOwner_3, &bomb);
    bomb->spawner = self;
    bomb->owner = owner;
    bomb->pos = *pos;
    p = bomb->pos;
    i = 0;
    {
    Ring8 cur;
    Ring8 ring = data_ov252_020d44ac;
    VecFx32 zero = data_02041dc8;
    VecFx32 down = data_ov252_020d44a0;

    for (; i < 10; i++) {
        if (i == slot) {
            continue;
        }
        if (((struct Spawner *)self)->pair[i].active == 0) {
            continue;
        }
        VEC_Subtract((VecFx32 *)(((struct Spawner *)self)->pair[i].obj + 0x14), &p, &d);
        if (VEC_Normalize(&d, &d) >= 0x6000) {
            continue;
        }
        {
            cur = ring;
            for (j = 0; j < 8; j++) {
                a = zero;
                item = *(int *)(bomb->spawner + 4);
                b = down;
                VEC_Add(&p, &cur.v[j], &a);
                a.y = 0x10000;
                {
                    CollisionHit *hit = Collision_CastRay(*(void **)(item + 0x7c), &a, &b);

                    if (hit != 0 && hit->c == 0) {
                        depth = (int)(((long long)hit->d * b.y) >> 27);
                        if (depth < 0) {
                            depth = -depth;
                        }
                        diff = depth - a.y;
                        if (diff < 0) {
                            diff = -diff;
                        }
                        if (diff <= 0x10) {
                            a.y = 0;
                            bomb->pos = a;
                            i = 0xa;
                            break;
                        }
                    }
                }
            }
        }
    }
    }
    bomb->pos.y = 0;
    Srt_SetTranslation((void *)(bomb->owner + 4), &bomb->pos);
    bomb->slot = slot;
    bomb->kind = kind;
    return handle;
}
