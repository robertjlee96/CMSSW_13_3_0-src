#ifndef Validation_EcalClusters_ContainmentCorrectionAnalyzer_h
#define Validation_EcalClusters_ContainmentCorrectionAnalyzer_h

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"
//
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "Geometry/Records/interface/CaloTopologyRecord.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertexContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
//
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Math/interface/LorentzVector.h"
//
#include "Validation/EcalClusters/interface/EcalSimPhotonMCTruth.h"
//
#include "CLHEP/Units/GlobalPhysicalConstants.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>
class TFile;

class EcalSimPhotonMCTruth;

class ContainmentCorrectionAnalyzer : public edm::one::EDAnalyzer<> {
public:
  explicit ContainmentCorrectionAnalyzer(const edm::ParameterSet &);
  ~ContainmentCorrectionAnalyzer() override;

  void analyze(const edm::Event &, const edm::EventSetup &) override;
  void beginJob() override;
  void endJob() override;

private:
  float ecalEta(float EtaParticle, float Zvertex, float plane_Radius);
  std::vector<EcalSimPhotonMCTruth> findMcTruth(std::vector<SimTrack> &theSimTracks,
                                                std::vector<SimVertex> &theSimVertices);
  void fillMcTruth(std::vector<SimTrack> &theSimTracks, std::vector<SimVertex> &theSimVertices);

  edm::EDGetTokenT<edm::SimTrackContainer> SimTrackCollection_;
  edm::EDGetTokenT<edm::SimVertexContainer> SimVertexCollection_;
  edm::EDGetTokenT<reco::SuperClusterCollection> BarrelSuperClusterCollection_;
  edm::EDGetTokenT<reco::SuperClusterCollection> EndcapSuperClusterCollection_;
  edm::EDGetTokenT<EcalRecHitCollection> reducedBarrelRecHitCollection_;
  edm::EDGetTokenT<EcalRecHitCollection> reducedEndcapRecHitCollection_;
  edm::ESGetToken<CaloTopology, CaloTopologyRecord> pTopologyToken;

  int nMCphotons;
  std::vector<float> mcEnergy, mcEta, mcPhi, mcPt;
  std::vector<int> isConverted;
  std::vector<float> x_vtx, y_vtx, z_vtx;
  int nRECOphotons;
  std::vector<float> superClusterEnergy, superClusterEta, superClusterPhi, superClusterEt;
  std::vector<float> e1, e9, e25;
  std::vector<int> seedXtal;
  std::vector<int> iMC;

  std::map<unsigned, unsigned> geantToIndex_;

  TH1F *h_EB_eRecoEtrueReference;
  TH1F *h_EB_e9EtrueReference;
  TH1F *h_EB_e25EtrueReference;
  TH1F *h_EE_eRecoEtrueReference;
  TH1F *h_EE_e9EtrueReference;
  TH1F *h_EE_e25EtrueReference;
  TH1F *h_EB_eTrue;
  TH1F *h_EE_eTrue;
  TH1F *h_EB_converted;
  TH1F *h_EE_converted;
};
#endif
