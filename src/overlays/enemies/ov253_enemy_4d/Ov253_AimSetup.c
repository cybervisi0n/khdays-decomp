/* Ov253_AimSetup -- aim setup: takes the scene camera's look direction (+0x20 minus
 * +0x14), keeps its length at +8 and builds the +0xc rotation to face it from data_02042258;
 * the +4 item's +0x74 position is kept at +0x20, the rotation's forward scaled by the length
 * gives an aim point kept at least 1.0 above the +0x24 height, which is sent as event 4 to the
 * owner's +0x74 handler when present; the +0x1c timer clears and slots 1 / 2 take 020d1d28 /
 * 020d1cf0. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern int func_ov022_02083f0c(void);
extern int Ov002_GetWord20(int handle);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Quat_FromTwoVectors(void *rotation, const VecFx32 *from, const VecFx32 *to);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042258;
extern void Ov253_AimFollow(void);
extern void Ov253_ForwardEvent4(void);

void Ov253_AimSetup(int *node) {
    int *state = (int *)node[1];
    VecFx32 dir;
    VecFx32 aim;
    VecFx32 fwd;
    int cam;
    int obj;
    int arg;

    cam = Ov002_GetWord20(func_ov022_02083f0c());
    VEC_Subtract((VecFx32 *)(cam + 0x20), (VecFx32 *)(cam + 0x14), &dir);
    state[2] = VEC_Normalize(&dir, &dir);
    Quat_FromTwoVectors((void *)(state + 3), &data_02042258, &dir);
    *(VecFx32 *)(state + 8) = *(VecFx32 *)(state[1] + 0x74);
    Vec3TransformViaTempMtx(&fwd, (void *)(state + 3), &data_02042258);
    ScaleVec3Fx12(state[2], &fwd, &aim);
    VEC_Add(&aim, (VecFx32 *)(state + 8), &aim);
    if (aim.y < state[9] + 0x1000) {
        aim.y = state[9] + 0x1000;
    }
    if (*(int *)(Ov107_GetActorManager() + 0x74) != 0) {
        obj = Ov107_GetActorManager();
        arg = func_ov022_02083f0c();
        (*(void (**)(int, int, VecFx32 *))(obj + 0x74))(arg, 4, (VecFx32 *)(state + 8));
    }
    state[7] = 0;
    SetIndexedSlot(node, 1, Ov253_AimFollow);
    SetIndexedSlot(node, 2, Ov253_ForwardEvent4);
}
