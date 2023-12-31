import FWCore.ParameterSet.Config as cms

from RecoMuon.TrackingTools.MuonServiceProxy_cff import *
from RecoMuon.TrackingTools.MuonTrackLoader_cff import *
from RecoMuon.GlobalTrackingTools.GlobalTrajectoryBuilderCommon_cff import *
globalMuons = cms.EDProducer("GlobalMuonProducer",
    MuonServiceProxy,
    MuonTrackLoaderForGLB,
    GLBTrajBuilderParameters = cms.PSet(
        GlobalTrajectoryBuilderCommon
    ),
    TrackerCollectionLabel = cms.InputTag("generalTracks"),
    MuonCollectionLabel = cms.InputTag("standAloneMuons","UpdatedAtVtx"),
    VertexCollectionLabel = cms.InputTag("offlinePrimaryVertices"),
    selectHighPurity = cms.bool(False)
)

globalMuons.GLBTrajBuilderParameters.GlobalMuonTrackMatcher.Propagator = 'SmartPropagatorRK'
globalMuons.GLBTrajBuilderParameters.TrackTransformer.Propagator = cms.string('SmartPropagatorAnyRK')

# FastSim has no template fit on tracker hits
# FastSim doesn't use Runge Kute for propagation
from Configuration.Eras.Modifier_fastSim_cff import fastSim
fastSim.toModify(globalMuons,
                 GLBTrajBuilderParameters =dict(GlbRefitterParameters = dict(TrackerRecHitBuilder = 'WithoutRefit',
                                                                             Propagator = 'SmartPropagatorAny'),
                                                TrackerRecHitBuilder = 'WithoutRefit',
                                                TrackTransformer = dict(TrackerRecHitBuilder = 'WithoutRefit',
                                                                        Propagator = 'SmartPropagatorAny'),
                                                GlobalMuonTrackMatcher = dict(Propagator = 'SmartPropagator') 
                                                )
)

from Configuration.ProcessModifiers.pp_on_AA_cff import pp_on_AA
pp_on_AA.toModify(globalMuons, selectHighPurity = True)
