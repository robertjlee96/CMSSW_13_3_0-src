import FWCore.ParameterSet.Config as cms

# AlCaReco for track based calibration using MinBias events
OutALCARECOAlCaPCCRandom_noDrop = cms.PSet(
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOAlCaPCCRandom')
    ),
    outputCommands = cms.untracked.vstring(
        'keep *_alcaPCCIntegratorRandom_alcaPCCRandom_*')
)



import copy
OutALCARECOAlCaPCCRandom=copy.deepcopy(OutALCARECOAlCaPCCRandom_noDrop)
OutALCARECOAlCaPCCRandom.outputCommands.insert(0,"drop *")
