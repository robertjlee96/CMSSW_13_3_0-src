#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ValidatedPluginMacros.h"

#include "RecoMuon/L3TrackFinder/interface/MuonCkfTrajectoryBuilder.h"
#include "RecoTracker/CkfPattern/interface/BaseCkfTrajectoryBuilderFactory.h"
#include "RecoMuon/L3TrackFinder/interface/HLTMuonL2SelectorForL3IO.h"

DEFINE_EDM_VALIDATED_PLUGIN(BaseCkfTrajectoryBuilderFactory, MuonCkfTrajectoryBuilder, "MuonCkfTrajectoryBuilder");
DEFINE_FWK_MODULE(HLTMuonL2SelectorForL3IO);
