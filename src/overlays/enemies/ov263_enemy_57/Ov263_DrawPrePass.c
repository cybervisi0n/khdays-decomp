/* Draw pre-pass of the ov263 enemy (x3 with ov265/ov280). The +0xa0 pose is copied to +0x3dc and
 * uniformly scaled by 0x189e/0x1119. Outside sub-state 8 a pending slot-6 effect is finished; a
 * guarded actor (+0x1c4 bits 1/3) finishes its slot-4 effect and requests sub-state 3 when spent or
 * already knocked out (3 current, pending or next), else 2 unless already in 2. The three +0x3c0
 * pieces follow: piece 0 takes the +0x3cc part's transform and passes it on to the +0x3bc part;
 * pieces 1/2 take the +0x3d4/+0x3d8 part's transform moved 0.5 back/forward and scaled out
 * sideways by the same ratio. Finally the base pre-pass (020c7ca4) runs. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

typedef struct { int m[4]; VecFx32 trans; int pad[4]; } Srt;
struct Pieces { char pad[0x3c0]; int piece[3]; };
struct Piece { char pad[0x10]; Srt srt; };
struct Part { char pad[4]; Srt srt; };

extern int FX_Div(int num, int den);
extern void Srt_SetScaleXYZ(void *srt, int x, int y, int z);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Srt_SetTranslation(Srt *t, const VecFx32 *v);

void Ov263_DrawPrePass(char *self)
{
    int w = FX_Div(0x189e, 0x1119);
    Srt xf;
    VecFx32 off;
    signed char i;

    *(Srt *)(self + 0x3dc) = *(Srt *)(self + 0xa0);
    Srt_SetScaleXYZ(self + 0x3dc, w, w, w);
    if (*(signed char *)(self + 0x1c6) != 8 && *(void **)(*(int *)(self + 0x3b8) + 0x34) != 0) {
        TaskList_FinishByTag(*(void **)(self + 0x3c), *(void **)(*(int *)(self + 0x3b8) + 0x34));
        *(void **)(*(int *)(self + 0x3b8) + 0x34) = 0;
    }
    if ((*(unsigned char *)(self + 0x1c4) & 0xa) != 0) {
        if (*(void **)(*(int *)(self + 0x3b8) + 0x24) != 0) {
            TaskList_FinishByTag(*(void **)(self + 0x3c), *(void **)(*(int *)(self + 0x3b8) + 0x24));
            *(void **)(*(int *)(self + 0x3b8) + 0x24) = 0;
        }
        if (*(short *)(self + 0x200 + 0x1a) > 0 && *(signed char *)(self + 0x1c6) != 3
            && *(signed char *)(self + 0x1c8) != 3 && *(signed char *)(self + 0x1c7) != 3) {
            if (*(signed char *)(self + 0x1c6) != 2) {
                *(signed char *)(self + 0x1c7) = 2;
            }
        } else {
            *(signed char *)(self + 0x1c7) = 3;
        }
    }
    for (i = 0; i < 3; i++) {
        switch (i) {
        case 1:
        case 2:
            xf = i == 1 ? *(Srt *)(*(int *)(self + 0x3d4) + 4) : *(Srt *)(*(int *)(self + 0x3d8) + 4);
            off.x = (int)(((long long)FX_Div(0x189e, 0x1119) * 0x1000LL + 0x800) >> 12);
            off.y = 0;
            off.z = i == 1 ? 0x800 : -0x800;
            Vec3TransformViaTempMtx(&off, &xf, &off);
            VEC_Add(&xf.trans, &off, &off);
            Srt_SetTranslation(&xf, &off);
            ((struct Piece *)((struct Pieces *)self)->piece[i])->srt = xf;
            break;
        default:
            ((struct Piece *)((struct Pieces *)self)->piece[i])->srt = ((struct Part *)*(int *)(self + 0x3cc))->srt;
            ((struct Piece *)**(int **)(self + 0x3bc))->srt = ((struct Piece *)((struct Pieces *)self)->piece[i])->srt;
            break;
        }
    }
    Ov107_AiState_PostTickBase(self);
}
