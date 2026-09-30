/* Drop entry of the ov256 actor's shell: a 10.0 drop ray from the +0xc point finds the ground (the
 * rig's world collision) and the point's +0x10 height moves down by the hit fraction of it. The shell
 * rig (+0) is reset and placed at the point, shown again (flag 1 of its +0x5c clears), actions
 * 0/2/4/3/1 are disabled and its animation stopped; the +0x18 clock and +0x1c flag reset and brain
 * slot +0x20 runs 020d0e5c. */

#include "nitro/fx_types.h"

extern int Collision_CastRay(int collision, VecFx32 *start, VecFx32 *ray);
extern void SrtTransform_SetIdentity(void *transform);
extern void Srt_SetTranslation(void *transform, const VecFx32 *translation);
extern void SetSubitemState(int item, int channel, int a, int b);
extern void RefreshObjectCallbacks(int item, int a);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_ShockwaveTick(void);

void Ov256_ShellDropStart(int *node)
{
    int *state = (int *)node[1];
    VecFx32 ray = {0};
    int hit;
    int world;

    world = *(int *)(state[1] + 4);
    ray.y = -0xa000;
    hit = Collision_CastRay(*(int *)(world + 0x7c), (VecFx32 *)(state + 3), &ray);
    if (hit != 0) {
        state[4] += (int)(((long long)*(int *)(hit + 0xc) * ray.y) >> 27);
    }
    SrtTransform_SetIdentity((void *)(*state + 4));
    Srt_SetTranslation((void *)(*state + 4), (VecFx32 *)(state + 3));
    *(int *)(*state + 0x5c) &= ~2;
    SetSubitemState(*state, 0, 0, 0);
    SetSubitemState(*state, 2, 0, 0);
    SetSubitemState(*state, 4, 0, 0);
    SetSubitemState(*state, 3, 0, 0);
    SetSubitemState(*state, 1, 0, 0);
    RefreshObjectCallbacks(*state, 0);
    state[6] = 0;
    *(unsigned char *)(state + 7) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_ShockwaveTick);
}
