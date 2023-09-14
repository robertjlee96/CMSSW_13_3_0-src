import FWCore.ParameterSet.Config as cms

ticlSeedingL1 = cms.EDProducer("TICLSeedingRegionProducer",
    seedingPSet = cms.PSet(
        algo_verbosity = cms.int32(0),
        endcapScalings = cms.vdouble(3.17445, 1.13219, 0.0),
        l1TkEmColl = cms.InputTag("l1tLayer1EG","L1TkEmEE"),
        maxAbsEta = cms.double(4.0),
        minAbsEta = cms.double(1.3),
        minPt = cms.double(5.0),
        quality = cms.int32(4),
        qualityIsMask = cms.bool(True),
        type = cms.string('SeedingRegionByL1')
    )
)
