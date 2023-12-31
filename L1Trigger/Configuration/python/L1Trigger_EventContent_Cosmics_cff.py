import FWCore.ParameterSet.Config as cms

# RAW content 
L1TriggerRAW = cms.PSet(
    outputCommands = cms.untracked.vstring(
    'keep *_cscTriggerPrimitiveDigis_*_*', 
    'keep *_dtTriggerPrimitiveDigis_*_*', 
    'keep *_rpcTriggerDigis_*_*', 
    'keep *_rctDigis_*_*', 
    'keep *_csctfDigis_*_*', 
    'keep *_csctfTrackDigis_*_*', 
    'keep *_dttfDigis_*_*', 
    'keep *_gctDigis_*_*', 
    'keep *_gmtDigis_*_*', 
    'keep *_gtDigis_*_*', 
    'keep *_gtEvmDigis_*_*', 
    'keep *_l1GtRecord_*_*', 
    'keep *_l1GtObjectMap_*_*', 
    'keep *_l1extraParticles_*_*')
)

# RECO content
L1TriggerRECO = cms.PSet(
    outputCommands = cms.untracked.vstring(
    'keep *_cscTriggerPrimitiveDigis_*_*', 
    'keep *_dtTriggerPrimitiveDigis_*_*', 
    'keep *_rpcTriggerDigis_*_*', 
    'keep *_rctDigis_*_*', 
    'keep *_csctfDigis_*_*', 
    'keep *_csctfTrackDigis_*_*', 
    'keep *_dttfDigis_*_*', 
    'keep *_gctDigis_*_*', 
    'keep *_gmtDigis_*_*', 
    'keep *_gtDigis_*_*', 
    'keep *_gtEvmDigis_*_*', 
    'keep *_l1GtRecord_*_*', 
    'keep *_l1GtTriggerMenuLite_*_*',
    'keep *_l1GtObjectMap_*_*', 
    'keep *_l1extraParticles_*_*',
    'keep *_l1L1GtObjectMap_*_*',
    'keep LumiDetails_lumiProducer_*_*',
    'keep LumiSummary_lumiProducer_*_*')
)

# AOD content
L1TriggerAOD = cms.PSet(
    outputCommands = cms.untracked.vstring(
    'keep *_gtDigis_*_*', 
    'keep *_l1GtRecord_*_*', 
    'keep *_l1GtTriggerMenuLite_*_*',
    'keep *_l1GtObjectMap_*_*', 
    'keep *_l1extraParticles_*_*',
    'keep *_l1L1GtObjectMap_*_*',
    'keep LumiSummary_lumiProducer_*_*')
)

L1TriggerFEVTDEBUG = cms.PSet(
    outputCommands = cms.untracked.vstring('keep *_simCscTriggerPrimitiveDigis_*_*', 
        'keep *_simDtTriggerPrimitiveDigis_*_*', 
        'keep *_simRpcTriggerDigis_*_*', 
        'keep *_simRctDigis_*_*', 
        'keep *_simCsctfDigis_*_*', 
        'keep *_simCsctfTrackDigis_*_*', 
        'keep *_simDttfDigis_*_*', 
        'keep *_simGctDigis_*_*', 
        'keep *_simGmtDigis_*_*', 
        'keep *_simGtDigis_*_*', 
        'keep *_cscTriggerPrimitiveDigis_*_*', 
        'keep *_dtTriggerPrimitiveDigis_*_*', 
        'keep *_rpcTriggerDigis_*_*', 
        'keep *_rctDigis_*_*', 
        'keep *_csctfDigis_*_*', 
        'keep *_csctfTrackDigis_*_*', 
        'keep *_dttfDigis_*_*', 
        'keep *_gctDigis_*_*', 
        'keep *_gmtDigis_*_*', 
        'keep *_gtDigis_*_*',
        'keep *_gtEvmDigis_*_*',
        'keep *_l1GtRecord_*_*', 
        'keep *_l1GtTriggerMenuLite_*_*',
        'keep *_l1extraParticles_*_*',
        'keep *_l1L1GtObjectMap_*_*',        
        'keep LumiDetails_lumiProducer_*_*',
        'keep LumiSummary_lumiProducer_*_*'       
        )
)

def _appendStage2Digis(obj):
    l1Stage2Digis = [
        'keep *_gtStage2Digis_*_*',
        'keep *_gmtStage2Digis_*_*',
        'keep *_caloStage2Digis_*_*',
        ]
    obj.outputCommands += l1Stage2Digis

# adding them to all places where we had l1extraParticles
from Configuration.Eras.Modifier_stage2L1Trigger_cff import stage2L1Trigger
stage2L1Trigger.toModify(L1TriggerRECO, func=_appendStage2Digis)
stage2L1Trigger.toModify(L1TriggerAOD, func=_appendStage2Digis)
stage2L1Trigger.toModify(L1TriggerFEVTDEBUG, func=_appendStage2Digis)
