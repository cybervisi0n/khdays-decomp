/* Update of the ov258 beam: its rig clears flag 1 and the +0x28 / +0x2c clocks run up at the frame
 * rate. On the first frame (+0x48) the beam aims from the owner's +0x44c hand at the target 13.6 high:
 * the look rotation is kept in +0x38, its length (less the owner radius and 1.0) in +0x30, and the
 * sweep starts a quarter turn to the +0x4a side. Later frames sweep that quarter turn back across
 * (1.57 - 6.28 x 1/+0x2c, mirrored by +0x4a) and cut the length at the first wall hit within 48.0.
 * Each frame the beam direction is stored in the owner's +0x3f8 slot, the beam rig sits on the
 * +0x44c / +0x450 hand (by +0x49) stretched to 1/3 + 1/7 of the length, the glow rig at the tip
 * (3.0), and the rotation is copied to the owner's +0x410. After 0x500 once the rig is idle, the
 * beam releases its owner effect slots (+0x49 and 0x26 / 0x27), hides the glow and ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int x, y, z, w; } Quat;
typedef struct { int m[9]; } Mtx33;
struct EffectPair { int res; int handle; };
struct Ov258Effects { char pad[0x464]; struct EffectPair pair[0x30]; };
struct Joint { char pad[0x14]; VecFx32 pos; };
struct Ov258Beams { char pad[0x3f8]; VecFx32 dirs[2]; };

extern void Mtx33_LookAt(Mtx33 *out, const VecFx32 *target, const VecFx32 *from, const VecFx32 *up);
extern void Quat_FromMtx33(Quat *out, const Mtx33 *m);
extern void Vec3TransformViaTempMtx(VecFx32 *out, const Quat *q, const VecFx32 *in);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern void Vec4_Normalize(Quat *out, Quat *in);
extern void QuatFromAxisAngle(Quat *out, const VecFx32 *axis, int angle);
extern void Quat_Multiply(Quat *out, const Quat *a, const Quat *b);
extern void Srt_SetTranslation(void *transform, const VecFx32 *translation);
extern void Srt_SetRotationQuat(void *srt, const Quat *rot);
extern void Srt_SetScaleXYZ(void *transform, int x, int y, int z);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int FX_Div(int value, int denom);
extern int Collision_CastRay(int collision, VecFx32 *start, VecFx32 *ray);
extern void ScaleVec3Fixed27(int scale, VecFx32 *in, VecFx32 *out);
extern void Task_MarkFinished(int *node);
extern const VecFx32 data_02042258;
extern const VecFx32 data_02042264;

static inline int FX_MUL(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}
/* position of the hand the beam leaves from: +0x44c when +0x49 is 0x19, else +0x450. The ternary picks
 * the position itself (picking the joint and taking ->pos after it colours the select differently). */
#define HAND_POS(st) (*((signed char *)(st) + 0x49) == 0x19 ? &(*(struct Joint **)((st)[2] + 0x44c))->pos \
                                                     : &(*(struct Joint **)((st)[2] + 0x450))->pos)

void Ov258_BeamUpdate(int *node)
{
    int *state = (int *)node[1];
    Mtx33 look;
    VecFx32 d;
    VecFx32 fwd;
    VecFx32 target;
    Quat q;
    VecFx32 unit;
    VecFx32 tip;
    VecFx32 dir;
    Quat rot;
    VecFx32 ray;
    VecFx32 start;
    VecFx32 end;

    *(int *)(state[0] + 0x5c) &= ~2;
    state[0xa] += *(int *)(node[0] + 0x2c);
    state[0xb] += *(int *)(node[0] + 0x2c);
    if (*((u8 *)state + 0x48) == 0) {
        (*((u8 *)state + 0x48))++;
        target = *(VecFx32 *)(state[3] + 0x190);
        target.y = 0xda00;
        Mtx33_LookAt(&look, &target, (VecFx32 *)(*(int *)(state[2] + 0x44c) + 0x14), &data_02042264);
        Quat_FromMtx33(&q, &look);
        Vec3TransformViaTempMtx(&fwd, &q, &data_02042258);
        VEC_Subtract(&target, (VecFx32 *)(*(int *)(state[2] + 0x44c) + 0x14), &d);
        unit = d;
        state[0xc] = VEC_Normalize(&unit, &unit);
        state[0xc] -= *(int *)(state[2] + 0x80) + 0x1000;
        d.y = 0;
        VEC_Normalize(&d, &d);
        VEC_DotProduct(&fwd, &d);
        Vec4_Normalize(&q, &q);
        *(Quat *)(state + 0xe) = q;
        QuatFromAxisAngle(&q, &data_02042264, *((signed char *)state + 0x4a) == 0 ? 0x1922 : -0x1922);
        Quat_Multiply(&q, &q, (Quat *)(state + 0xe));
        Vec3TransformViaTempMtx(&fwd, &q, &data_02042258);
        ((struct Ov258Beams *)state[2])->dirs[*((signed char *)state + 0x4a)] = fwd;
        Srt_SetTranslation((void *)(state[0] + 4), HAND_POS(state));
        Srt_SetRotationQuat((void *)(state[0] + 4), &q);
        Srt_SetScaleXYZ((void *)(state[0] + 4), 0x1000, 0x1000, state[0xc] / 3 + state[0xc] / 7);
        ScaleVec3Fx12(state[0xc], &((VecFx32 *)(state[2] + 0x3f8))[*((signed char *)state + 0x4a)], &tip);
        VEC_Add(&tip, HAND_POS(state), &tip);
        Srt_SetTranslation((void *)(state[1] + 4), &tip);
        Srt_SetScaleXYZ((void *)(state[1] + 4), 0x3000, 0x3000, 0x3000);
        *(Quat *)(state[2] + 0x410) = q;
    } else {
        int t = FX_Div(state[0xb], 0x1000);
        int world;
        int hit;

        QuatFromAxisAngle(&rot, &data_02042264,
                      (*((signed char *)state + 0x4a) == 0 ? 0x1922 : -0x1922) +
                          FX_MUL(*((signed char *)state + 0x4a) == 0 ? -0x6488 : 0x6488, t));
        Quat_Multiply(&rot, &rot, (Quat *)(state + 0xe));
        Vec3TransformViaTempMtx(&dir, &rot, &data_02042258);
        ((struct Ov258Beams *)state[2])->dirs[*((signed char *)state + 0x4a)] = dir;
        start = *(VecFx32 *)(*(int *)(state[2] + 0x44c) + 0x14);
        world = *(int *)(state[2] + 4);
        ScaleVec3Fx12(0x30000, &dir, &ray);
        hit = Collision_CastRay(*(int *)(world + 0x7c), &start, &ray);
        if (hit != 0) {
            ScaleVec3Fixed27(*(int *)(hit + 0xc), &ray, &dir);
            state[0xc] = VEC_Normalize(&dir, &dir);
        }
        Srt_SetTranslation((void *)(state[0] + 4), HAND_POS(state));
        Srt_SetRotationQuat((void *)(state[0] + 4), &rot);
        Srt_SetScaleXYZ((void *)(state[0] + 4), 0x1000, 0x1000, state[0xc] / 3 + state[0xc] / 7);
        ScaleVec3Fx12(state[0xc], &((VecFx32 *)(state[2] + 0x3f8))[*((signed char *)state + 0x4a)], &end);
        VEC_Add(&end, HAND_POS(state), &end);
        Srt_SetTranslation((void *)(state[1] + 4), &end);
        Srt_SetScaleXYZ((void *)(state[1] + 4), 0x3000, 0x3000, 0x3000);
        *(Quat *)(state[2] + 0x410) = rot;
    }
    if (state[0xa] < 0x500 && *(u8 *)(state[0] + 0xad) != 0) {
        return;
    }
    ((struct Ov258Effects *)state[2])->pair[*((signed char *)state + 0x49)].handle = 0;
    *(int *)(state[1] + 0x5c) |= 2;
    ((struct Ov258Effects *)state[2])->pair[*((signed char *)state + 0x49) == 0x19 ? 0x26 : 0x27].handle = 0;
    Task_MarkFinished(node);
}
