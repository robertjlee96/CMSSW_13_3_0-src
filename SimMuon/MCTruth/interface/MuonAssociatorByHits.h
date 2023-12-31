#ifndef MuonAssociatorByHits_h
#define MuonAssociatorByHits_h

#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"

#include "SimMuon/MCTruth/interface/MuonAssociatorByHitsHelper.h"
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"
#include "SimMuon/MCTruth/interface/GEMHitAssociator.h"
#include "SimMuon/MCTruth/interface/RPCHitAssociator.h"
#include "SimMuon/MCTruth/interface/CSCHitAssociator.h"
#include "SimMuon/MCTruth/interface/DTHitAssociator.h"

#include <memory>

namespace muonAssociatorByHitsDiagnostics {
  class InputDumper;
}

class TrackerTopologyRcd;

class MuonAssociatorByHits {
public:
  MuonAssociatorByHits(const edm::ParameterSet &conf, edm::ConsumesCollector &&iC);
  virtual ~MuonAssociatorByHits();

  // Originally from TrackAssociatorBase from where this class used to inherit
  // from
  reco::RecoToSimCollection associateRecoToSim(edm::Handle<edm::View<reco::Track>> &tCH,
                                               edm::Handle<TrackingParticleCollection> &tPCH,
                                               const edm::Event *event,
                                               const edm::EventSetup *setup) const {
    edm::RefToBaseVector<reco::Track> tc;
    for (unsigned int j = 0; j < tCH->size(); j++)
      tc.push_back(tCH->refAt(j));

    edm::RefVector<TrackingParticleCollection> tpc(tPCH.id());
    for (unsigned int j = 0; j < tPCH->size(); j++)
      tpc.push_back(edm::Ref<TrackingParticleCollection>(tPCH, j));

    return associateRecoToSim(tc, tpc, event, setup);
  }

  virtual reco::SimToRecoCollection associateSimToReco(edm::Handle<edm::View<reco::Track>> &tCH,
                                                       edm::Handle<TrackingParticleCollection> &tPCH,
                                                       const edm::Event *event,
                                                       const edm::EventSetup *setup) const {
    edm::RefToBaseVector<reco::Track> tc;
    for (unsigned int j = 0; j < tCH->size(); j++)
      tc.push_back(tCH->refAt(j));

    edm::RefVector<TrackingParticleCollection> tpc(tPCH.id());
    for (unsigned int j = 0; j < tPCH->size(); j++)
      tpc.push_back(edm::Ref<TrackingParticleCollection>(tPCH, j));

    return associateSimToReco(tc, tpc, event, setup);
  }

  /* Associate SimTracks to RecoTracks By Hits */
  /// Association Reco To Sim with Collections
  reco::RecoToSimCollection associateRecoToSim(const edm::RefToBaseVector<reco::Track> &,
                                               const edm::RefVector<TrackingParticleCollection> &,
                                               const edm::Event *event = nullptr,
                                               const edm::EventSetup *setup = nullptr) const;

  /// Association Sim To Reco with Collections
  reco::SimToRecoCollection associateSimToReco(const edm::RefToBaseVector<reco::Track> &,
                                               const edm::RefVector<TrackingParticleCollection> &,
                                               const edm::Event *event = nullptr,
                                               const edm::EventSetup *setup = nullptr) const;

private:
  MuonAssociatorByHitsHelper helper_;
  TrackerHitAssociator::Config trackerHitAssociatorConfig_;
  GEMHitAssociator::Config gemHitAssociatorConfig_;
  RPCHitAssociator::Config rpcHitAssociatorConfig_;
  CSCHitAssociator::Config cscHitAssociatorConfig_;
  DTHitAssociator::Config dtHitAssociatorConfig_;

  const edm::ESGetToken<TrackerTopology, TrackerTopologyRcd> ttopoToken_;
  std::unique_ptr<muonAssociatorByHitsDiagnostics::InputDumper> diagnostics_;
};

#endif
