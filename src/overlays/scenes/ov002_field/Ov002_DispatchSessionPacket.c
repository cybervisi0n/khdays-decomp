/* Ov002_DispatchSessionPacket: channel 7 receive callback. */

#include "nitro/fx_types.h"

typedef struct Ov002ForwardPacketFlags {
    unsigned char nKind : 2;
    unsigned char nIndex : 6;
} Ov002ForwardPacketFlags;
typedef struct Ov002PeerReadyPacketFlags {
    unsigned char nSlot : 3;
    unsigned char bReady : 1;
    unsigned char nReserved : 4;
} Ov002PeerReadyPacketFlags;
typedef struct Ov002SessionPacketHeader {
    unsigned char nOp, nArg1, nArg2, nArg3;
    int nValue;
} Ov002SessionPacketHeader;
typedef struct Ov002ForwardRecordPacket {
    unsigned char nOp;
    Ov002ForwardPacketFlags flags;
    unsigned char nOwner;
    signed char nRecordKey;
    unsigned char nParam, pad005[3];
    VecFx32 vPosition;
} Ov002ForwardRecordPacket;
typedef struct Ov002SpotSpawnPacket {
    unsigned char nOp, nArg2, nKind, nArg1;
    VecFx32 vPosition;
} Ov002SpotSpawnPacket;
typedef struct Ov002PeerReadyPacket {
    unsigned char nOp;
    Ov002PeerReadyPacketFlags flags;
} Ov002PeerReadyPacket;
typedef union Ov002SessionPacket {
    Ov002SessionPacketHeader header;
    Ov002ForwardRecordPacket forward;
    Ov002SpotSpawnPacket spot;
    Ov002PeerReadyPacket ready;
} Ov002SessionPacket;
extern void Ov002_ClaimEventHandle(int nId);
extern void Ov002_HandleSeatMessage(void *pPacket);
extern void Ov002_PublishSlotDay(int nSlot, int nDay);
extern void Ov002_ApplyPeerRosterSlot(void *pPacket);
extern void Ov002_ApplyTimerCommand(int nKind, int nDelta);
extern void Ov002_FillSeatBoardLocally(void *pPacket);
extern void Ov002_ApplyPeerSeatValues(void *pPacket);
extern void Ov002_NotePeerAck(void *pPacket);
extern void Ov002_TickSlotCooldown(int nSlot, int nIndex);
extern void Ov002_RebuildObjectEventParts(int nSlot);
extern void Ov002_PublishValue(int nValue);
extern void Ov002_SetPendingSlotFlag(int nBit);
extern void Ov002_ForwardRecordFields(int nOwner, int nKey, int nIndex, VecFx32 *pPos, signed char nKind, unsigned char nParam);
extern void *Ov002_SpawnKindIntoFreeSpot(int nKind, int nArg1, int nArg2, VecFx32 *pPos, int nMode);
extern void Ov002_ForwardLinkRequest(void *pPacket);
extern void Ov002_UpdateLocalPeerReadyNotice(unsigned int nSlot, int bReady);
extern void Ov002_HandleSpawnRequest(void *pPacket);
extern void Ov002_ApplyPackedTallyMessage(void *pPacket, unsigned int nPayloadBytes);
extern void Ov002_RecordAck(void *pPacket);
extern void Ov002_RaiseRequestFlag(void *pPacket);
extern void Ov002_RecordLinkEvent(int nEvent, int nPeer);

void Ov002_DispatchSessionPacket(Ov002SessionPacket *pPacket, unsigned int nPayloadBytes)
{
    switch (pPacket->header.nOp) {
    case 0: break;
    case 1: Ov002_ClaimEventHandle(pPacket->header.nValue); break;
    case 2:
    case 3:
    case 4: Ov002_HandleSeatMessage(pPacket); break;
    case 5: Ov002_PublishSlotDay(pPacket->header.nArg1, pPacket->header.nArg2); break;
    case 6: Ov002_ApplyPeerRosterSlot(pPacket); break;
    case 7: Ov002_ApplyTimerCommand(pPacket->header.nArg1, pPacket->header.nValue); break;
    case 8: Ov002_FillSeatBoardLocally(pPacket); break;
    case 9: Ov002_ApplyPeerSeatValues(pPacket); break;
    case 18: Ov002_NotePeerAck(pPacket); break;
    case 12: Ov002_TickSlotCooldown(pPacket->header.nArg1, pPacket->header.nArg2); break;
    case 13: Ov002_RebuildObjectEventParts(pPacket->header.nArg1); break;
    case 14: Ov002_PublishValue(pPacket->header.nArg1); break;
    case 15: Ov002_SetPendingSlotFlag(pPacket->header.nArg1); break;
    case 16:
        Ov002_ForwardRecordFields(pPacket->forward.nOwner, pPacket->forward.nRecordKey,
            pPacket->forward.flags.nIndex, &pPacket->forward.vPosition,
            (signed char)pPacket->forward.flags.nKind, pPacket->forward.nParam);
        break;
    case 17:
        Ov002_SpawnKindIntoFreeSpot(pPacket->spot.nKind, pPacket->spot.nArg1,
            pPacket->spot.nArg2, &pPacket->spot.vPosition, 3);
        break;
    case 20:
    case 21: Ov002_ForwardLinkRequest(pPacket); break;
    case 22:
        Ov002_UpdateLocalPeerReadyNotice(pPacket->ready.flags.nSlot, (signed char)pPacket->ready.flags.bReady);
        break;
    case 23:
    case 24: Ov002_HandleSpawnRequest(pPacket); break;
    case 10: Ov002_ApplyPackedTallyMessage(pPacket, nPayloadBytes); break;
    case 11: Ov002_RecordAck(pPacket); break;
    case 19: Ov002_RaiseRequestFlag(pPacket); break;
    case 25: Ov002_RecordLinkEvent(25, pPacket->header.nArg1); break;
    case 26: Ov002_RecordLinkEvent(26, 255); break;
    }
}
