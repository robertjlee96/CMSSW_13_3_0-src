import FWCore.ParameterSet.Config as cms

generalV0Candidates = cms.EDProducer("V0Producer",
                                     
   # which beamSpot to reference
   beamSpot = cms.InputTag('offlineBeamSpot'),

   # reference primary vertex instead of beamSpot
   useVertex = cms.bool(False),
   # which vertex collection to use
   vertices = cms.InputTag('offlinePrimaryVertices'),

   # which TrackCollection to use for vertexing
   trackRecoAlgorithm = cms.InputTag('generalTracks'),

   # which V0s to reconstruct
   doKShorts = cms.bool(True),
   doLambdas = cms.bool(True),

   # perform vertex fit (if false, the POCA is used in lieu of the point
   # resulting from a fit)
   doFit = cms.bool(True),

   # which vertex fitting algorithm to use
   # True -> KalmanVertexFitter (recommended)
   # False -> AdaptiveVertexFitter (not recommended)
   vertexFitter = cms.bool(True),

   # use the refitted tracks returned from the KVF for V0Candidate kinematics
   # this is automatically set to False if using the AdaptiveVertexFitter
   useRefTracks = cms.bool(True),

   # -- cuts on initial track collection --
   # Track normalized Chi2 <
   tkChi2Cut = cms.double(10.),
   # Number of valid hits on track >=
   tkNHitsCut = cms.int32(3),
   # Pt of track >
   tkPtCut = cms.double(0.35),
   # Track impact parameter significance >
   tkIPSigXYCut = cms.double(2.),
   tkIPSigZCut = cms.double(-1.),

   # -- cuts on the vertex --
   # Vertex chi2 <
   vtxChi2Cut = cms.double(6.63),
   # XY decay distance significance >
   vtxDecaySigXYCut = cms.double(15.),
   # XY decay distance >
   vtxDecayXYCut = cms.double(-1.),
   # XY decay distance for same-sign vertices >
   ssVtxDecayXYCut = cms.double(-1.),
   # XYZ decay distance significance >
   vtxDecaySigXYZCut = cms.double(-1.),

   # -- miscellaneous cuts --
   # allow same-sign pairs of tracks
   allowSS = cms.bool(False),
   # Threshold for inner/outer DCA cuts:
   #    inner tracks = distance between POCA and z-axis <
   #    outer tracks = distance between POCA and z-axis >=
   innerOuterTkDCAThreshold = cms.double(5.),
   # POCA distance between inner tracks <
   innerTkDCACut = cms.double(1.),
   # POCA distance between outer tracks <
   outerTkDCACut = cms.double(1.),
   # allow vertices where the angle between the tracks is more than 90 degrees
   allowWideAngleVtx = cms.bool(False),
   # invariant mass of track pair - assuming both tracks are charged pions <
   mPiPiCut = cms.double(0.6),
   # check if either track has a hit radially inside the vertex position minus this number times the sigma of the vertex fit
   # note: Set this to -1 to disable this cut, which MUST be done if you want to run V0Producer on the AOD track collection!
   innerHitPosCut = cms.double(4.),
   # cos(angleXY) between x and p of V0 candidate >
   cosThetaXYCut = cms.double(0.998),
   # cos(angleXYZ) between x and p of V0 candidate >
   cosThetaXYZCut = cms.double(-2.),

   # -- cuts on the V0 candidate mass --
   # V0 mass window +- pdg value
   kShortMassCut = cms.double(0.07),
   lambdaMassCut = cms.double(0.05)

)

