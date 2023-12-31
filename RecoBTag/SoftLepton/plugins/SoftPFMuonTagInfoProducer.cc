// * Author: Alberto Zucchetta
// * Mail: a.zucchetta@cern.ch
// * January 16, 2015

#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/BTauReco/interface/SoftLeptonTagInfo.h"
#include "DataFormats/BTauReco/interface/CandSoftLeptonTagInfo.h"

#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
// Muons
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include "DataFormats/BTauReco/interface/SoftLeptonTagInfo.h"

// Transient Track and IP
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include <cmath>
#include <vector>

class SoftPFMuonTagInfoProducer : public edm::global::EDProducer<> {
public:
  SoftPFMuonTagInfoProducer(const edm::ParameterSet& conf);

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const final;
  float boostedPPar(const math::XYZVector&, const math::XYZVector&) const;

  const edm::EDGetTokenT<edm::View<reco::Jet> > jetToken;
  const edm::EDGetTokenT<edm::View<reco::Muon> > muonToken;
  const edm::EDGetTokenT<reco::VertexCollection> vertexToken;
  const edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> builderToken;
  const edm::EDPutTokenT<reco::CandSoftLeptonTagInfoCollection> putToken;
  const float pTcut, SIPsigcut, IPsigcut, ratio1cut, ratio2cut;
  const bool useFilter;
};

SoftPFMuonTagInfoProducer::SoftPFMuonTagInfoProducer(const edm::ParameterSet& conf)
    : jetToken(consumes(conf.getParameter<edm::InputTag>("jets"))),
      muonToken(consumes(conf.getParameter<edm::InputTag>("muons"))),
      vertexToken(consumes(conf.getParameter<edm::InputTag>("primaryVertex"))),
      builderToken(esConsumes(edm::ESInputTag("", "TransientTrackBuilder"))),
      putToken(produces()),
      pTcut(conf.getParameter<double>("muonPt")),
      SIPsigcut(conf.getParameter<double>("muonSIPsig")),
      IPsigcut(conf.getParameter<double>("filterIpsig")),
      ratio1cut(conf.getParameter<double>("filterRatio1")),
      ratio2cut(conf.getParameter<double>("filterRatio2")),
      useFilter(conf.getParameter<bool>("filterPromptMuons")) {}

void SoftPFMuonTagInfoProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("jets");
  desc.add<edm::InputTag>("muons");
  desc.add<edm::InputTag>("primaryVertex");
  desc.add<double>("muonPt");
  desc.add<double>("muonSIPsig");
  desc.add<double>("filterIpsig");
  desc.add<double>("filterRatio1");
  desc.add<double>("filterRatio2");
  desc.add<bool>("filterPromptMuons");
  descriptions.addDefault(desc);
}

void SoftPFMuonTagInfoProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
  // Declare produced collection
  reco::CandSoftLeptonTagInfoCollection theMuonTagInfo;

  // Declare and open Jet collection
  edm::Handle<edm::View<reco::Jet> > theJetCollection = iEvent.getHandle(jetToken);

  // Declare Muon collection
  edm::Handle<edm::View<reco::Muon> > theMuonCollection = iEvent.getHandle(muonToken);

  // Declare and open Vertex collection
  edm::Handle<reco::VertexCollection> theVertexCollection = iEvent.getHandle(vertexToken);
  if (!theVertexCollection.isValid() || theVertexCollection->empty()) {
    iEvent.emplace(putToken, std::move(theMuonTagInfo));
    return;
  }
  const reco::Vertex* vertex = &theVertexCollection->front();

  // Build TransientTrackBuilder
  const TransientTrackBuilder& transientTrackBuilder = iSetup.getData(builderToken);

  // Loop on jets
  for (unsigned int ij = 0, nj = theJetCollection->size(); ij < nj; ij++) {
    edm::RefToBase<reco::Jet> jetRef = theJetCollection->refAt(ij);
    // Build TagInfo object
    reco::CandSoftLeptonTagInfo tagInfo;
    tagInfo.setJetRef(jetRef);
    // Loop on jet daughters
    for (unsigned int id = 0, nd = jetRef->numberOfDaughters(); id < nd; ++id) {
      edm::Ptr<reco::Candidate> lepPtr = jetRef->daughterPtr(id);
      if (std::abs(lepPtr->pdgId()) != 13)
        continue;

      const reco::Muon* muon(nullptr);
      // Step 1: try to access the muon from reco::PFCandidate
      const reco::PFCandidate* pfcand = dynamic_cast<const reco::PFCandidate*>(lepPtr.get());
      if (pfcand) {
        muon = pfcand->muonRef().get();
      }
      // If not PFCandidate is available, find a match looping on the muon collection
      else {
        for (unsigned int im = 0, nm = theMuonCollection->size(); im < nm; ++im) {  // --- Begin loop on muons
          const reco::Muon* recomuon = &theMuonCollection->at(im);
          const pat::Muon* patmuon = dynamic_cast<const pat::Muon*>(recomuon);
          // Step 2: try a match between reco::Candidate
          if (patmuon) {
            if (patmuon->originalObjectRef() == lepPtr) {
              muon = theMuonCollection->refAt(im).get();
              break;
            }
          }
          // Step 3: try a match with dR and dpT if pat::Muon casting fails
          else {
            if (reco::deltaR(*recomuon, *lepPtr) < 0.01 &&
                std::abs(recomuon->pt() - lepPtr->pt()) / lepPtr->pt() < 0.1) {
              muon = theMuonCollection->refAt(im).get();
              break;
            }
          }
        }  // --- End loop on muons
      }
      if (!muon || !muon::isLooseMuon(*muon) || muon->pt() < pTcut)
        continue;
      reco::TrackRef trkRef(muon->innerTrack());
      reco::TrackBaseRef trkBaseRef(trkRef);
      // Build Transient Track
      reco::TransientTrack transientTrack = transientTrackBuilder.build(trkRef);
      // Define jet and muon vectors
      reco::Candidate::Vector jetvect(jetRef->p4().Vect()), muonvect(muon->p4().Vect());
      // Calculate variables
      reco::SoftLeptonProperties properties;
      Measurement1D ip2d = IPTools::signedTransverseImpactParameter(
                               transientTrack, GlobalVector(jetRef->px(), jetRef->py(), jetRef->pz()), *vertex)
                               .second;
      Measurement1D ip3d = IPTools::signedImpactParameter3D(
                               transientTrack, GlobalVector(jetRef->px(), jetRef->py(), jetRef->pz()), *vertex)
                               .second;
      properties.sip2dsig = ip2d.significance();
      properties.sip3dsig = ip3d.significance();
      properties.sip2d = ip2d.value();
      properties.sip3d = ip3d.value();
      properties.deltaR = reco::deltaR(*jetRef, *muon);
      properties.ptRel = ((jetvect - muonvect).Cross(muonvect)).R() / jetvect.R();  // | (Pj-Pu) X Pu | / | Pj |
      float mag = muonvect.R() * jetvect.R();
      float dot = muon->p4().Dot(jetRef->p4());
      properties.etaRel = -log((mag - dot) / (mag + dot)) / 2.;
      properties.ratio = muon->pt() / jetRef->pt();
      properties.ratioRel = muon->p4().Dot(jetRef->p4()) / jetvect.Mag2();
      properties.p0Par = boostedPPar(muon->momentum(), jetRef->momentum());
      properties.charge = muon->charge();

      if (std::abs(properties.sip3dsig) > SIPsigcut)
        continue;

      // Filter leptons from W, Z decays
      if (useFilter &&
          ((std::abs(properties.sip3dsig) < IPsigcut && properties.ratio > ratio1cut) || properties.ratio > ratio2cut))
        continue;

      // Insert lepton properties
      tagInfo.insert(lepPtr, properties);

    }  // --- End loop on daughters

    // Fill the TagInfo collection
    theMuonTagInfo.push_back(tagInfo);
  }  // --- End loop on jets

  // Put the TagInfo collection in the event
  iEvent.emplace(putToken, std::move(theMuonTagInfo));
}

// compute the lepton momentum along the jet axis, in the jet rest frame
float SoftPFMuonTagInfoProducer::boostedPPar(const math::XYZVector& vector, const math::XYZVector& axis) const {
  static const double lepton_mass = 0.00;  // assume a massless (ultrarelativistic) lepton
  static const double jet_mass = 5.279;    // use B±/B0 mass as the jet rest mass [PDG 2007 updates]
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzM4D<double> > lepton(
      vector.Dot(axis) / axis.r(), ROOT::Math::VectorUtil::Perp(vector, axis), 0., lepton_mass);
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzM4D<double> > jet(axis.r(), 0., 0., jet_mass);
  ROOT::Math::BoostX boost(-jet.Beta());
  return boost(lepton).x();
}

DEFINE_FWK_MODULE(SoftPFMuonTagInfoProducer);
