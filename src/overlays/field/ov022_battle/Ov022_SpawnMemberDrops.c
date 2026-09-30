/* Spawns drops of one kind (up to 100) above a player's position for the player's group. */

#pragma opt_propagation off

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Ov022ColorBuffer {
    u8 channel[6];
};

struct Ov022ColorWork {
    struct Ov022ColorBuffer color;
    char _pad06[2];
    VecFx32 vector;
};

struct Ov022ActiveState {
    int _unk00;
    void *activeBase;
};

extern VecFx32 *func_ov022_020881f8(int kind);
extern int Ov022_GetEntryField66(int kind);
extern int Ov002_GetSlotTableByte(int state);
extern void Ov002_SpawnAllDrops(u8 *color, int id, int *vector);
extern struct Ov022ActiveState data_ov022_020b2e78;

void Ov022_SpawnMemberDrops(int kind, int channel, int intensity) {
    struct Ov022ColorWork work;
    u8 *color;
    void *activeBase;
    int zero;

    color = work.color.channel;
    zero = 0;
    activeBase = data_ov022_020b2e78.activeBase;
    color[0] = zero;
    color[1] = 0;
    color[2] = 0;
    color[3] = 0;
    color[4] = 0;
    color[5] = 0;
    if (activeBase == 0) {
        return;
    }
    if (intensity > 100) {
        intensity = 100;
    }
    work.color.channel[channel] = (u8)intensity;
    work.vector = *func_ov022_020881f8(kind);
    work.vector.y += 0x800;
    Ov002_SpawnAllDrops(work.color.channel,
                        (u16)Ov002_GetSlotTableByte(Ov022_GetEntryField66(kind)),
                        (int *)&work.vector);
}
