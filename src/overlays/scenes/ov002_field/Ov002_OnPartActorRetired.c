/* Ov002_OnPartActorRetired: host retirement notification and probabilistic
 * replacement spawn. Preserve both RNG advances, including the discarded draw. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct ActorTransform {char pad0[0x78];int nSlotTableIndex;} ActorTransform;
typedef struct ActorXfmBlock {u32 aPrefixWords[4];VecFx32 vPosition;u32 aSuffixWords[4];} ActorXfmBlock;
typedef struct Ov002PartOwnerTagFields {u8 nGroupIndex,nSlotIndex;u16 wReserved;} Ov002PartOwnerTagFields;
typedef union Ov002PartOwnerTag {u32 raw;Ov002PartOwnerTagFields fields;} Ov002PartOwnerTag;
typedef struct Actor Actor;
struct Actor {
    char pad0[4];ActorTransform *pTransform;char pad8[0x98];ActorXfmBlock xfmBlock;
    char padcc[0xe8];u8 nPlayerIndex;char pad1b5[0xa7];Actor *pLastAttacker;
    char pad260[0x70];Ov002PartOwnerTag ownerTag;
};
typedef struct Ov002MarkerRow {
    u8 bId;u8 nKind:4,nHigh:4;char pad2[3];s8 nParam;char pad6[4];
    s16 nDropKey,nDropChance;s8 nDropEntry;char padf;
} Ov002MarkerRow;
typedef struct Ov002RecordGroup {s8 nRowIndex;u8 nLimit,nCount;s8 nLastPlayer;} Ov002RecordGroup;
typedef struct Ov002RecordEntryFlags {s8 nState:4,nValue:4;} Ov002RecordEntryFlags;
typedef struct Ov002RecordEntry {s8 nGroupCount;Ov002RecordEntryFlags bStateAndValue;char pad2[2];Ov002RecordGroup aGroups[1];} Ov002RecordEntry;
typedef struct Ov002RecordList {char pad0[4];Ov002MarkerRow *pRows;Ov002RecordEntry *apEntries[1];} Ov002RecordList;
typedef struct Ov002ObjectSlot {char pad0[0x14];s8 bEntryIndex;char pad15[3];} Ov002ObjectSlot;
typedef struct Ov002PartRetireCmd {u8 nKind,nEntryIndex,nGroupIndex;} Ov002PartRetireCmd;
typedef struct Ov002SessionSpawnCmd {
    u8 nKind;u8 nSlot:2,nStyle:6;u8 nEntry;s8 nKeyIndex;u8 nSeed;u8 pad5[3];VecFx32 vPos;
} Ov002SessionSpawnCmd;
typedef struct Ov002PendingObjectSpawn {
    s16 x,y,z;u8 nSlot:2,nStyle:6;u8 nEntry;s8 nKeyIndex;s8 bGuaranteed:1;
} Ov002PendingObjectSpawn;
typedef struct Ov002ObjectContext {
    char pad0[4];Ov002RecordList *pEntryList;char pad8[2];s16 nTrackedRowId,nRetiredCount;
    char pade[2];s8 bDefault;char pad11[0x33];Ov002ObjectSlot *pSlots;char pad48[0x16];
    u16 aPendingRetireTags[8];Ov002PendingObjectSpawn aPendingSpawns[4];
} Ov002ObjectContext;
extern Ov002ObjectContext *data_ov002_0207fa14;
extern s16 Session_GetLocalPlayerIndex(void);
extern int Session_IsActive(void);
extern u32 Ov002_BuildSessionCommand(int,void *);
extern void Ov002_TickSlotCooldown(int,int);
extern void Ov002_AddMissionTally(int,int,int);
extern u16 LoadGlobalU16At0(void);
extern int RandNext(void);
extern u32 Slot_EvalPackedParam(int,int);
extern int Ov002_TakeEntryOfKind1(void);
extern int Ov002_FindKeyEntryIndex(int);
extern void *Ov002_GetRootField8d14(int);
extern int Ov002_GetSlotTableByte(int);
extern int Session_RandNextScaled(int);

void Ov002_OnPartActorRetired(Actor *pActor)
{
    Ov002PartRetireCmd retireCmd;
    Ov002SessionSpawnCmd spawnCmd;
    Ov002ObjectContext *pCtx;
    Ov002RecordEntry *pRecord;
    Ov002RecordGroup *pGroup;
    Ov002MarkerRow *pRow;
    int nDropKey,nEntry,nKeyIndex;
    int i;
    pCtx=data_ov002_0207fa14;
    if(Session_GetLocalPlayerIndex()!=0)return;
    nDropKey=-1;
    retireCmd.nEntryIndex=pCtx->pSlots[(pActor->ownerTag.raw>>8)&0xff].bEntryIndex;
    retireCmd.nGroupIndex=pActor->ownerTag.raw&0xff;
    pRecord=pCtx->pEntryList->apEntries[retireCmd.nEntryIndex];
    pGroup=&pRecord->aGroups[retireCmd.nGroupIndex];
    pRow=&pCtx->pEntryList->pRows[pGroup->nRowIndex];
    pCtx->bDefault=-1;
    pRecord->bStateAndValue.nValue=-1;
    pGroup->nLastPlayer=-1;
    if(pActor->pLastAttacker){
        pCtx->bDefault=pActor->pLastAttacker->nPlayerIndex;
        pRecord->bStateAndValue.nValue=(s8)pActor->pLastAttacker->nPlayerIndex;
        pGroup->nLastPlayer=pActor->pLastAttacker->nPlayerIndex;
    }
    if(Session_IsActive()){
        if(Ov002_BuildSessionCommand(0xc,&retireCmd)==0xffff){
            u16 *pPending=0;
            for(i=0;i<8;i++)if(pCtx->aPendingRetireTags[i]==0xffff){pPending=&pCtx->aPendingRetireTags[i];break;}
            *pPending=(retireCmd.nEntryIndex<<8)|retireCmd.nGroupIndex;
        }
    }else Ov002_TickSlotCooldown(retireCmd.nEntryIndex,retireCmd.nGroupIndex);
    if(pCtx->nTrackedRowId!=-1 && pCtx->nTrackedRowId==pCtx->pEntryList->pRows[pGroup->nRowIndex].bId){
        pCtx->nRetiredCount++;
        Ov002_AddMissionTally(pCtx->bDefault,3,1);
    }
    if(LoadGlobalU16At0()!=0x2a){
        int nRoll=RandNext();
        s16 nChance=pRow->nDropChance;
        if(pCtx->bDefault!=-1 && Slot_EvalPackedParam(pCtx->bDefault,0x4e) && nChance>0){
            nChance+=0x19a;
            if(nChance>0x1000)nChance=0x1000;
        }
        if(pRow->nDropKey>=0 && nChance>0 && nRoll-nChance<=0)nDropKey=pRow->nDropKey;
    }
    if(nDropKey==-1)return;
    RandNext();
    nEntry=pRow->nDropEntry;
    if(nEntry==-1)nEntry=Ov002_TakeEntryOfKind1();
    nKeyIndex=Ov002_FindKeyEntryIndex((s16)nDropKey);
    Ov002_GetRootField8d14((s16)nKeyIndex);
    if(nEntry<0)return;
    spawnCmd.nEntry=nEntry;
    spawnCmd.nKeyIndex=nKeyIndex;
    spawnCmd.nStyle=(u8)Ov002_GetSlotTableByte(pActor->pTransform->nSlotTableIndex);
    spawnCmd.vPos=pActor->xfmBlock.vPosition;
    spawnCmd.nSlot=0;
    spawnCmd.nSeed=Session_RandNextScaled(0x100);
    if(Ov002_BuildSessionCommand(0x10,&spawnCmd)==0xffff){
        Ov002PendingObjectSpawn *pPending=0;
        for(i=0;i<4;i++)if(pCtx->aPendingSpawns[i].nKeyIndex==-1){pPending=&pCtx->aPendingSpawns[i];break;}
        if(!pPending && pRow->nDropChance==0x1000){
            for(i=0;i<4;i++)if(!pCtx->aPendingSpawns[i].bGuaranteed){pPending=&pCtx->aPendingSpawns[i];break;}
        }
        if(pPending){
            pPending->nSlot=spawnCmd.nSlot;
            pPending->nStyle=spawnCmd.nStyle;
            pPending->nEntry=spawnCmd.nEntry;
            pPending->nKeyIndex=spawnCmd.nKeyIndex;
            pPending->x=spawnCmd.vPos.x>>4;
            pPending->y=spawnCmd.vPos.y>>4;
            pPending->z=spawnCmd.vPos.z>>4;
            pPending->bGuaranteed=(s8)(pRow->nDropChance==0x1000);
        }
    }
}
