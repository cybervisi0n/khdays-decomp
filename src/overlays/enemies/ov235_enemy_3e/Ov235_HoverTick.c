/* Hover tick of an ov235 state. While the +0xc idle byte is set the +0x40 rate is the frame rate
 * x 3.75 and, with a nearest target (020cab14, kept in +0x5c), the +0x2c orientation looks at it
 * (0203cd7c about data_02042264) and +0x68 takes the unit direction to it (zeroed without one).
 * Once it clears the +0x40 rate becomes x 0.6 and the +0x44 timer accumulates the frame rate; an
 * armed +0x74 request with fewer than 8 +0x78 uses plays animation 0x26 (looping), clears the
 * timer and hands over to Ov235_VolleyTick, otherwise animation 0x1c plays, the +0x3a8 part
 * plays motion 0x15, the owner's +0x64 velocity becomes 1.5 up and the tick hands over to
 * Ov235_TurnTick_2. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[4]; } Quat;
typedef struct { int m[9]; } Mtx33;

extern int Ov107_FindNearestObject(int obj, int kind);
extern void Mtx33_LookAt(Mtx33 *out, const VecFx32 *at, const VecFx32 *from, const VecFx32 *up);
extern void Quat_FromMtx33(Quat *out, const Mtx33 *m);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov235_VolleyTick(int *node);
extern void Ov235_TurnTick_2(int *node);
extern const VecFx32 data_02042264;
extern const VecFx32 data_02041dc8;

static inline void VEC_Set(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void Ov235_HoverTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 m;
    int owner;
    int target;

    if (*(unsigned char *)state[3] == 0) {
        state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 50;
        state[0x11] += *(int *)(node[0] + 0x2c);
        if (state[0x1d] != 0 && state[0x1e] < 8) {
            Ov107_PostTagUpdate((Actor *)(*state), 0x26, 1);
            state[0x11] = 0;
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov235_VolleyTick);
            return;
        }
        Ov107_PostTagUpdate((Actor *)(*state), 0x1c, 0);
        Ov107_StartAnim(*(int *)(*state + 0x3a8), 0x15, 0);
        VEC_Set((VecFx32 *)(*state + 0x64), 0, 0x1800, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov235_TurnTick_2);
        return;
    }
    state[0x10] = *(int *)(node[0] + 0x2c) * 30 / 8;
    target = state[0x17] = Ov107_FindNearestObject(*state, 0);
    if (target != 0) {
        owner = *state;
        Mtx33_LookAt(&m, (VecFx32 *)(target + 0x74), (VecFx32 *)(owner + 0x74), &data_02042264);
        Quat_FromMtx33((Quat *)(state + 0xb), &m);
        VEC_Subtract((void *)(target + 0x74), (void *)(owner + 0x74), (VecFx32 *)(state + 0x1a));
        VEC_Normalize((VecFx32 *)(state + 0x1a), (VecFx32 *)(state + 0x1a));
    } else {
        *(VecFx32 *)(state + 0x1a) = data_02041dc8;
    }
}
