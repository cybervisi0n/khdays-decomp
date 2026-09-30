#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov183_SteerSweep();

struct NodeBits { unsigned char ready : 1; };
extern int SetIndexedSlot();

/* Starts the steering sweep after applying its frame-scaled vertical drain.
 * The algebraic zero in the config mask keeps the source dependency that makes
 * MWCC schedule the halfword update like the retail binary; it optimizes away. */
void Ov183_BeginSteerSweep(int this_)
{
    int holder = *(int *)(this_ + 4);
    int node;
    int m;
    unsigned int config;

    *(VecFx32 *)(holder + 0x54) = *(VecFx32 *)(holder + 0x78);

    m = *(int *)(*(int *)this_ + 0x2c) * 0x1e;
    *(int *)(holder + 0x7c) -= (int)((((long long)m << 7) + 0x800) >> 12);

    node = *(int *)holder;
    if (((struct NodeBits *)(node + 0x17a))->ready == 0) return;

    Ov107_PostTagUpdate((Actor *)node, 0x12, 0);
    Ov107_StartAnim(*(int *)(*(int *)holder + 0x390), 0, 0);

    node = *(int *)holder;
    config = *(unsigned short *)(node + 0x60);
    {
        unsigned int field =
            (unsigned short)(((config << 16) >> 24) & ~0x40);
        config &= ~(0xff00 | (field ^ field));
        field <<= 24;
        field = config | (field >> 16);
        *(unsigned short *)(node + 0x60) = field;
    }
    *(int *)(holder + 0x6c) = 0;
    *(unsigned char *)(holder + 0x51) &= ~2;
    SetIndexedSlot(this_, *(signed char *)(this_ + 0x20), Ov183_SteerSweep);
}
