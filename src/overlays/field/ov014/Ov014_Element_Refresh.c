/* Re-evaluates the element's game-state gate, then rebinds its model sequence and animation. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern unsigned int GameState_GetField(int bitOffset, int bitCount);
extern void Ov014_ActorRelease(int this_);
extern int Ov002_LookupChannelEntry(int arg0);
extern void RegisterSeqAndInit(int this_, int arg1, int arg2, int arg3);
extern void Ov002_RebindAnimTracks(short *pAnim, int nBlend, int nFrame);

void Ov014_Element_Refresh(int this_) {
    char *self = (char *)this_;
    int entry = *(int *)(self + 8);
    VecFx32 saved;

    if (*(signed char *)(self + 0x135) == 0) {
        unsigned int state = GameState_GetField(*(u16 *)(self + 0x14),
                                            *(unsigned char *)(self + 0x16));
        if ((((state & 0xfffe) << 15) >> 16) == 1)
            Ov014_ActorRelease(this_);
    }

    if (*(signed char *)(entry + 0x58) == 0)
        return;

    saved = *(VecFx32 *)(self + 0xd0);
    RegisterSeqAndInit(this_ + 0x2c, Ov002_LookupChannelEntry(entry + 0x58), 1, 4);
    *(VecFx32 *)(self + 0xd0) = saved;
    *(u16 *)(self + 0xa8) = *(u16 *)(self + 0x18);
    *(u16 *)(self + 0x2c) |= 0x20;
    *(u16 *)(self + 0x12) |= 4;
    Ov002_RebindAnimTracks((short *)(self + 0x2c), *(signed char *)(self + 0x135), 0);
}
