/* Draws the calendar's rolling day digits. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct {
    unsigned char opaque000[0xa4];
    VecFx32 position;
    VecFx32 scale;
    unsigned char opaque0bc[0x48];
    u16 color;
    u16 opaque106;
} Ov004DigitGlyph;
typedef struct {
    int projection[4];
    int farPlane;
    VecFx32 target, pos, up;
} CamActor;
typedef struct {
    Ov004DigitGlyph digitGlyphs[10];
    int digitDrawCount;
    Ov004DigitGlyph *digitDrawGlyphs[6];
    VecFx32 digitDrawPositions[6];
    u16 digitDrawColors[6];
    CamActor digitCamera;
} Ov004Context;
extern Ov004Context *data_ov004_02051384;
extern void Camera_CommitMatricesEx(CamActor *actor, int top, int bottom, int left, int right);
extern void Widget_SetTagWord(Ov004DigitGlyph *glyph, u16 color);
extern void Scene_DrawNode(Ov004DigitGlyph *glyph);

void Ov004_DrawRollingDigits(void)
{
    int i;
    Ov004DigitGlyph *glyph;
    Camera_CommitMatricesEx(&data_ov004_02051384->digitCamera, 0x3b33, -0x3b33, -0x4d9a, 0x4d9a);
    for (i = 0; i < data_ov004_02051384->digitDrawCount; i++) {
        glyph = data_ov004_02051384->digitDrawGlyphs[i];
        glyph->scale.x = glyph->scale.y = glyph->scale.z = 0x5e3;
        data_ov004_02051384->digitDrawGlyphs[i]->position = data_ov004_02051384->digitDrawPositions[i];
        Widget_SetTagWord(data_ov004_02051384->digitDrawGlyphs[i], data_ov004_02051384->digitDrawColors[i]);
        Scene_DrawNode(data_ov004_02051384->digitDrawGlyphs[i]);
    }
}

