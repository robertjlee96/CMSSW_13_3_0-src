import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run2_2018_cff import Run2_2018
process = cms.Process("Sim",Run2_2018)

process.load("Configuration.Generator.QCD_Pt_3000_3500_cfi")
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load("IOMC.EventVertexGenerators.VtxSmearedGauss_cfi")
process.load('Configuration.Geometry.GeometryExtended2018Reco_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.Generator_cff')
process.load('Configuration.StandardSequences.SimIdeal_cff')
process.load("SimG4CMS.Calo.CaloSimHitStudy_cfi")

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = autoCond['run2_mc']

process.source = cms.Source("EmptySource")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(20)
)

if 'MessageLogger' in process.__dict__:
    process.MessageLogger.G4cerr=dict()

process.Timing = cms.Service("Timing")

process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",
    oncePerEventMode = cms.untracked.bool(True),
    showMallocInfo = cms.untracked.bool(True),
    dump = cms.untracked.bool(True),
    ignoreTotal = cms.untracked.int32(1)
)

process.load("IOMC.RandomEngine.IOMC_cff")
process.RandomNumberGeneratorService.generator.initialSeed = 456789
process.RandomNumberGeneratorService.g4SimHits.initialSeed = 9876
process.RandomNumberGeneratorService.VtxSmeared.initialSeed = 123456789
process.rndmStore = cms.EDProducer("RandomEngineStateProducer")

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('qcdpt_3000_3500_QGSP_FTFP_BERT_EML.root')
)

# Event output
process.output = cms.OutputModule("PoolOutputModule",
    process.FEVTSIMEventContent,
    fileName = cms.untracked.string('simevent_qcdpt_3000_3500_QGSP_FTFP_BERT_EML.root')
)
process.generation_step = cms.Path(process.pgen)
process.simulation_step = cms.Path(process.psim)
process.analysis_step   = cms.Path(process.CaloSimHitStudy)
process.out_step = cms.EndPath(process.output)

process.generator.pythiaHepMCVerbosity = False
process.generator.pythiaPylistVerbosity = 0
process.g4SimHits.Physics.type = 'SimG4Core/Physics/QGSP_FTFP_BERT_EML'

# process.g4SimHits.ECalSD.IgnoreTrackID      = True
# process.g4SimHits.HCalSD.IgnoreTrackID      = True
# process.g4SimHits.ECalSD.TimeSliceUnit      = 5
# process.g4SimHits.HCalSD.TimeSliceUnit      = 5

# process.g4SimHits.HCalSD.UseShowerLibrary   = True
# process.g4SimHits.HCalSD.UseParametrize     = False
# process.g4SimHits.HCalSD.UsePMTHits         = False
# process.g4SimHits.HCalSD.UseFibreBundleHits = False
# process.g4SimHits.HFShower.UseShowerLibrary = False
# process.g4SimHits.HFShower.UseHFGflash      = True
# process.g4SimHits.HFShower.TrackEM          = False
# process.g4SimHits.HFShower.OnlyLong         = True
# process.g4SimHits.HFShower.EminLibrary      = 0.0

# Schedule definition                                                          
process.schedule = cms.Schedule(process.generation_step,
                                process.simulation_step,
                                process.analysis_step,
                                process.out_step
                                )

# filter all path with the production filter sequence                          
for path in process.paths:
        getattr(process,path)._seq = process.generator * getattr(process,path)._seq
