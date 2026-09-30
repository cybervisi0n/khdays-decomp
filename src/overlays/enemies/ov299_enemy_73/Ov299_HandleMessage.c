/* Message handler of the ov299 enemy. A "spawned" message (kind 5) carries a packed 24-bit
 * position in bytes 5..13 which becomes the translation of a fresh transform; sub-kinds 0/1/2
 * attach the matching +0x394 part model to it as the +0x398 effect (kind 0x15, or 0x17 for
 * parts 1/2) under the +0x3c owner, and part 1 spawning within 0xa000 of the player (the
 * manager's first actor +0x88) fires effect 1 on the actor. The base handler always runs. */

#include "nitro/fx_types.h"

typedef struct { int w[11]; } SrtTransform;

extern void SrtTransform_SetIdentity(SrtTransform *transform);
extern void Srt_SetTranslation(SrtTransform *transform, const VecFx32 *translation);
extern int Ov107_CreateNodeXformTask(int model, int parent, int kind, int zero,
                                     SrtTransform *transform);
extern int *Ov107_GetActorManager(void);
extern void VEC_Subtract(const void *a, const VecFx32 *b, VecFx32 *d);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void Ov107_ForwardVisibleEvent(int owner, int effect);
extern void Ov107_AiState_OnMessage(int owner, unsigned char *command, int arg);

void Ov299_HandleMessage(int owner, unsigned char *command, int arg)
{
    SrtTransform transform;
    VecFx32 translation;
    VecFx32 d;
    union {
        int words[3];
        unsigned char bytes[12];
    } packed;
    int *manager;

    if (command[2] == 5) {
        packed.bytes[3] = command[5];
        packed.bytes[2] = command[6];
        packed.bytes[1] = command[7];
        translation.x = packed.words[0] >> 8;
        packed.bytes[7] = command[8];
        packed.bytes[6] = command[9];
        packed.bytes[5] = command[0xa];
        translation.y = packed.words[1] >> 8;
        packed.bytes[11] = command[0xb];
        packed.bytes[10] = command[0xc];
        packed.bytes[9] = command[0xd];
        translation.z = packed.words[2] >> 8;
        SrtTransform_SetIdentity(&transform);
        Srt_SetTranslation(&transform, &translation);
        switch (command[3]) {
        case 0: case 1: case 2:
            *(int *)(owner + command[3] * 8 + 0x398) = Ov107_CreateNodeXformTask(
                    *(int *)(owner + 0x3c), *(int *)(owner + command[3] * 8 + 0x394),
                    (unsigned char)((command[3] != 0 ? 2 : 0) | 0x15), 0, &transform);
            if (command[3] == 1) {
                manager = Ov107_GetActorManager();
                if (manager != 0 && *manager != 0) {
                    VEC_Subtract((void *)(*manager + 0x88), &translation, &d);
                    if (VEC_Normalize(&d, &d) <= 0xa000) {
                        Ov107_ForwardVisibleEvent(owner, 1);
                    }
                }
            }
            break;
        }
    }
    Ov107_AiState_OnMessage(owner, command, arg);
}
