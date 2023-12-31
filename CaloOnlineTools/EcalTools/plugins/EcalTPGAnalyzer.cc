// -*- C++ -*-
//
// Class:      EcalTPGAnalyzer
//
//
// Original Author:  Pascal Paganini
//
//

// system include files
#include <memory>
#include <utility>

// user include files
#include "FWCore/Framework/interface/EventSetup.h"

#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"

#include "EcalTPGAnalyzer.h"

#include <TMath.h>
#include <sstream>

using namespace edm;
class CaloSubdetectorGeometry;

EcalTPGAnalyzer::EcalTPGAnalyzer(const edm::ParameterSet& iConfig)
    : tpCollection_(iConfig.getParameter<edm::InputTag>("TPCollection")),
      tpEmulatorCollection_(iConfig.getParameter<edm::InputTag>("TPEmulatorCollection")),
      digiCollectionEB_(iConfig.getParameter<edm::InputTag>("DigiCollectionEB")),
      digiCollectionEE_(iConfig.getParameter<edm::InputTag>("DigiCollectionEE")),
      gtRecordCollectionTag_(iConfig.getParameter<std::string>("GTRecordCollection")),
      l1GtReadoutRecordToken_(consumes<L1GlobalTriggerReadoutRecord>(edm::InputTag(gtRecordCollectionTag_))),
      tpToken_(consumes<EcalTrigPrimDigiCollection>(tpCollection_)),
      tpEmulToken_(consumes<EcalTrigPrimDigiCollection>(tpEmulatorCollection_)),
      ebDigiToken_(consumes<EBDigiCollection>(digiCollectionEB_)),
      eeDigiToken_(consumes<EEDigiCollection>(digiCollectionEE_)),
      eTTMapToken_(esConsumes<edm::Transition::BeginRun>()),
      ebGeometryToken_(esConsumes<edm::Transition::BeginRun>(edm::ESInputTag("", "EcalBarrel"))),
      eeGeometryToken_(esConsumes<edm::Transition::BeginRun>(edm::ESInputTag("", "EcalEndcap"))),
      l1GtMaskToken_(esConsumes()),
      allowTP_(iConfig.getParameter<bool>("ReadTriggerPrimitives")),
      useEE_(iConfig.getParameter<bool>("UseEndCap")),
      print_(iConfig.getParameter<bool>("Print")) {
  // file
  file_ = new TFile("ECALTPGtree.root", "RECREATE");
  file_->cd();

  // tree
  tree_ = new TTree("EcalTPGAnalysis", "EcalTPGAnalysis");

  tree_->Branch("runNb", &treeVariables_.runNb, "runNb/i");                                                //
  tree_->Branch("evtNb", &treeVariables_.evtNb, "evtNb/i");                                                //
  tree_->Branch("bxNb", &treeVariables_.bxNb, "bxNb/i");                                                   //
  tree_->Branch("orbitNb", &treeVariables_.orbitNb, "orbitNb/i");                                          //
  tree_->Branch("nbOfActiveTriggers", &treeVariables_.nbOfActiveTriggers, "nbOfActiveTriggers/i");         //
  tree_->Branch("activeTriggers", treeVariables_.activeTriggers, "activeTriggers[nbOfActiveTriggers]/I");  //

  tree_->Branch("nbOfTowers", &treeVariables_.nbOfTowers, "nbOfTowers/i");             //
  tree_->Branch("ieta", treeVariables_.ieta, "ieta[nbOfTowers]/I");                    //
  tree_->Branch("iphi", treeVariables_.iphi, "iphi[nbOfTowers]/I");                    //
  tree_->Branch("nbOfXtals", treeVariables_.nbOfXtals, "nbOfXtals[nbOfTowers]/I");     //
  tree_->Branch("rawTPData", treeVariables_.rawTPData, "rawTPData[nbOfTowers]/I");     //
  tree_->Branch("rawTPEmul1", treeVariables_.rawTPEmul1, "rawTPEmul1[nbOfTowers]/I");  //
  tree_->Branch("rawTPEmul2", treeVariables_.rawTPEmul2, "rawTPEmul2[nbOfTowers]/I");  //
  tree_->Branch("rawTPEmul3", treeVariables_.rawTPEmul3, "rawTPEmul3[nbOfTowers]/I");  //
  tree_->Branch("rawTPEmul4", treeVariables_.rawTPEmul4, "rawTPEmul4[nbOfTowers]/I");  //
  tree_->Branch("rawTPEmul5", treeVariables_.rawTPEmul5, "rawTPEmul5[nbOfTowers]/I");  //
  tree_->Branch("eRec", treeVariables_.eRec, "eRec[nbOfTowers]/F");                    //
}

EcalTPGAnalyzer::~EcalTPGAnalyzer() {
  file_->cd();
  tree_->Write();
  file_->Close();
}

void EcalTPGAnalyzer::beginRun(edm::Run const&, edm::EventSetup const& evtSetup) {
  // geometry
  eTTmap_ = &evtSetup.getData(eTTMapToken_);
  theBarrelGeometry_ = &evtSetup.getData(ebGeometryToken_);
  theEndcapGeometry_ = &evtSetup.getData(eeGeometryToken_);
}

void EcalTPGAnalyzer::endRun(edm::Run const&, edm::EventSetup const& evtSetup) {}

void EcalTPGAnalyzer::analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) {
  using namespace edm;
  using namespace std;

  if (print_)
    std::cout << "===========" << iEvent.id() << std::endl;

  map<EcalTrigTowerDetId, towerEner> mapTower;
  map<EcalTrigTowerDetId, towerEner>::iterator itTT;

  ///////////////////////////
  // get Evts info
  ///////////////////////////

  treeVariables_.runNb = iEvent.id().run();
  treeVariables_.evtNb = iEvent.id().event();
  treeVariables_.bxNb = iEvent.bunchCrossing();
  treeVariables_.orbitNb = iEvent.orbitNumber();

  ///////////////////////////
  // get L1 info
  ///////////////////////////

  edm::Handle<L1GlobalTriggerReadoutRecord> gtRecord;
  iEvent.getByToken(l1GtReadoutRecordToken_, gtRecord);
  DecisionWord dWord = gtRecord->decisionWord();  // this will get the decision word *before* masking disabled bits

  const auto& l1GtTmAlgo = iSetup.getData(l1GtMaskToken_);
  std::vector<unsigned int> triggerMaskAlgoTrig = l1GtTmAlgo.gtTriggerMask();

  // apply masks on algo
  int iDaq = 0;
  int iBit = -1;
  treeVariables_.nbOfActiveTriggers = 0;
  for (std::vector<bool>::iterator itBit = dWord.begin(); itBit != dWord.end(); ++itBit) {
    iBit++;
    int maskBit = triggerMaskAlgoTrig[iBit] & (1 << iDaq);
    if (maskBit)
      *itBit = false;
    if (*itBit) {
      treeVariables_.activeTriggers[treeVariables_.nbOfActiveTriggers] = iBit;
      treeVariables_.nbOfActiveTriggers++;
    }
  }

  ///////////////////////////
  // Get TP data
  ///////////////////////////

  edm::Handle<EcalTrigPrimDigiCollection> tp;
  iEvent.getByToken(tpToken_, tp);
  if (print_)
    std::cout << "TP collection size=" << tp.product()->size() << std::endl;

  for (unsigned int i = 0; i < tp.product()->size(); i++) {
    EcalTriggerPrimitiveDigi d = (*(tp.product()))[i];
    const EcalTrigTowerDetId TPtowid = d.id();
    towerEner tE;
    tE.iphi_ = TPtowid.iphi();
    tE.ieta_ = TPtowid.ieta();
    tE.tpgADC_ = d[0].raw();
    mapTower[TPtowid] = tE;
  }

  ///////////////////////////
  // Get Emulators TP
  ///////////////////////////

  edm::Handle<EcalTrigPrimDigiCollection> tpEmul;
  iEvent.getByToken(tpEmulToken_, tpEmul);
  if (print_)
    std::cout << "TPEmulator collection size=" << tpEmul.product()->size() << std::endl;

  for (unsigned int i = 0; i < tpEmul.product()->size(); i++) {
    EcalTriggerPrimitiveDigi d = (*(tpEmul.product()))[i];
    const EcalTrigTowerDetId TPtowid = d.id();
    itTT = mapTower.find(TPtowid);
    if (itTT != mapTower.end())
      for (int j = 0; j < 5; j++)
        (itTT->second).tpgEmul_[j] = d[j].raw();
  }

  ///////////////////////////
  // Get nb of crystals read out
  ///////////////////////////

  // Get EB xtal digi inputs
  edm::Handle<EBDigiCollection> digiEB;
  iEvent.getByToken(ebDigiToken_, digiEB);

  for (unsigned int i = 0; i < digiEB.product()->size(); i++) {
    const EBDataFrame& df = (*(digiEB.product()))[i];
    const EBDetId& id = df.id();
    const EcalTrigTowerDetId towid = id.tower();
    itTT = mapTower.find(towid);
    if (itTT != mapTower.end())
      (itTT->second).nbXtal_++;
  }

  if (useEE_) {
    // Get EE xtal digi inputs
    edm::Handle<EEDigiCollection> digiEE;
    iEvent.getByToken(eeDigiToken_, digiEE);
    for (unsigned int i = 0; i < digiEE.product()->size(); i++) {
      const EEDataFrame& df = (*(digiEE.product()))[i];
      const EEDetId& id = df.id();
      const EcalTrigTowerDetId towid = eTTmap_->towerOf(id);
      itTT = mapTower.find(towid);
      if (itTT != mapTower.end())
        (itTT->second).nbXtal_++;
    }
  }

  ///////////////////////////
  // Get rechits
  ///////////////////////////

  //... to be completed

  ///////////////////////////
  // fill tree
  ///////////////////////////

  treeVariables_.nbOfTowers = mapTower.size();
  int towerNb = 0;
  for (itTT = mapTower.begin(); itTT != mapTower.end(); ++itTT) {
    treeVariables_.ieta[towerNb] = (itTT->second).ieta_;
    treeVariables_.iphi[towerNb] = (itTT->second).iphi_;
    treeVariables_.nbOfXtals[towerNb] = (itTT->second).nbXtal_;
    treeVariables_.rawTPData[towerNb] = (itTT->second).tpgADC_;
    treeVariables_.rawTPEmul1[towerNb] = (itTT->second).tpgEmul_[0];
    treeVariables_.rawTPEmul2[towerNb] = (itTT->second).tpgEmul_[1];
    treeVariables_.rawTPEmul3[towerNb] = (itTT->second).tpgEmul_[2];
    treeVariables_.rawTPEmul4[towerNb] = (itTT->second).tpgEmul_[3];
    treeVariables_.rawTPEmul5[towerNb] = (itTT->second).tpgEmul_[4];
    treeVariables_.eRec[towerNb] = (itTT->second).eRec_;
    towerNb++;
  }

  tree_->Fill();
}
