import FWCore.ParameterSet.Config as cms

trackMCMatch = cms.EDProducer("MCTrackMatcher",
    trackingParticles = cms.InputTag("mix","MergedTrackTruth"),
    tracks = cms.InputTag("generalTracks"),
    genParticles = cms.InputTag("genParticles"),
    associator = cms.string('trackAssociatorByHits'),
    throwOnMissingTPCollection = cms.bool(True)
)


