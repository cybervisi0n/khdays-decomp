/* Per-frame update of the ov258 actor: the five body segments (+0x3d8) take the transform of their
 * joint (+0x430 .. +0x444 chain) and point toward the next joint (length into +0x70); the first segment
 * sits 12.0 up and its +0x3d4 shadow rig copies it with a 3.375 (moves 3, 5, 7, 8) or 1.875 radius.
 * The three effect parts (+0x3ec..+0x3f4) sit on the +0x43c, +0x448 and +0x42c joints (the last 3.0
 * higher). With a +0x454 target the +0x430 head looks at it; then the base update runs. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int m[9]; } Mtx33;
typedef struct { int w[4]; VecFx32 t; int s[4]; } SrtTransform;
struct Xf10 { char pad[0x10]; SrtTransform srt; };
struct Ov258Body { char pad[0x3d8]; int parts[8]; };

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Mtx33_LookAt(Mtx33 *out, const VecFx32 *target, const VecFx32 *from, const VecFx32 *up);
extern void Quat_FromMtx33(void *srt, const Mtx33 *rot);
extern const VecFx32 data_02042264;

void Ov258_Update(char *self)
{
    VecFx32 d;
    SrtTransform srt;
    Mtx33 look;
    VecFx32 pos;
    signed char i;
    int len;

    for (i = 0; i < 5; i++) {
        int part = ((struct Ov258Body *)self)->parts[i];

        switch (i) {
        case 0:
            srt = *(SrtTransform *)(*(int *)(self + 0x430) + 4);
            srt.t.y = 0xc000;
            VEC_Subtract((VecFx32 *)(*(int *)(self + 0x42c) + 0x14), &srt.t, &d);
            len = VEC_Normalize(&d, &d);
            break;
        case 1:
            srt = *(SrtTransform *)(*(int *)(self + 0x434) + 4);
            VEC_Subtract((VecFx32 *)(*(int *)(self + 0x438) + 0x14), &srt.t, &d);
            len = VEC_Normalize(&d, &d);
            break;
        case 2:
            srt = *(SrtTransform *)(*(int *)(self + 0x438) + 4);
            VEC_Subtract((VecFx32 *)(*(int *)(self + 0x43c) + 0x14), &srt.t, &d);
            len = VEC_Normalize(&d, &d);
            break;
        case 3:
            srt = *(SrtTransform *)(*(int *)(self + 0x440) + 4);
            VEC_Subtract((VecFx32 *)(*(int *)(self + 0x444) + 0x14), &srt.t, &d);
            len = VEC_Normalize(&d, &d);
            break;
        case 4:
            srt = *(SrtTransform *)(*(int *)(self + 0x444) + 4);
            VEC_Subtract((VecFx32 *)(*(int *)(self + 0x448) + 0x14), &srt.t, &d);
            len = VEC_Normalize(&d, &d);
            break;
        }
        *(VecFx32 *)(part + 0x64) = d;
        *(int *)(part + 0x70) = len;
        if (i == 0) {
            if (*(signed char *)(self + 0x1c6) == 3 || *(signed char *)(self + 0x1c6) == 5 ||
                *(signed char *)(self + 0x1c6) == 7 || *(signed char *)(self + 0x1c6) == 8) {
                *(int *)(**(int **)(self + 0x3d4) + 0x74) = 0x3600;
            } else {
                *(int *)(**(int **)(self + 0x3d4) + 0x74) = 0x1e00;
            }
            *(VecFx32 *)(**(int **)(self + 0x3d4) + 0x64) = d;
            *(int *)(**(int **)(self + 0x3d4) + 0x70) = len;
        }
        ((struct Xf10 *)((struct Ov258Body *)self)->parts[i])->srt = srt;
        if (i == 0) {
            ((struct Xf10 *)**(int **)(self + 0x3d4))->srt = srt;
        }
    }
    for (; i < 8; i++) {
        int part = ((struct Ov258Body *)self)->parts[i];

        pos = i == 5 ? *(VecFx32 *)(*(int *)(self + 0x43c) + 0x14)
                     : (i == 6 ? *(VecFx32 *)(*(int *)(self + 0x448) + 0x14) : *(VecFx32 *)(*(int *)(self + 0x42c) + 0x14));
        *(VecFx32 *)(part + 0x58) = pos;
        if (i == 7) {
            *(int *)(part + 0x5c) += 0x3000;
        }
    }
    if (*(int *)(self + 0x454) != 0) {
        Mtx33_LookAt(&look, (VecFx32 *)(*(int *)(self + 0x454) + 0x190), (VecFx32 *)(*(int *)(self + 0x430) + 0x14),
                      &data_02042264);
        Quat_FromMtx33((void *)(*(int *)(self + 0x430) + 4), &look);
    }
    Ov107_AiState_PostTickBase(self);
}
