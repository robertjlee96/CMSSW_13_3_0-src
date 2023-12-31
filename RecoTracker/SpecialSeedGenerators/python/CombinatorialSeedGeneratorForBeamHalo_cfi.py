# The following comments couldn't be translated into the new config version:

#along momentum

#and opposite to momentum

import FWCore.ParameterSet.Config as cms

from RecoTracker.TkTrackingRegions.GlobalTrackingRegion_cfi import *
from RecoLocalTracker.SiStripClusterizer.SiStripClusterChargeCut_cfi import *

layerInfo = cms.PSet(
    TID = cms.PSet(
        matchedRecHits = cms.InputTag("siStripMatchedRecHits","matchedRecHit"),
        useRingSlector = cms.bool(False),
        TTRHBuilder = cms.string('WithTrackAngle'),
        clusterChargeCut = cms.PSet(refToPSet_ = cms.string('SiStripClusterChargeCutNone')),
        rphiRecHits = cms.InputTag("siStripMatchedRecHits","rphiRecHit")
    ),
    FPix = cms.PSet(
        TTRHBuilder = cms.string('TTRHBuilderWithoutAngle4PixelPairs'),
        HitProducer = cms.string('siPixelRecHits'),
    ),
    TEC = cms.PSet(#extend the seeds to inner rings of TEC
        minRing = cms.int32(1),
        matchedRecHits = cms.InputTag("siStripMatchedRecHits","matchedRecHit"),
        useRingSlector = cms.bool(True),
        TTRHBuilder = cms.string('WithTrackAngle'),
        clusterChargeCut = cms.PSet(refToPSet_ = cms.string('SiStripClusterChargeCutNone')),
        rphiRecHits = cms.InputTag("siStripMatchedRecHits","rphiRecHit"),
        maxRing = cms.int32(2)
    ),
    MTEC = cms.PSet(# re-use MTEC name for the outer rings
        minRing = cms.int32(5),
        matchedRecHits = cms.InputTag("siStripMatchedRecHits","matchedRecHit"),
        useRingSlector = cms.bool(False),
        TTRHBuilder = cms.string('WithTrackAngle'),
        clusterChargeCut = cms.PSet(refToPSet_ = cms.string('SiStripClusterChargeCutNone')),
        rphiRecHits = cms.InputTag("siStripMatchedRecHits","rphiRecHit"),
        maxRing = cms.int32(7)
    ),
)

layerList = [
    'FPix1_pos+FPix2_pos', 
    'FPix1_neg+FPix2_neg', 
    'TID2_pos+TID3_pos', 
    'TID2_neg+TID3_neg', 
    'TEC1_neg+TEC2_neg',
    'TEC1_pos+TEC2_pos',
    'TEC2_neg+TEC3_neg',
    'TEC2_pos+TEC3_pos',
    'TEC3_neg+TEC4_neg',
    'TEC3_pos+TEC4_pos',
    'TEC4_neg+TEC5_neg',
    'TEC4_pos+TEC5_pos',
    'TEC5_neg+TEC6_neg',
    'TEC5_pos+TEC6_pos',
    'MTEC7_neg+MTEC8_neg',
    'MTEC7_pos+MTEC8_pos',
    'MTEC8_neg+MTEC9_neg',
    'MTEC8_pos+MTEC9_pos'
    ]

beamhaloTrackerSeeds = cms.EDProducer("CtfSpecialSeedGenerator",
    SeedMomentum = cms.double(15.0), ##initial momentum in GeV !!!set to a lower value for slice test data

    ErrorRescaling = cms.double(50.0),
    RegionFactoryPSet = cms.PSet(
        RegionPSetBlock,
        ComponentName = cms.string('GlobalRegionProducer')
    ),
    Charges = cms.vint32(-1, 1),
    OrderedHitsFactoryPSets = cms.VPSet(
        cms.PSet(
            ComponentName = cms.string('BeamHaloPairGenerator'),
            maxTheta = cms.double(0.1),
            PropagationDirection = cms.string('alongMomentum'),
            NavigationDirection = cms.string('outsideIn'),
            LayerSrc = cms.InputTag("beamhaloTrackerSeedingLayers")
        ), 
        cms.PSet(
            ComponentName = cms.string('BeamHaloPairGenerator'),
            maxTheta = cms.double(0.1),
            PropagationDirection = cms.string('oppositeToMomentum'),
            NavigationDirection = cms.string('outsideIn'),
            LayerSrc = cms.InputTag("beamhaloTrackerSeedingLayers")
        )),
    UseScintillatorsConstraint = cms.bool(False),
    TTRHBuilder = cms.string('WithTrackAngle'),
    SeedsFromPositiveY = cms.bool(False),
    SeedsFromNegativeY = cms.bool(False),
    doClusterCheck = cms.bool(True),
    ClusterCollectionLabel = cms.InputTag("siStripClusters"),
    MaxNumberOfStripClusters = cms.uint32(10000),
    MaxNumberOfPixelClusters = cms.uint32(10000),
    PixelClusterCollectionLabel = cms.InputTag("siPixelClusters"),
    CheckHitsAreOnDifferentLayers = cms.bool(False),
    SetMomentum = cms.bool(True),
    requireBOFF = cms.bool(False),
    maxSeeds = cms.int32(10000),
)


