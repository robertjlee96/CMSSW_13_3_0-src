/**\class ParticleDecayProducer
 *
 * Original Author:  Nicola De Filippis
 *
 */

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include <vector>

class ParticleDecayProducer : public edm::global::EDProducer<> {
public:
  explicit ParticleDecayProducer(const edm::ParameterSet&);
  ~ParticleDecayProducer() override = default;

private:
  void produce(edm::StreamID, edm::Event& e, edm::EventSetup const& c) const override;
  edm::EDGetTokenT<reco::CandidateCollection> genCandidatesToken_;
  const int motherPdgId_;
  const std::vector<int> daughtersPdgId_;
  const std::string decayChain_;
  std::vector<std::string> valias;
};

// Candidate handling
#include "DataFormats/Candidate/interface/CandMatchMap.h"
#include "DataFormats/Common/interface/AssociationVector.h"
#include "CommonTools/Utils/interface/PtComparator.h"
#include "FWCore/Framework/interface/Event.h"
#include <sstream>
using namespace edm;
using namespace std;
using namespace reco;

// constructors
ParticleDecayProducer::ParticleDecayProducer(const edm::ParameterSet& iConfig)
    : genCandidatesToken_(consumes<CandidateCollection>(iConfig.getParameter<InputTag>("src"))),
      motherPdgId_(iConfig.getParameter<int>("motherPdgId")),
      daughtersPdgId_(iConfig.getParameter<vector<int> >("daughtersPdgId")),
      decayChain_(iConfig.getParameter<std::string>("decayChain")) {
  string alias;
  produces<CandidateCollection>(alias = decayChain_ + "Mother").setBranchAlias(alias);
  for (unsigned int j = 0; j < daughtersPdgId_.size(); ++j) {
    ostringstream index, collection;
    index << j;
    collection << decayChain_ << "Lepton" << index.str();
    valias.push_back(collection.str());
    produces<CandidateCollection>(valias.at(j)).setBranchAlias(valias.at(j));
  }
}

void ParticleDecayProducer::produce(edm::StreamID, edm::Event& iEvent, edm::EventSetup const&) const {
  // get gen particle candidates
  const edm::Handle<CandidateCollection> genCandidatesCollection = iEvent.getHandle(genCandidatesToken_);

  unique_ptr<CandidateCollection> mothercands(new CandidateCollection);
  unique_ptr<CandidateCollection> daughterscands(new CandidateCollection);
  size_t daughtersize = daughtersPdgId_.size();
  for (CandidateCollection::const_iterator p = genCandidatesCollection->begin(); p != genCandidatesCollection->end();
       ++p) {
    if (p->pdgId() == motherPdgId_ && p->status() == 3) {
      mothercands->push_back(p->clone());
      size_t ndau = p->numberOfDaughters();
      for (size_t i = 0; i < ndau; ++i) {
        for (size_t j = 0; j < daughtersize; ++j) {
          if (p->daughter(i)->pdgId() == daughtersPdgId_[j] && p->daughter(i)->status() == 3) {
            daughterscands->push_back(p->daughter(i)->clone());
          }
        }
      }
    }
  }

  iEvent.put(std::move(mothercands), decayChain_ + "Mother");
  daughterscands->sort(GreaterByPt<reco::Candidate>());

  for (unsigned int row = 0; row < daughtersize; ++row) {
    unique_ptr<CandidateCollection> leptonscands_(new CandidateCollection);
    leptonscands_->push_back((daughterscands->begin() + row)->clone());
    iEvent.put(std::move(leptonscands_), valias.at(row));
  }
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(ParticleDecayProducer);
