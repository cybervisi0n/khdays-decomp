/*
 * Ov282_ReaimEmitterCone -- x3 (ov210/...). Tick the sub-emitter and re-aim its spawn cone.
 * If a child scene exists at +0x3c, tick it (0203c4a8); tick the emitter (020c41e4). Skip the rest
 * unless one of the three aim words +0x124/+0x128/+0x12c is set. Build the aim matrix into `m` via
 * 0202ed60(&m, &data_02042264, self+0x124). Take the muzzle offset (self+0x74..0x7c), pull its Y in by
 * the recoil +0x13c, and drive the emitter node at *(self+0x190)+0x30: set its cone via
 * 0203ca14(node, x, y+0x100, z), its length via 0203ca9c(node, 0x1000), and its orientation via
 * 0203c9d0(node, &m).
 */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct xform4 { int w[4]; };

extern void ObjList_Update(int scene, unsigned tick);
extern void Quat_FromTwoVectors(struct xform4 *out, void *basis, int *vec);
extern void Srt_SetTranslationXYZ(int node, int x, int y, int z);
extern void Srt_SetScaleUniform(int node, int len);
extern void Srt_SetRotationQuat(int node, struct xform4 *m);
extern int data_02042264;

void Ov282_ReaimEmitterCone(int self, unsigned tick) {
    if (*(int *)(self + 0x3c) != 0) {
        ObjList_Update(*(int *)(self + 0x3c), tick);
    }
    Ov107_AiState_ResolveContacts((Actor *)self, tick);
    if (*(int *)(self + 0x124) == 0 && *(int *)(self + 0x128) == 0 && *(int *)(self + 0x12c) == 0) {
        return;
    }
    {
        int recoil = *(int *)(self + 0x13c);
        VecFx32 s;
        struct xform4 m;

        Quat_FromTwoVectors(&m, &data_02042264, (int *)(self + 0x124));
        s = *(VecFx32 *)(self + 0x74);
        s.y -= recoil;
        Srt_SetTranslationXYZ(*(int *)(self + 0x190) + 0x30, s.x, s.y + 0x100, s.z);
        Srt_SetScaleUniform(*(int *)(self + 0x190) + 0x30, 0x1000);
        Srt_SetRotationQuat(*(int *)(self + 0x190) + 0x30, &m);
    }
}
