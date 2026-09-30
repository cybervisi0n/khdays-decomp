/* Charge-hit tick of the ov237 actor: while fewer than three charge steps remain (+0x34) the +0x3c
 * push turns flat toward the +0x3dc target at 0.1875; a 3.0 sphere 2.0 above the +0x38 point hits
 * with push data_ov237_020d1b64 (effect 1, kind 4) and counts hits in +0x55. When the +4 rig
 * finishes: out of steps, blocked (+0x17a bit 1), after five hits or with a +0x4b4 hold the charge
 * ends (pose 0x14, effect 0x11 unless held, then 020cfe70); otherwise a step is spent and pose 0x13
 * replays (effect 0x10 unless held). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { VecFx32 pos; int radius; } Sphere;
typedef struct { u8 b0 : 1; u8 b1 : 1; } Bits;

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov237_AttackHitTest(int *node, void *sphere, void *box, void *segment, VecFx32 *push, int once, unsigned short effect, int kind);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov237_AiStep_QueueAction2OnAnimEnd(void);
extern const VecFx32 data_ov237_020d1b64;

void Ov237_ChargeHitTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    Sphere sphere;
    VecFx32 push;

    push = data_ov237_020d1b64;
    if (state[0xd] < 3) {
        VEC_Subtract((VecFx32 *)(*(int *)(*state + 0x3dc) + 0x190), (VecFx32 *)(*state + 0xb0), &dir);
        dir.y = 0;
        VEC_Normalize(&dir, &dir);
        ScaleVec3Fx12(0x300, &dir, &dir);
        *(VecFx32 *)(state + 0xf) = dir;
    }
    sphere.pos = *(VecFx32 *)state[0xe];
    sphere.pos.y += 0x2000;
    sphere.radius = 0x3000;
    if (Ov237_AttackHitTest(node, &sphere, 0, 0, &push, 0, 1, 4) != 0) {
        (*((u8 *)state + 0x55))++;
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    if (state[0xd] == 0 || ((Bits *)(*state + 0x17a))->b1 || *((u8 *)state + 0x55) >= 5 ||
        *(int *)(*state + 0x4b4) != 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 0x14, 0);
        if (*(int *)(*state + 0x4b4) == 0) {
            func_ov107_020c0b90(*state, 0x11, *(VecFx32 *)state[0xe], 0);
        }
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov237_AiStep_QueueAction2OnAnimEnd);
        return;
    }
    state[0xd]--;
    Ov107_PostTagUpdate((Actor *)(*state), 0x13, 0);
    if (*(int *)(*state + 0x4b4) != 0) {
        return;
    }
    func_ov107_020c0b90(*state, 0x10, *(VecFx32 *)state[0xe], 0);
}
