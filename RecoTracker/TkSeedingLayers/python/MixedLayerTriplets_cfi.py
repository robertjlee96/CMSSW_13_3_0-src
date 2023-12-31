import FWCore.ParameterSet.Config as cms

# This is standard pixel-triplet seeding, but making use of TEC disks
# in forward region to boost acceptance.

from RecoTracker.TkSeedingLayers.seedingLayersEDProducer_cfi import *

MixedLayerTriplets = seedingLayersEDProducer.clone(
    layerList = ['BPix1+BPix2+BPix3', 
                 'BPix1+BPix2+FPix1_pos', 
                 'BPix1+BPix2+FPix1_neg', 
                 'BPix1+FPix1_pos+FPix2_pos', 
                 'BPix1+FPix1_neg+FPix2_neg', 
                 'BPix1+BPix2+TIB1', 
                 'BPix1+BPix3+TIB1', 
                 'BPix2+BPix3+TIB1', 
                 'BPix1+FPix1_pos+TID1_pos', 
                 'BPix1+FPix1_neg+TID1_neg', 
                 'BPix1+FPix1_pos+TID2_pos', 
                 'BPix1+FPix1_neg+TID2_neg', 
                 'FPix1_pos+FPix2_pos+TEC1_pos', 
                 'FPix1_neg+FPix2_neg+TEC1_neg', 
                 'FPix1_pos+FPix2_pos+TEC2_pos', 
                 'FPix1_neg+FPix2_neg+TEC2_neg'],
    TEC = dict(
	matchedRecHits = cms.InputTag("siStripMatchedRecHits","matchedRecHit"),
        TTRHBuilder = cms.string('WithTrackAngle'),
	clusterChargeCut = cms.PSet(refToPSet_ = cms.string('SiStripClusterChargeCutNone'))
    ),
    FPix = dict(
        TTRHBuilder = cms.string('WithTrackAngle'),
        HitProducer = cms.string('siPixelRecHits'),
    ),
    TID = dict(
        matchedRecHits = cms.InputTag("siStripMatchedRecHits","matchedRecHit"),
        TTRHBuilder = cms.string('WithTrackAngle'),
        clusterChargeCut = cms.PSet(refToPSet_ = cms.string('SiStripClusterChargeCutNone'))
    ),
    BPix = dict(
        TTRHBuilder = cms.string('WithTrackAngle'),
        HitProducer = cms.string('siPixelRecHits'),
    ),
    TIB = dict(
        matchedRecHits = cms.InputTag("siStripMatchedRecHits","matchedRecHit"),
        TTRHBuilder = cms.string('WithTrackAngle'),
        clusterChargeCut = cms.PSet(refToPSet_ = cms.string('SiStripClusterChargeCutNone'))
    )
)
