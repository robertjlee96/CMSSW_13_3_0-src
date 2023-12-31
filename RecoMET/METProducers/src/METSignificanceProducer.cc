// -*- C++ -*-
//
// Package:    METProducers
// Class:      METSignificanceProducer
//
//

//____________________________________________________________________________||
#include "RecoMET/METProducers/interface/METSignificanceProducer.h"

//____________________________________________________________________________||
namespace cms {

  //____________________________________________________________________________||
  METSignificanceProducer::METSignificanceProducer(const edm::ParameterSet& iConfig) {
    std::vector<edm::InputTag> srcLeptonsTags = iConfig.getParameter<std::vector<edm::InputTag>>("srcLeptons");
    for (std::vector<edm::InputTag>::const_iterator it = srcLeptonsTags.begin(); it != srcLeptonsTags.end(); it++) {
      lepTokens_.push_back(consumes<edm::View<reco::Candidate>>(*it));
    }

    pfjetsToken_ = consumes<edm::View<reco::Jet>>(iConfig.getParameter<edm::InputTag>("srcPfJets"));

    metToken_ = consumes<edm::View<reco::MET>>(iConfig.getParameter<edm::InputTag>("srcMet"));
    pfCandidatesToken_ = consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("srcPFCandidates"));

    jetSFType_ = iConfig.getParameter<std::string>("srcJetSF");
    jetResPtType_ = iConfig.getParameter<std::string>("srcJetResPt");
    jetResPhiType_ = iConfig.getParameter<std::string>("srcJetResPhi");
    jetSFTypeToken_ = esConsumes(edm::ESInputTag("", jetSFType_));
    jetResPtTypeToken_ = esConsumes(edm::ESInputTag("", jetResPtType_));
    jetResPhiTypeToken_ = esConsumes(edm::ESInputTag("", jetResPhiType_));
    rhoToken_ = consumes<double>(iConfig.getParameter<edm::InputTag>("srcRho"));

    edm::InputTag srcWeights = iConfig.getParameter<edm::InputTag>("srcWeights");
    if (!srcWeights.label().empty())
      weightsToken_ = consumes<edm::ValueMap<float>>(srcWeights);

    metSigAlgo_ = new metsig::METSignificance(iConfig);

    produces<double>("METSignificance");
    produces<math::Error<2>::type>("METCovariance");
  }

  METSignificanceProducer::~METSignificanceProducer() { delete metSigAlgo_; }

  //____________________________________________________________________________||
  void METSignificanceProducer::produce(edm::Event& event, const edm::EventSetup& setup) {
    //
    // met
    //
    edm::Handle<edm::View<reco::MET>> metHandle;
    event.getByToken(metToken_, metHandle);
    const reco::MET& met = (*metHandle)[0];

    //
    // candidates
    //
    edm::Handle<reco::CandidateView> pfCandidates;
    event.getByToken(pfCandidatesToken_, pfCandidates);

    //
    // leptons
    //
    std::vector<edm::Handle<reco::CandidateView>> leptons;
    for (std::vector<edm::EDGetTokenT<edm::View<reco::Candidate>>>::const_iterator srcLeptons_i = lepTokens_.begin();
         srcLeptons_i != lepTokens_.end();
         ++srcLeptons_i) {
      edm::Handle<reco::CandidateView> leptons_i;
      event.getByToken(*srcLeptons_i, leptons_i);
      leptons.push_back(leptons_i);
    }

    //
    // jets
    //
    edm::Handle<edm::View<reco::Jet>> jets;
    event.getByToken(pfjetsToken_, jets);

    edm::Handle<double> rho;
    event.getByToken(rhoToken_, rho);

    edm::Handle<edm::ValueMap<float>> weights;
    if (!weightsToken_.isUninitialized())
      event.getByToken(weightsToken_, weights);

    JME::JetResolution resPtObj = JME::JetResolution::get(setup, jetResPtTypeToken_);
    JME::JetResolution resPhiObj = JME::JetResolution::get(setup, jetResPhiTypeToken_);
    JME::JetResolutionScaleFactor resSFObj = JME::JetResolutionScaleFactor::get(setup, jetSFTypeToken_);

    //
    // compute the significance
    //
    double sumPtUnclustered = 0;
    const edm::ValueMap<float>* weightsPtr = nullptr;
    if (met.isWeighted()) {
      if (weightsToken_.isUninitialized())
        throw cms::Exception("InvalidInput")
            << "MET is weighted (e.g. PUPPI), but no weights given in METSignificanceProducer\n";
      weightsPtr = &*weights;
    }
    reco::METCovMatrix cov = metSigAlgo_->getCovariance(*jets,
                                                        leptons,
                                                        pfCandidates,
                                                        *rho,
                                                        resPtObj,
                                                        resPhiObj,
                                                        resSFObj,
                                                        event.isRealData(),
                                                        sumPtUnclustered,
                                                        weightsPtr);
    double sig = metSigAlgo_->getSignificance(cov, met);

    auto significance = std::make_unique<double>();
    (*significance) = sig;

    auto covPtr = std::make_unique<math::Error<2>::type>();
    (*covPtr)(0, 0) = cov(0, 0);
    (*covPtr)(1, 0) = cov(1, 0);
    (*covPtr)(1, 1) = cov(1, 1);

    event.put(std::move(covPtr), "METCovariance");
    event.put(std::move(significance), "METSignificance");
  }

  //____________________________________________________________________________||
  DEFINE_FWK_MODULE(METSignificanceProducer);
}  // namespace cms

//____________________________________________________________________________||
