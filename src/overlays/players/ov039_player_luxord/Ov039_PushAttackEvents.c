/* Push this enemy's attack event(s): event 5 at each attack anchor (Ov039_GetAttackAnchor)
 * with range 0x1000, the enemy's facing (angle at +0x80 of its +0x20 node, biased by 0x8000)
 * and a per-band flag read from a table by the progress band ((+0x7b0 - 0x9000) / 0x3000, 15
 * bands, wrapping to 0). While the shared rig's +0x2cd4 flag is clear one event is pushed from
 * the single-row table; once it is set both anchors get one, each from its own row. */

#include "nitro/fx_types.h"

typedef struct { int v[15]; } Ov039BandRow;

extern void Ov039_GetAttackAnchor(char *self, int side, VecFx32 *out);              /* Ov039_GetAttackAnchor */
extern void Ov022_MarshalNetworkRecord(char *self, int event, VecFx32 *pos, int range, int angle, int flag);
extern char *data_ov039_020b5600;
extern const Ov039BandRow data_ov039_020b53c8;      /* the single-anchor per-band flags */

void Ov039_PushAttackEvents(char *self)
{
    VecFx32 vAnchor;
    char *rig = data_ov039_020b5600 + 0xd4 + 0x2c00;
    int band;
    int angle;

    angle = (unsigned short)(*(unsigned short *)(*(char **)(self + 0x20) + 0x80) - 0x8000);
    angle = (unsigned short)(angle + 0x8000);
    band = (*(int *)(self + 0x7b0) - 0x9000) / 0x3000;
    if (band >= 15) band = 0;
    if (*(int *)rig == 0) {
        Ov039BandRow single = data_ov039_020b53c8;
        Ov039_GetAttackAnchor(self, 0, &vAnchor);
        Ov022_MarshalNetworkRecord(self, 5, &vAnchor, 0x1000, angle, single.v[band]);
        return;
    }
    {
        int table[2][15] = {
            { 1, 1, 1, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0 },
            { 2, 2, 2, 0, 0, 0, 2, 2, 2, 1, 1, 1, 0, 0, 0 },
        };
        Ov039_GetAttackAnchor(self, 0, &vAnchor);
        Ov022_MarshalNetworkRecord(self, 5, &vAnchor, 0x1000, angle, table[0][band]);
        Ov039_GetAttackAnchor(self, 1, &vAnchor);
        Ov022_MarshalNetworkRecord(self, 5, &vAnchor, 0x1000, angle, table[1][band]);
    }
}
