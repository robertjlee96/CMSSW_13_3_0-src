import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.Eras import eras

process = cms.Process('HcalGeometryTest',eras.Run3)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
#process.load("Geometry.HcalCommonData.testPhase0GeometryXML_cfi")
#process.load("Geometry.HcalCommonData.testPhase1GeometryXML_cfi")
#process.load("Geometry.HcalCommonData.hcalDDConstants_cff")
#process.load("Geometry.HcalEventSetup.hcalTopologyIdeal_cfi")
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.GeometrySimDB_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2022_realistic', '')


process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
    )

process.hga = cms.EDAnalyzer("HcalGeometryDetIdAnalyzer",
                             UseOldLoader      = cms.bool(False)
)

process.Timing = cms.Service("Timing")
process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck")

process.p1 = cms.Path(process.hga)
