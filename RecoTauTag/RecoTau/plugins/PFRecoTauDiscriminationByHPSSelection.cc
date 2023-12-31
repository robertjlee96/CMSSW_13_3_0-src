#include "RecoTauTag/RecoTau/interface/TauDiscriminationProducerBase.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include <FWCore/ParameterSet/interface/ConfigurationDescriptions.h>
#include <FWCore/ParameterSet/interface/ParameterSetDescription.h>

#include "CommonTools/Utils/interface/StringObjectFunction.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "DataFormats/TauReco/interface/PFRecoTauChargedHadron.h"
#include "DataFormats/TauReco/interface/PFRecoTauChargedHadronFwd.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

namespace {
  // Apply a hypothesis on the mass of the strips.
  math::XYZTLorentzVector applyMassConstraint(const math::XYZTLorentzVector& vec, double mass) {
    double factor = sqrt(vec.energy() * vec.energy() - mass * mass) / vec.P();
    return math::XYZTLorentzVector(vec.px() * factor, vec.py() * factor, vec.pz() * factor, vec.energy());
  }
}  // namespace

class PFRecoTauDiscriminationByHPSSelection : public PFTauDiscriminationProducerBase {
public:
  explicit PFRecoTauDiscriminationByHPSSelection(const edm::ParameterSet&);
  ~PFRecoTauDiscriminationByHPSSelection() override;
  double discriminate(const reco::PFTauRef&) const override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  typedef StringObjectFunction<reco::PFTau> TauFunc;

  struct DecayModeCuts {
    DecayModeCuts() : maxMass_(nullptr) {}
    ~DecayModeCuts() {}  // CV: maxMass object gets deleted by PFRecoTauDiscriminationByHPSSelection destructor
    unsigned nTracksMin_;
    unsigned nChargedPFCandsMin_;
    double minMass_;
    TauFunc* maxMass_;
    bool applyBendCorrection_mass_;
    bool applyBendCorrection_eta_;
    bool applyBendCorrection_phi_;
    double minPi0Mass_;
    double maxPi0Mass_;
    double assumeStripMass_;
  };

  typedef std::pair<unsigned int, unsigned int> IntPair;
  typedef std::pair<double, double> DoublePair;
  typedef std::map<IntPair, DecayModeCuts> DecayModeCutMap;

  DecayModeCutMap decayModeCuts_;
  double matchingCone_;
  double minPt_;

  bool requireTauChargedHadronsToBeChargedPFCands_;

  int minPixelHits_;

  int verbosity_;
};

PFRecoTauDiscriminationByHPSSelection::PFRecoTauDiscriminationByHPSSelection(const edm::ParameterSet& pset)
    : PFTauDiscriminationProducerBase(pset) {
  // Get the matchign cut
  matchingCone_ = pset.getParameter<double>("matchingCone");
  minPt_ = pset.getParameter<double>("minTauPt");
  // Get the mass cuts for each decay mode
  typedef std::vector<edm::ParameterSet> VPSet;
  const VPSet& decayModes = pset.getParameter<VPSet>("decayModes");
  for (auto const& decayMode : decayModes) {
    // The mass window(s)
    DecayModeCuts cuts;
    cuts.nTracksMin_ = decayMode.getParameter<unsigned>("nTracksMin");
    cuts.nChargedPFCandsMin_ = decayMode.getParameter<unsigned>("nChargedPFCandsMin");
    cuts.minMass_ = decayMode.getParameter<double>("minMass");
    cuts.maxMass_ = new TauFunc(decayMode.getParameter<std::string>("maxMass"));
    edm::ParameterSet applyBendCorrection = decayMode.getParameter<edm::ParameterSet>("applyBendCorrection");
    cuts.applyBendCorrection_eta_ = applyBendCorrection.getParameter<bool>("eta");
    cuts.applyBendCorrection_phi_ = applyBendCorrection.getParameter<bool>("phi");
    cuts.applyBendCorrection_mass_ = applyBendCorrection.getParameter<bool>("mass");
    cuts.minPi0Mass_ = decayMode.getParameter<double>("minPi0Mass");
    cuts.maxPi0Mass_ = decayMode.getParameter<double>("maxPi0Mass");
    cuts.assumeStripMass_ = decayMode.getParameter<double>("assumeStripMass");
    decayModeCuts_.insert(std::make_pair(
        // The decay mode as a key
        std::make_pair(decayMode.getParameter<uint32_t>("nCharged"), decayMode.getParameter<uint32_t>("nPiZeros")),
        cuts));
  }
  requireTauChargedHadronsToBeChargedPFCands_ = pset.getParameter<bool>("requireTauChargedHadronsToBeChargedPFCands");
  minPixelHits_ = pset.getParameter<int>("minPixelHits");
  verbosity_ = pset.getParameter<int>("verbosity");
}

PFRecoTauDiscriminationByHPSSelection::~PFRecoTauDiscriminationByHPSSelection() {
  for (DecayModeCutMap::iterator it = decayModeCuts_.begin(); it != decayModeCuts_.end(); ++it) {
    delete it->second.maxMass_;
  }
}

namespace {
  inline const reco::Track* getTrack(const reco::Candidate& cand) {
    const reco::PFCandidate* pfCandPtr = dynamic_cast<const reco::PFCandidate*>(&cand);
    if (pfCandPtr) {
      if (pfCandPtr->trackRef().isNonnull())
        return pfCandPtr->trackRef().get();
      else if (pfCandPtr->gsfTrackRef().isNonnull())
        return pfCandPtr->gsfTrackRef().get();
      else
        return nullptr;
    }
    const pat::PackedCandidate* packedCand = dynamic_cast<const pat::PackedCandidate*>(&cand);
    if (packedCand && packedCand->hasTrackDetails())
      return &packedCand->pseudoTrack();

    return nullptr;
  }
}  // namespace

double PFRecoTauDiscriminationByHPSSelection::discriminate(const reco::PFTauRef& tau) const {
  if (verbosity_) {
    edm::LogPrint("PFTauByHPSSelect") << "<PFRecoTauDiscriminationByHPSSelection::discriminate>:";
    edm::LogPrint("PFTauByHPSSelect") << " nCharged = " << tau->signalTauChargedHadronCandidates().size();
    edm::LogPrint("PFTauByHPSSelect") << " nPiZeros = " << tau->signalPiZeroCandidates().size();
  }

  // Check if we pass the min pt
  if (tau->pt() < minPt_) {
    if (verbosity_) {
      edm::LogPrint("PFTauByHPSSelect") << " fails minPt cut.";
    }
    return 0.0;
  }

  // See if we select this decay mode
  DecayModeCutMap::const_iterator massWindowIter = decayModeCuts_.find(
      std::make_pair(tau->signalTauChargedHadronCandidates().size(), tau->signalPiZeroCandidates().size()));
  // Check if decay mode is supported
  if (massWindowIter == decayModeCuts_.end()) {
    if (verbosity_) {
      edm::LogPrint("PFTauByHPSSelect") << " fails mass-window definition requirement.";
    }
    return 0.0;
  }
  const DecayModeCuts& massWindow = massWindowIter->second;

  if (massWindow.nTracksMin_ > 0) {
    unsigned int nTracks = 0;
    const std::vector<reco::PFRecoTauChargedHadron>& chargedHadrons = tau->signalTauChargedHadronCandidates();
    for (std::vector<reco::PFRecoTauChargedHadron>::const_iterator chargedHadron = chargedHadrons.begin();
         chargedHadron != chargedHadrons.end();
         ++chargedHadron) {
      if (chargedHadron->algoIs(reco::PFRecoTauChargedHadron::kChargedPFCandidate) ||
          chargedHadron->algoIs(reco::PFRecoTauChargedHadron::kTrack)) {
        ++nTracks;
      }
    }
    if (verbosity_) {
      edm::LogPrint("PFTauByHPSSelect") << "nTracks = " << nTracks << " (min = " << massWindow.nTracksMin_ << ")";
    }
    if (nTracks < massWindow.nTracksMin_) {
      if (verbosity_) {
        edm::LogPrint("PFTauByHPSSelect") << " fails nTracks requirement for mass-window.";
      }
      return 0.0;
    }
  }
  if (massWindow.nChargedPFCandsMin_ > 0) {
    unsigned int nChargedPFCands = 0;
    const std::vector<reco::PFRecoTauChargedHadron>& chargedHadrons = tau->signalTauChargedHadronCandidates();
    for (std::vector<reco::PFRecoTauChargedHadron>::const_iterator chargedHadron = chargedHadrons.begin();
         chargedHadron != chargedHadrons.end();
         ++chargedHadron) {
      if (chargedHadron->algoIs(reco::PFRecoTauChargedHadron::kChargedPFCandidate)) {
        ++nChargedPFCands;
      }
    }
    if (verbosity_) {
      edm::LogPrint("PFTauByHPSSelect") << "nChargedPFCands = " << nChargedPFCands
                                        << " (min = " << massWindow.nChargedPFCandsMin_ << ")";
    }
    if (nChargedPFCands < massWindow.nChargedPFCandsMin_) {
      if (verbosity_) {
        edm::LogPrint("PFTauByHPSSelect") << " fails nChargedPFCands requirement for mass-window.";
      }
      return 0.0;
    }
  }

  math::XYZTLorentzVector tauP4 = tau->p4();
  if (verbosity_) {
    edm::LogPrint("PFTauByHPSSelect") << "tau: Pt = " << tauP4.pt() << ", eta = " << tauP4.eta()
                                      << ", phi = " << tauP4.phi() << ", mass = " << tauP4.mass();
  }
  // Find the total pizero p4
  reco::Candidate::LorentzVector stripsP4;
  for (auto const& cand : tau->signalPiZeroCandidates()) {
    const math::XYZTLorentzVector& candP4 = cand.p4();
    stripsP4 += candP4;
  }

  // Apply strip mass assumption corrections
  if (massWindow.assumeStripMass_ >= 0) {
    for (auto const& cand : tau->signalPiZeroCandidates()) {
      const math::XYZTLorentzVector& uncorrected = cand.p4();
      math::XYZTLorentzVector corrected = applyMassConstraint(uncorrected, massWindow.assumeStripMass_);
      math::XYZTLorentzVector correction = corrected - uncorrected;
      tauP4 += correction;
      stripsP4 += correction;
    }
  }
  if (verbosity_) {
    edm::LogPrint("PFTauByHPSSelect") << "strips: Pt = " << stripsP4.pt() << ", eta = " << stripsP4.eta()
                                      << ", phi = " << stripsP4.phi() << ", mass = " << stripsP4.mass();
  }

  // Check if tau fails mass cut
  double maxMass_value = (*massWindow.maxMass_)(*tau);
  double bendCorrection_mass = (massWindow.applyBendCorrection_mass_) ? tau->bendCorrMass() : 0.;
  if (!((tauP4.M() - bendCorrection_mass) < maxMass_value && (tauP4.M() + bendCorrection_mass) > massWindow.minMass_)) {
    if (verbosity_) {
      edm::LogPrint("PFTauByHPSSelect") << " fails tau mass-window cut.";
    }
    return 0.0;
  }
  // CV: require that mass of charged tau decay products is always within specified mass window,
  //     irrespective of bendCorrection_mass
  reco::Candidate::LorentzVector tauP4_charged;
  const std::vector<reco::PFRecoTauChargedHadron>& signalChargedHadrons = tau->signalTauChargedHadronCandidates();
  for (std::vector<reco::PFRecoTauChargedHadron>::const_iterator signalChargedHadron = signalChargedHadrons.begin();
       signalChargedHadron != signalChargedHadrons.end();
       ++signalChargedHadron) {
    tauP4_charged += signalChargedHadron->p4();
  }
  if (!(tauP4_charged.mass() < maxMass_value)) {
    if (verbosity_) {
      edm::LogPrint("PFTauByHPSSelect") << " fails tau mass-window cut.";
    }
    return 0.0;
  }

  // Check if it fails the pi0 IM cut
  if (stripsP4.M() > massWindow.maxPi0Mass_ || stripsP4.M() < massWindow.minPi0Mass_) {
    if (verbosity_) {
      edm::LogPrint("PFTauByHPSSelect") << " fails strip mass-window cut.";
    }
    return 0.0;
  }

  // Check if tau passes matching cone cut
  //edm::LogPrint("PFTauByHPSSelect") << "dR(tau, jet) = " << deltaR(tauP4, tau->jetRef()->p4()) ;
  if (deltaR(tauP4, tau->jetRef()->p4()) > matchingCone_) {
    if (verbosity_) {
      edm::LogPrint("PFTauByHPSSelect") << " fails matching-cone cut.";
    }
    return 0.0;
  }

  // Check if tau passes cone cut
  double cone_size = tau->signalConeSize();
  // Check if any charged objects fail the signal cone cut
  for (auto const& cand : tau->signalTauChargedHadronCandidates()) {
    if (verbosity_) {
      edm::LogPrint("PFTauByHPSSelect") << "dR(tau, signalPFChargedHadr) = " << deltaR(cand.p4(), tauP4);
    }
    if (deltaR(cand.p4(), tauP4) > cone_size) {
      if (verbosity_) {
        edm::LogPrint("PFTauByHPSSelect") << " fails signal-cone cut for charged hadron(s).";
      }
      return 0.0;
    }
  }
  // Now check the pizeros
  for (auto const& cand : tau->signalPiZeroCandidates()) {
    double bendCorrection_eta = (massWindow.applyBendCorrection_eta_) ? cand.bendCorrEta() : 0.;
    double dEta = std::max(0., fabs(cand.eta() - tauP4.eta()) - bendCorrection_eta);
    double bendCorrection_phi = (massWindow.applyBendCorrection_phi_) ? cand.bendCorrPhi() : 0.;
    double dPhi = std::max(0., std::abs(reco::deltaPhi(cand.phi(), tauP4.phi())) - bendCorrection_phi);
    double dR2 = dEta * dEta + dPhi * dPhi;
    if (verbosity_) {
      edm::LogPrint("PFTauByHPSSelect") << "dR2(tau, signalPiZero) = " << dR2;
    }
    if (dR2 > cone_size * cone_size) {
      if (verbosity_) {
        edm::LogPrint("PFTauByHPSSelect") << " fails signal-cone cut for strip(s).";
      }
      return 0.0;
    }
  }

  if (requireTauChargedHadronsToBeChargedPFCands_) {
    for (auto const& cand : tau->signalTauChargedHadronCandidates()) {
      if (verbosity_) {
        std::string algo_string;
        if (cand.algo() == reco::PFRecoTauChargedHadron::kChargedPFCandidate)
          algo_string = "ChargedPFCandidate";
        else if (cand.algo() == reco::PFRecoTauChargedHadron::kTrack)
          algo_string = "Track";
        else if (cand.algo() == reco::PFRecoTauChargedHadron::kPFNeutralHadron)
          algo_string = "PFNeutralHadron";
        else
          algo_string = "Undefined";
        edm::LogPrint("PFTauByHPSSelect") << "algo(signalPFChargedHadr) = " << algo_string;
      }
      if (!(cand.algo() == reco::PFRecoTauChargedHadron::kChargedPFCandidate)) {
        if (verbosity_) {
          edm::LogPrint("PFTauByHPSSelect") << " fails cut on PFRecoTauChargedHadron algo.";
        }
        return 0.0;
      }
    }
  }

  if (minPixelHits_ > 0) {
    int numPixelHits = 0;
    const int nProngs = tau->decayMode() / 5 + 1;  //no. of charged prongs expected for this tau
    int nTracks = 0;
    for (const auto& chargedHadrCand : tau->signalChargedHadrCands()) {
      const reco::Track* track = getTrack(*chargedHadrCand);
      if (track != nullptr) {
        numPixelHits += track->hitPattern().numberOfValidPixelHits();
        nTracks++;
      }
    }
    //MB: how to deal with tau@miniAOD case?
    if (nTracks < nProngs) {  //"lost track" probably used to build this tau
      for (const auto& track : tau->signalTracks()) {
        if (track.isNonnull()) {
          numPixelHits += track->hitPattern().numberOfValidPixelHits();
          nTracks++;
        }
      }
    }
    if (!(numPixelHits >= minPixelHits_)) {
      if (verbosity_) {
        edm::LogPrint("PFTauByHPSSelect") << " fails cut on sum of pixel hits.";
      }
      return 0.0;
    }
  }

  // Otherwise, we pass!
  if (verbosity_) {
    edm::LogPrint("PFTauByHPSSelect") << " passes all cuts.";
  }
  return 1.0;
}

void PFRecoTauDiscriminationByHPSSelection::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  // hpsSelectionDiscriminator
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("PFTauProducer", edm::InputTag("combinatoricRecoTaus"));
  desc.add<int>("verbosity", 0);
  desc.add<double>("minTauPt", 0.0);
  {
    edm::ParameterSetDescription psd0;
    psd0.add<std::string>("BooleanOperator", "and");
    desc.add<edm::ParameterSetDescription>("Prediscriminants", psd0);
  }

  {
    edm::ParameterSetDescription vpset_decayModes;
    vpset_decayModes.add<double>("minPi0Mass", -1.e3);
    vpset_decayModes.add<std::string>("maxMass");
    vpset_decayModes.add<double>("maxPi0Mass", 1.e9);
    vpset_decayModes.add<unsigned int>("nPiZeros");
    vpset_decayModes.add<double>("minMass");
    vpset_decayModes.add<unsigned int>("nChargedPFCandsMin", 0);
    vpset_decayModes.add<unsigned int>("nTracksMin", 0);
    vpset_decayModes.add<unsigned int>("nCharged");
    {
      edm::ParameterSetDescription psd0;
      psd0.add<bool>("phi");
      psd0.add<bool>("eta");
      psd0.add<bool>("mass");
      vpset_decayModes.add<edm::ParameterSetDescription>("applyBendCorrection", psd0);
    }
    vpset_decayModes.add<double>("assumeStripMass", -1.0);
    std::vector<edm::ParameterSet> vpset_default;
    {
      edm::ParameterSet pset;
      pset.addParameter<double>("minPi0Mass", -1.e3);
      pset.addParameter<double>("maxPi0Mass", 1.e9);
      pset.addParameter<unsigned int>("nChargedPFCandsMin", 0);
      pset.addParameter<unsigned int>("nTracksMin", 0);
      pset.addParameter<double>("assumeStripMass", -1.0);
      vpset_default.push_back(pset);
    }
    desc.addVPSet("decayModes", vpset_decayModes, vpset_default);
  }

  desc.add<double>("matchingCone", 0.5);
  desc.add<int>("minPixelHits", 1);
  desc.add<bool>("requireTauChargedHadronsToBeChargedPFCands", false);
  descriptions.add("hpsSelectionDiscriminator", desc);
}

DEFINE_FWK_MODULE(PFRecoTauDiscriminationByHPSSelection);
