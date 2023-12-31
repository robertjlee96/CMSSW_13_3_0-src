#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"
#include "DataFormats/RecoCandidate/interface/IsoDepositDirection.h"
#include "DataFormats/TauReco/interface/PFTau.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "PhysicsTools/IsolationAlgos/interface/IsoDepositExtractor.h"

class PFTauExtractor : public reco::isodeposit::IsoDepositExtractor {
public:
  explicit PFTauExtractor(const edm::ParameterSet&, edm::ConsumesCollector&& iC);
  ~PFTauExtractor() override {}

  /// definition of pure virtual functions inherited from IsoDepositExtractor base-class
  void fillVetos(const edm::Event&, const edm::EventSetup&, const reco::TrackCollection&) override {}
  reco::IsoDeposit deposit(const edm::Event& evt, const edm::EventSetup& es, const reco::Track& track) const override {
    return depositFromObject(evt, es, track);
  }
  reco::IsoDeposit deposit(const edm::Event& evt,
                           const edm::EventSetup& es,
                           const reco::Candidate& candidate) const override {
    return depositFromObject(evt, es, candidate);
  }

private:
  /// configuration parameters
  edm::EDGetTokenT<reco::PFTauCollection> tauSourceToken_;
  edm::EDGetTokenT<edm::View<reco::Candidate> > candidateSourceToken_;
  double maxDxyTrack_;
  double maxDzTrack_;
  double dRmatchPFTau_;
  double dRVetoCone_;
  double dRIsoCone_;
  double dRvetoPFTauSignalConeConstituents_;

  /// private member function for computing the IsoDeposits
  /// in case of reco::Track as well as in case of reco::Canididate input
  template <typename T>
  reco::IsoDeposit depositFromObject(const edm::Event&, const edm::EventSetup&, const T&) const;
};

PFTauExtractor::PFTauExtractor(const edm::ParameterSet& cfg, edm::ConsumesCollector&& iC) {
  tauSourceToken_ = iC.consumes<reco::PFTauCollection>(cfg.getParameter<edm::InputTag>("tauSource"));
  candidateSourceToken_ =
      iC.mayConsume<edm::View<reco::Candidate> >(cfg.getParameter<edm::InputTag>("candidateSource"));
  maxDxyTrack_ = cfg.getParameter<double>("Diff_r");
  maxDzTrack_ = cfg.getParameter<double>("Diff_z");
  dRmatchPFTau_ = cfg.getParameter<double>("dRmatchPFTau");
  dRVetoCone_ = cfg.getParameter<double>("DR_Veto");
  dRIsoCone_ = cfg.getParameter<double>("DR_Max");
  dRvetoPFTauSignalConeConstituents_ = cfg.getParameter<double>("dRvetoPFTauSignalConeConstituents");
}

template <typename T>
reco::IsoDeposit PFTauExtractor::depositFromObject(const edm::Event& evt,
                                                   const edm::EventSetup& es,
                                                   const T& tauCandidate) const {
  //--- create IsoDeposit to be returned;
  //    set "direction" of IsoDeposit cone to jet-axis of tauCandidate
  reco::isodeposit::Direction tauCandidateDirection(tauCandidate.eta(), tauCandidate.phi());
  reco::IsoDeposit isoDeposit(tauCandidateDirection);
  isoDeposit.addCandEnergy(tauCandidate.pt());

  //--- find PFTau closest to tauDirection
  edm::Handle<reco::PFTauCollection> pfTaus;
  evt.getByToken(tauSourceToken_, pfTaus);

  double dR_min = -1.;
  const reco::PFTau* pfTau_matched = nullptr;
  for (reco::PFTauCollection::const_iterator pfTau = pfTaus->begin(); pfTau != pfTaus->end(); ++pfTau) {
    double dR = deltaR(pfTau->eta(), pfTau->phi(), tauCandidate.eta(), tauCandidate.phi());
    if (pfTau_matched == nullptr || dR < dR_min) {
      dR_min = dR;
      pfTau_matched = &(*pfTau);
    }
  }

  //--- compute IsoDeposit for matched PFTau
  if (pfTau_matched != nullptr && dR_min < dRmatchPFTau_) {
    edm::Handle<edm::View<reco::Candidate> > candidates;
    evt.getByToken(candidateSourceToken_, candidates);

    const reco::Particle::Point& tauVertex = pfTau_matched->vertex();
    double dRsignalCone_max = 0.;
    for (edm::View<reco::Candidate>::const_iterator candidate = candidates->begin(); candidate != candidates->end();
         ++candidate) {
      double dR = deltaR(candidate->momentum(), pfTau_matched->momentum());

      //--- check that candidate is inbetween veto and isolation cone,
      //    and is compatible with originating from the same primary event vertex as the PFTau
      if (dR > dRVetoCone_ && dR < dRIsoCone_ && (candidate->vertex() - tauVertex).Rho() < maxDxyTrack_ &&
          fabs(candidate->vertex().z() - tauVertex.z()) < maxDzTrack_) {
        //--- check that the candidate is not associated to one of the tau decay products
        //    within the signal cone of the PFTau
        bool isSignalCone = false;
        for (std::vector<reco::CandidatePtr>::const_iterator tauSignalConeConstituent =
                 pfTau_matched->signalCands().begin();
             tauSignalConeConstituent != pfTau_matched->signalCands().end();
             ++tauSignalConeConstituent) {
          double dR = deltaR(candidate->momentum(), (*tauSignalConeConstituent)->momentum());
          if (dR <= dRvetoPFTauSignalConeConstituents_)
            isSignalCone = true;
        }

        if (!isSignalCone) {
          reco::isodeposit::Direction candidateDirection(candidate->eta(), candidate->phi());
          isoDeposit.addDeposit(candidateDirection, candidate->pt());
        }
      }
    }

    //--- set size of veto cone of IsoDeposit to largest distance
    //    of any tau decay product within the signal cone of the PFTau
    //    (add a small positive number in order to avoid issues
    //     with rounding errors and "<" versus "<=" comparisson)
    reco::IsoDeposit::Veto isoDepositVeto;
    isoDepositVeto.vetoDir = tauCandidateDirection;
    isoDepositVeto.dR = dRsignalCone_max + 1.e-3;
    isoDeposit.setVeto(isoDepositVeto);
  } else {
    edm::LogWarning("PFTauExtractor::depositFromObject")
        << " Failed to match PFTau to tauCandidate direction given by"
        << " eta = " << tauCandidate.eta() << ", phi = " << tauCandidate.phi()
        << " --> skipping computation of IsoDeposit !!";
  }

  return isoDeposit;
}

#include "FWCore/Framework/interface/MakerMacros.h"

#include "PhysicsTools/IsolationAlgos/interface/IsoDepositExtractorFactory.h"

DEFINE_EDM_PLUGIN(IsoDepositExtractorFactory, PFTauExtractor, "PFTauExtractor");
