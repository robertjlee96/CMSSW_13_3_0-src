/* \class JetDeltaRValueMapProducer
 *
 * Associates jets using delta-R matching, and writes out
 * a valuemap of single float variables based on a StringObjectFunction. 
 * This is used for miniAOD. 
 *
 * \author: Sal Rappoccio
 *
 *
 * for more details about the cut syntax, see the documentation
 * page below:
 *
 *   https://twiki.cern.ch/twiki/bin/view/CMS/SWGuidePhysicsCutParser
 *
 *
 */

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/CaloJet.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "CommonTools/Utils/interface/StringObjectFunction.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Math/interface/deltaR.h"

template <class T, class C = T, typename TN = float>
class JetDeltaRValueMapProducer : public edm::stream::EDProducer<> {
public:
  typedef TN value_type;
  typedef edm::ValueMap<value_type> JetValueMap;
  typedef std::vector<value_type> ValueVector;

  JetDeltaRValueMapProducer(edm::ParameterSet const& params)
      : srcToken_(consumes<typename edm::View<T> >(params.getParameter<edm::InputTag>("src"))),
        matchedToken_(consumes<typename edm::View<C> >(params.getParameter<edm::InputTag>("matched"))),
        distMax_(params.getParameter<double>("distMax")),
        value_(params.existsAs<std::string>("value") ? params.getParameter<std::string>("value") : ""),
        values_(params.existsAs<std::vector<std::string> >("values")
                    ? params.getParameter<std::vector<std::string> >("values")
                    : std::vector<std::string>()),
        valueLabels_(params.existsAs<std::vector<std::string> >("valueLabels")
                         ? params.getParameter<std::vector<std::string> >("valueLabels")
                         : std::vector<std::string>()),
        lazyParser_(params.existsAs<bool>("lazyParser") ? params.getParameter<bool>("lazyParser") : false),
        multiValue_(false) {
    if (!value_.empty()) {
      if (value_ != "index") {
        evaluationMap_.insert(std::make_pair(
            value_, std::unique_ptr<StringObjectFunction<C> >(new StringObjectFunction<C>(value_, lazyParser_))));
      }
      produces<JetValueMap>();
    }

    if (!valueLabels_.empty() || !values_.empty()) {
      if (valueLabels_.size() == values_.size()) {
        multiValue_ = true;
        for (size_t i = 0; i < valueLabels_.size(); ++i) {
          evaluationMap_.insert(std::make_pair(
              valueLabels_[i],
              std::unique_ptr<StringObjectFunction<C> >(new StringObjectFunction<C>(values_[i], lazyParser_))));
          produces<JetValueMap>(valueLabels_[i]);
        }
      } else
        edm::LogWarning("ValueLabelMismatch")
            << "The number of value labels does not match the number of values. Values will not be evaluated.";
    }
  }

  ~JetDeltaRValueMapProducer() override {}

private:
  void produce(edm::Event& iEvent, const edm::EventSetup& iSetup) override {
    edm::Handle<typename edm::View<T> > h_jets1;
    iEvent.getByToken(srcToken_, h_jets1);
    edm::Handle<typename edm::View<C> > h_jets2;
    iEvent.getByToken(matchedToken_, h_jets2);

    ValueVector values(h_jets1->size(), -99999);

    std::map<std::string, std::vector<float> > valuesMap;
    if (multiValue_) {
      for (size_t i = 0; i < valueLabels_.size(); ++i)
        valuesMap.insert(std::make_pair(valueLabels_[i], std::vector<float>(h_jets1->size(), -99999)));
    }
    std::vector<bool> jets1_locks(h_jets1->size(), false);

    for (typename edm::View<C>::const_iterator ibegin = h_jets2->begin(), iend = h_jets2->end(), ijet = ibegin;
         ijet != iend;
         ++ijet) {
      float matched_dR2 = 1e9;
      int matched_index = -1;
      int iindex = ijet - ibegin;

      for (typename edm::View<T>::const_iterator jbegin = h_jets1->begin(), jend = h_jets1->end(), jjet = jbegin;
           jjet != jend;
           ++jjet) {
        int jindex = jjet - jbegin;

        if (jets1_locks.at(jindex))
          continue;  // skip jets that have already been matched

        float temp_dR2 = reco::deltaR2(ijet->eta(), ijet->phi(), jjet->eta(), jjet->phi());
        if (temp_dR2 < matched_dR2) {
          matched_dR2 = temp_dR2;
          matched_index = jindex;
        }
      }  // end loop over src jets

      if (matched_index >= 0) {
        if (matched_dR2 > distMax_ * distMax_)
          edm::LogInfo("MatchedJetsFarApart") << "Matched jets separated by dR greater than distMax=" << distMax_;
        else {
          jets1_locks.at(matched_index) = true;
          if (!value_.empty()) {
            if (value_ == "index") {
              values.at(matched_index) = iindex;
            } else {
              values.at(matched_index) = (*(evaluationMap_.at(value_)))(*ijet);
            }
          }
          if (multiValue_) {
            for (size_t i = 0; i < valueLabels_.size(); ++i)
              valuesMap.at(valueLabels_[i]).at(matched_index) = (*(evaluationMap_.at(valueLabels_[i])))(*ijet);
          }
        }
      }
    }  // end loop over matched jets

    if (!value_.empty()) {
      std::unique_ptr<JetValueMap> jetValueMap(new JetValueMap());

      typename JetValueMap::Filler filler(*jetValueMap);
      filler.insert(h_jets1, values.begin(), values.end());
      filler.fill();

      // put in Event
      iEvent.put(std::move(jetValueMap));
    }
    if (multiValue_) {
      for (size_t i = 0; i < valueLabels_.size(); ++i) {
        std::unique_ptr<JetValueMap> jetValueMap(new JetValueMap());

        typename JetValueMap::Filler filler(*jetValueMap);
        filler.insert(h_jets1, valuesMap.at(valueLabels_[i]).begin(), valuesMap.at(valueLabels_[i]).end());
        filler.fill();

        // put in Event
        iEvent.put(std::move(jetValueMap), valueLabels_[i]);
      }
    }
  }

  const edm::EDGetTokenT<typename edm::View<T> > srcToken_;
  const edm::EDGetTokenT<typename edm::View<C> > matchedToken_;
  const double distMax_;
  const std::string value_;
  const std::vector<std::string> values_;
  const std::vector<std::string> valueLabels_;
  const bool lazyParser_;
  bool multiValue_;
  std::map<std::string, std::unique_ptr<const StringObjectFunction<C> > > evaluationMap_;
};

typedef JetDeltaRValueMapProducer<reco::Jet> RecoJetDeltaRValueMapProducer;
typedef JetDeltaRValueMapProducer<reco::Jet, pat::Jet> RecoJetToPatJetDeltaRValueMapProducer;
typedef JetDeltaRValueMapProducer<pat::Jet> PatJetDeltaRValueMapProducer;
typedef JetDeltaRValueMapProducer<reco::Jet, reco::GenJet, int> RecoJetToGenJetDeltaRValueMapProducer;

DEFINE_FWK_MODULE(RecoJetDeltaRValueMapProducer);
DEFINE_FWK_MODULE(RecoJetToPatJetDeltaRValueMapProducer);
DEFINE_FWK_MODULE(PatJetDeltaRValueMapProducer);
DEFINE_FWK_MODULE(RecoJetToGenJetDeltaRValueMapProducer);
