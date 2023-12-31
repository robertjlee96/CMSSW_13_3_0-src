import FWCore.ParameterSet.Config as cms

from RecoTracker.PixelSeeding.PixelTripletHLTGenerator_cfi import *
SeedGeneratorParameters = cms.PSet(
    ComponentName = cms.string('TSGFromOrderedHits'),
    OrderedHitsFactoryPSet = cms.PSet(
        ComponentName = cms.string('StandardHitTripletGenerator'),
        SeedingLayers = cms.InputTag('PixelLayerTriplets'),
        GeneratorPSet = cms.PSet(
            PixelTripletHLTGenerator
        )
    ),
    TTRHBuilder = cms.string('WithTrackAngle')
)


