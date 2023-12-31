/*
 * RecoTauCleaner
 *
 * Author: Evan K. Friis, UC Davis
 *
 * Given a input collection of PFTaus, produces an output collection of PFTaus
 * such that no two PFTaus come from the same PFJet.  If multiple taus in the
 * collection come from the same PFJet, (dirty) they are ordered according to a
 * list of cleaners.  Each cleaner is a RecoTauCleanerPlugin, and returns a
 * double corresponding to the 'quality' for a given tau - an example would be
 * the level of isolation.  The set of dirty taus is then ranked
 * lexicographically by these cleaners, and the best one is placed in the
 * output collection.
 *
 */

#include <algorithm>
#include <memory>

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <FWCore/ParameterSet/interface/ConfigurationDescriptions.h>
#include <FWCore/ParameterSet/interface/ParameterSetDescription.h>

#include "RecoTauTag/RecoTau/interface/RecoTauBuilderPlugins.h"
#include "RecoTauTag/RecoTau/interface/RecoTauCleaningTools.h"
#include "RecoTauTag/RecoTau/interface/RecoTauCommonUtilities.h"

#include "DataFormats/TauReco/interface/PFTau.h"
#include "DataFormats/TauReco/interface/PFTauFwd.h"
#include "DataFormats/TauReco/interface/PFRecoTauChargedHadron.h"
#include "DataFormats/TauReco/interface/RecoTauPiZero.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

template <typename Prod>
class RecoTauCleanerImpl : public edm::stream::EDProducer<> {
  typedef reco::tau::RecoTauCleanerPlugin Cleaner;
  struct CleanerEntryType {
    std::shared_ptr<Cleaner> plugin_;
    float tolerance_;
  };
  typedef std::vector<std::unique_ptr<CleanerEntryType>> CleanerList;
  // Define our output type - i.e. reco::PFTau OR reco::PFTauRef
  typedef typename Prod::value_type output_type;

  // Predicate that determines if two taus 'overlap' i.e. share a base PFJet
  class RemoveDuplicateJets {
  public:
    bool operator()(const reco::PFTauRef& a, const reco::PFTauRef& b) const { return (a->jetRef() == b->jetRef()); }
  };

public:
  explicit RecoTauCleanerImpl(const edm::ParameterSet& pset);
  ~RecoTauCleanerImpl() override;
  void produce(edm::Event& evt, const edm::EventSetup& es) override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  edm::InputTag tauSrc_;
  CleanerList cleaners_;
  // Optional selection on the output of the taus
  std::unique_ptr<const StringCutObjectSelector<reco::PFTau>> outputSelector_;
  edm::EDGetTokenT<reco::PFTauCollection> tau_token;
  int verbosity_;
};

template <typename Prod>
RecoTauCleanerImpl<Prod>::RecoTauCleanerImpl(const edm::ParameterSet& pset) {
  tauSrc_ = pset.getParameter<edm::InputTag>("src");
  tau_token = consumes<reco::PFTauCollection>(tauSrc_);
  // Build our list of quality plugins
  typedef std::vector<edm::ParameterSet> VPSet;
  // Get each of our tau builders
  const VPSet& cleaners = pset.getParameter<VPSet>("cleaners");
  for (VPSet::const_iterator cleanerPSet = cleaners.begin(); cleanerPSet != cleaners.end(); ++cleanerPSet) {
    auto cleanerEntry = std::make_unique<CleanerEntryType>();
    // Get plugin name
    const std::string& pluginType = cleanerPSet->getParameter<std::string>("plugin");
    // Build the plugin
    cleanerEntry->plugin_ = RecoTauCleanerPluginFactory::get()->create(pluginType, *cleanerPSet, consumesCollector());
    cleanerEntry->tolerance_ = cleanerPSet->getParameter<double>("tolerance");
    cleaners_.emplace_back(std::move(cleanerEntry));
  }

  // Check if we want to apply a final output selection
  std::string selection = pset.getParameter<std::string>("outputSelection");
  if (!selection.empty()) {
    outputSelector_ = std::make_unique<StringCutObjectSelector<reco::PFTau>>(selection);
  }

  // Enable/disable debug output
  verbosity_ = pset.getParameter<int>("verbosity");

  // Build the predicate that ranks our taus.
  produces<Prod>();
}

template <typename Prod>
RecoTauCleanerImpl<Prod>::~RecoTauCleanerImpl() {}

namespace {
  // Template to convert a ref to desired output type
  template <typename T>
  const T convert(const reco::PFTauRef& tau);

  //template<> const edm::RefToBase<reco::PFTau>
  //convert<edm::RefToBase<reco::PFTau> >(const reco::PFTauRef &tau) {
  // return edm::RefToBase<reco::PFTau>(tau);
  //}

  template <>
  const reco::PFTauRef convert<reco::PFTauRef>(const reco::PFTauRef& tau) {
    return tau;
  }

  template <>
  const reco::PFTau convert<reco::PFTau>(const reco::PFTauRef& tau) {
    return *tau;
  }
}  // namespace

namespace {
  template <typename T>
  std::string format_vT(const std::vector<T>& vT) {
    std::ostringstream os;
    os << "{ ";
    unsigned numEntries = vT.size();
    for (unsigned iEntry = 0; iEntry < numEntries; ++iEntry) {
      os << vT[iEntry];
      if (iEntry < (numEntries - 1))
        os << ", ";
    }
    os << " }";
    return os.str();
  }

  struct PFTauRankType {
    PFTauRankType(const reco::PFTauRef& tauRef) : idx_(tauRef.key()), tauRef_(tauRef) {}
    ~PFTauRankType() {}
    void print(const std::string& label) const {
      std::cout << label << " (" << tauRef_.id() << ":" << tauRef_.key() << ", idx = " << idx_ << "):";
      assert(tauRef_.key() == idx_);
      std::cout << " Pt = " << tauRef_->pt() << ", eta = " << tauRef_->eta() << ", phi = " << tauRef_->phi()
                << ", mass = " << tauRef_->mass() << " (decayMode = " << tauRef_->decayMode() << ")";
      std::cout << std::endl;
      std::cout << "associated jet:";
      if (tauRef_->jetRef().isNonnull()) {
        std::cout << " Pt = " << tauRef_->jetRef()->pt() << ", eta = " << tauRef_->jetRef()->eta()
                  << ", phi = " << tauRef_->jetRef()->phi() << ", mass = " << tauRef_->jetRef()->mass()
                  << ", area = " << tauRef_->jetRef()->jetArea();
      } else
        std::cout << " N/A";
      std::cout << std::endl;
      const std::vector<reco::PFRecoTauChargedHadron>& signalTauChargedHadronCandidates =
          tauRef_->signalTauChargedHadronCandidates();
      size_t numChargedHadrons = signalTauChargedHadronCandidates.size();
      for (size_t iChargedHadron = 0; iChargedHadron < numChargedHadrons; ++iChargedHadron) {
        const reco::PFRecoTauChargedHadron& chargedHadron = signalTauChargedHadronCandidates.at(iChargedHadron);
        std::cout << " chargedHadron #" << iChargedHadron << ":" << std::endl;
        chargedHadron.print(std::cout);
      }
      const std::vector<reco::RecoTauPiZero>& signalPiZeroCandidates = tauRef_->signalPiZeroCandidates();
      size_t numPiZeros = signalPiZeroCandidates.size();
      std::cout << "signalPiZeroCandidates = " << numPiZeros << std::endl;
      for (size_t iPiZero = 0; iPiZero < numPiZeros; ++iPiZero) {
        const reco::RecoTauPiZero& piZero = signalPiZeroCandidates.at(iPiZero);
        std::cout << " piZero #" << iPiZero << ": Pt = " << piZero.pt() << ", eta = " << piZero.eta()
                  << ", phi = " << piZero.phi() << ", mass = " << piZero.mass() << std::endl;
      }
      const auto& isolationCands = tauRef_->isolationCands();
      size_t numCands = isolationCands.size();
      std::cout << "isolationCands = " << numCands << std::endl;
      for (size_t iCand = 0; iCand < numCands; ++iCand) {
        const auto& cand = isolationCands.at(iCand);
        std::cout << " pfCand #" << iCand << " (" << cand.id() << ":" << cand.key() << "):"
                  << " Pt = " << cand->pt() << ", eta = " << cand->eta() << ", phi = " << cand->phi() << std::endl;
      }
      std::cout << " ranks = " << format_vT(ranks_) << std::endl;
      std::cout << " tolerances = " << format_vT(tolerances_) << std::endl;
    }
    size_t idx_;
    reco::PFTauRef tauRef_;
    size_t N_;
    std::vector<float> ranks_;
    std::vector<float> tolerances_;
  };

  bool isHigherRank(const PFTauRankType* tau1, const PFTauRankType* tau2) {
    //std::cout << "<isHigherRank>:" << std::endl;
    //std::cout << "tau1 @ " << tau1;
    //tau1->print("");
    //std::cout << "tau2 @ " << tau2;
    //tau2->print("");
    assert(tau1->N_ == tau1->ranks_.size());
    assert(tau1->N_ == tau2->ranks_.size());
    assert(tau1->N_ == tau1->tolerances_.size());
    for (size_t i = 0; i < tau1->N_; ++i) {
      const float& val1 = tau1->ranks_[i];
      const float& val2 = tau2->ranks_[i];
      double av = 0.5 * (val1 + val2);
      double thresh = av * tau1->tolerances_[i];
      if (val1 < (val2 - thresh))
        return true;
      else if (val2 < (val1 - thresh))
        return false;
    }
    return true;
  }
}  // namespace

template <typename Prod>
void RecoTauCleanerImpl<Prod>::produce(edm::Event& evt, const edm::EventSetup& es) {
  if (verbosity_) {
    std::cout << "<RecoTauCleanerImpl::produce>:" << std::endl;
  }

  // Update all our cleaners with the event info if they need it
  for (typename CleanerList::iterator cleaner = cleaners_.begin(); cleaner != cleaners_.end(); ++cleaner) {
    (*cleaner)->plugin_->setup(evt, es);
  }

  // Get the input collection of all taus. Some are from the same PFJet. We must clean them.
  edm::Handle<reco::PFTauCollection> inputTaus;
  evt.getByToken(tau_token, inputTaus);

  // Sort the input tau refs according to our predicate
  std::list<PFTauRankType*> rankedTaus;
  size_t N = inputTaus->size();
  for (size_t idx = 0; idx < N; ++idx) {
    reco::PFTauRef inputRef(inputTaus, idx);
    PFTauRankType* rankedTau = new PFTauRankType(inputRef);
    rankedTau->N_ = cleaners_.size();
    rankedTau->ranks_.reserve(rankedTau->N_);
    rankedTau->tolerances_.reserve(rankedTau->N_);
    for (typename CleanerList::const_iterator cleaner = cleaners_.begin(); cleaner != cleaners_.end(); ++cleaner) {
      rankedTau->ranks_.push_back((*(*cleaner)->plugin_)(inputRef));
      rankedTau->tolerances_.push_back((*cleaner)->tolerance_);
    }
    if (verbosity_) {
      std::ostringstream os;
      os << "rankedTau #" << idx;
      rankedTau->print(os.str());
    }
    rankedTaus.push_back(rankedTau);
  }
  rankedTaus.sort(isHigherRank);

  // Make an STL algorithm friendly vector of refs
  typedef std::vector<reco::PFTauRef> PFTauRefs;
  PFTauRefs dirty(inputTaus->size());
  size_t idx_sorted = 0;
  for (std::list<PFTauRankType*>::const_iterator rankedTau = rankedTaus.begin(); rankedTau != rankedTaus.end();
       ++rankedTau) {
    dirty[idx_sorted] = (*rankedTau)->tauRef_;
    if (verbosity_) {
      std::cout << "dirty[" << idx_sorted << "] = " << dirty[idx_sorted].id() << ":" << dirty[idx_sorted].key()
                << std::endl;
    }
    delete (*rankedTau);
    ++idx_sorted;
  }

  // Clean the taus, ensuring that only one tau per jet is produced
  PFTauRefs cleanTaus = reco::tau::cleanOverlaps<PFTauRefs, RemoveDuplicateJets>(dirty);

  // create output collection
  auto output = std::make_unique<Prod>();
  //output->reserve(cleanTaus.size());

  // Copy clean refs into output
  for (PFTauRefs::const_iterator tau = cleanTaus.begin(); tau != cleanTaus.end(); ++tau) {
    // If we are applying an output selection, check if it passes
    bool selected = true;
    if (outputSelector_.get() && !(*outputSelector_)(**tau)) {
      selected = false;
    }
    if (selected) {
      output->push_back(convert<output_type>(*tau));
    }
  }
  evt.put(std::move(output));
}

typedef RecoTauCleanerImpl<reco::PFTauCollection> RecoTauCleaner;
typedef RecoTauCleanerImpl<reco::PFTauRefVector> RecoTauRefCleaner;

template <>
void RecoTauCleanerImpl<reco::PFTauCollection>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  // RecoTauCleaner
  edm::ParameterSetDescription desc;
  desc.add<std::string>("outputSelection", "");
  {
    // this description is the validator for all PSets in the cleaners VPSet passed to the plugin from the python configuration
    edm::ParameterSetDescription vps_description_for_cleaners;
    // the common parameters for all cleaners
    // no default value is provided -- the user has to provide the values for these parameters
    vps_description_for_cleaners.add<std::string>("plugin");
    vps_description_for_cleaners.add<double>("tolerance", 0);
    vps_description_for_cleaners.add<std::string>("name");

    // the following parameters are not common for all cleaners, they are needed for few PSets, therefore they are added as optional
    //     these optional parameters are used in the default cleaners vector
    vps_description_for_cleaners.addOptional<int>("passForCharge");
    vps_description_for_cleaners.addOptional<double>("selectionFailValue");
    vps_description_for_cleaners.addOptional<std::vector<unsigned int>>("nprongs");
    vps_description_for_cleaners.addOptional<edm::InputTag>("src");
    vps_description_for_cleaners.addOptional<double>("minTrackPt");
    vps_description_for_cleaners.addOptional<std::string>("selection");
    vps_description_for_cleaners.addOptional<std::string>("selectionPassFunction");
    //     more PSets for cleaners can be found in
    //     RecoTauTag/RecoTau/python/RecoTauCleanerPlugins.py
    //     however, at this moment (2018-11-09) they do not have any new optional parameters

    // the cleaner defaults, as in RecoTauTag/RecoTau/python/RecoTauCleaner_cfi.py
    std::vector<edm::ParameterSet> default_cleaners;
    default_cleaners.reserve(7);
    {
      edm::ParameterSet cleaner_Charge;
      cleaner_Charge.addParameter<std::string>("name", "Charge");
      cleaner_Charge.addParameter<std::string>("plugin", "RecoTauChargeCleanerPlugin");
      cleaner_Charge.addParameter<int>("passForCharge", 1);
      cleaner_Charge.addParameter<double>("selectionFailValue", 0);
      cleaner_Charge.addParameter<std::vector<unsigned int>>("nprongs",
                                                             {
                                                                 1,
                                                                 3,
                                                             });
      cleaner_Charge.addParameter<double>("tolerance", 0);
      default_cleaners.push_back(cleaner_Charge);
    }
    {
      edm::ParameterSet temp2;
      temp2.addParameter<std::string>("name", "HPS_Select");
      temp2.addParameter<std::string>("plugin", "RecoTauDiscriminantCleanerPlugin");
      temp2.addParameter<edm::InputTag>("src", edm::InputTag("hpsSelectionDiscriminator"));
      temp2.addParameter<double>("tolerance", 0);
      default_cleaners.push_back(temp2);
    }
    {
      edm::ParameterSet temp2;
      temp2.addParameter<std::string>("name", "killSoftTwoProngTaus");
      temp2.addParameter<std::string>("plugin", "RecoTauSoftTwoProngTausCleanerPlugin");
      temp2.addParameter<double>("minTrackPt", 5.0);
      temp2.addParameter<double>("tolerance", 0);
      default_cleaners.push_back(temp2);
    }
    {
      edm::ParameterSet temp2;
      temp2.addParameter<std::string>("name", "ChargedHadronMultiplicity");
      temp2.addParameter<std::string>("plugin", "RecoTauChargedHadronMultiplicityCleanerPlugin");
      temp2.addParameter<double>("tolerance", 0);
      default_cleaners.push_back(temp2);
    }
    {
      edm::ParameterSet temp2;
      temp2.addParameter<std::string>("name", "Pt");
      temp2.addParameter<std::string>("plugin", "RecoTauStringCleanerPlugin");
      temp2.addParameter<std::string>("selectionPassFunction", "-pt()");
      temp2.addParameter<std::string>("selection", "leadPFCand().isNonnull()");
      temp2.addParameter<double>("selectionFailValue", 1000.0);
      temp2.addParameter<double>("tolerance", 0.01);
      default_cleaners.push_back(temp2);
    }
    {
      edm::ParameterSet temp2;
      temp2.addParameter<std::string>("name", "StripMultiplicity");
      temp2.addParameter<std::string>("plugin", "RecoTauStringCleanerPlugin");
      temp2.addParameter<std::string>("selectionPassFunction", "-signalPiZeroCandidates().size()");
      temp2.addParameter<std::string>("selection", "leadPFCand().isNonnull()");
      temp2.addParameter<double>("selectionFailValue", 1000.0);
      temp2.addParameter<double>("tolerance", 0);
      default_cleaners.push_back(temp2);
    }
    {
      edm::ParameterSet temp2;
      temp2.addParameter<std::string>("name", "CombinedIsolation");
      temp2.addParameter<std::string>("plugin", "RecoTauStringCleanerPlugin");
      temp2.addParameter<std::string>("selectionPassFunction",
                                      "isolationPFChargedHadrCandsPtSum() + isolationPFGammaCandsEtSum()");
      temp2.addParameter<std::string>("selection", "leadPFCand().isNonnull()");
      temp2.addParameter<double>("selectionFailValue", 1000.0);
      temp2.addParameter<double>("tolerance", 0);
      default_cleaners.push_back(temp2);
    }

    desc.addVPSet("cleaners", vps_description_for_cleaners, default_cleaners);
  }

  desc.add<int>("verbosity", 0);
  desc.add<edm::InputTag>("src", edm::InputTag("combinatoricRecoTaus"));
  descriptions.add("RecoTauCleaner", desc);
}

template <>
void RecoTauCleanerImpl<reco::PFTauRefVector>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  // there was no cfi file for this plugin
}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(RecoTauCleaner);
DEFINE_FWK_MODULE(RecoTauRefCleaner);
