/* Draw hook of the ov146 actor's +0x384 model: while its owner shows the effect (+0x38c), every entity
 * of the owner's scene list (+0x388 set's +4 grid, +0x80 list) flagged 0x10 (+0x1c4) is drawn with it:
 * the model's +0x78 animation takes frame n % 28 + 3, and the model is scaled to twice the entity's
 * radius and placed on its +0x74 point before the draw (0203bc78). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void SrtTransform_SetIdentity(void *srt);
extern int *List_First(void *list);
extern int *List_Next(void *list);
extern void NNS_G3dMdlSetMdlPolygonID(int a, int b, int c);
extern void Srt_SetScaleUniform(void *srt, int scale);
extern void Srt_SetTranslation(void *srt, const VecFx32 *v);
extern int Obj_RenderModel(char *model, int arg);

void Ov146_DrawMarkedEntities(char *model, int arg)
{
    int n;
    char *owner;
    int grid;
    int *it;
    char *e;

    owner = *(char **)(model + 0x84);
    grid = *(int *)(*(int *)(owner + 0x388) + 4);
    n = 0;
    SrtTransform_SetIdentity(model + 0x30);
    if (*(int *)(owner + 0x38c) == 0) {
        return;
    }
    it = List_First((void *)(grid + 0x80));
    e = it == 0 ? 0 : (char *)*it;
    while (e != 0) {
        if (*(u8 *)(e + 0x1c4) & 0x10) {
            NNS_G3dMdlSetMdlPolygonID(*(int *)(*(int *)(*(int *)(owner + 0x384) + 0x88) + 0x78), 0, n % 28 + 3);
            Srt_SetScaleUniform(model + 0x30, *(int *)(e + 0x80) * 2);
            Srt_SetTranslation(model + 0x30, (VecFx32 *)(e + 0x74));
            Obj_RenderModel(model, arg);
            n++;
        }
        it = List_Next((void *)(grid + 0x80));
        e = it == 0 ? 0 : (char *)*it;
    }
}
