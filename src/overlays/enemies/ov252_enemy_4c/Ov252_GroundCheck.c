/* Ground check of an ov252 helper: +0x70 clears and a ray from the +8 track along the
 * (0, -896.0, 66.0) offset turned by the +0x54 angle (020cdafc) is cast through the +4 item's
 * world. When it lands on plain ground the result is whether the hit depth (distance * ray
 * height, in eighths) is within 0x10 of the track height; with no plain hit the result is left
 * undefined, as in the original. */

#include "nitro/fx_types.h"

typedef struct { void *a; void *b; void *c; int d; } CollisionHit;

extern VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec);
extern CollisionHit *Collision_CastRay(void *collision, VecFx32 *origin, VecFx32 *direction);
extern const VecFx32 data_ov252_020d435c;

int Ov252_GroundCheck(int *node)
{
    int *state = (int *)node[1];
    VecFx32 pos;
    CollisionHit *hit;
    int item;
    int depth;
    int diff;

    item = *(int *)(*state + 4);
    pos = *(VecFx32 *)state[2];
    {
    VecFx32 ray = data_ov252_020d435c;

    state[0x1c] = 0;
    ray = Ov252_TurnVecY(state[0x15], &ray);
    hit = Collision_CastRay(*(void **)(item + 0x7c), &pos, &ray);
    if (hit != 0 && hit->c == 0) {
        depth = (int)(((long long)hit->d * ray.y) >> 27);
        if (depth < 0) {
            depth = -depth;
        }
        diff = depth - *(int *)(state[2] + 4);
        if (diff < 0) {
            diff = -diff;
        }
        return diff <= 0x10;
    }
    }
}
