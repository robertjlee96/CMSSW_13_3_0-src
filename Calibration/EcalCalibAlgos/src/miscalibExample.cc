// -*- C++ -*-
//
// Package:    miscalibExample
// Class:      miscalibExample
//
/**\class miscalibExample miscalibExample.cc Calibration/EcalCalibAlgos/src/miscalibExample.cc

 Description: Perform single electron calibration (tested on TB data only).

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Lorenzo AGOSTINO
//         Created:  Tue Jul 18 12:17:01 CEST 2006
//
//

// system include files

// user include files
#include "Calibration/EcalCalibAlgos/interface/miscalibExample.h"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <iostream>
#include <stdexcept>
#include <vector>

miscalibExample::miscalibExample(const edm::ParameterSet& iConfig)
    : rootfile_{iConfig.getUntrackedParameter<std::string>("rootfile", "ecalSimpleTBanalysis.root")},
      correctedHybridSuperClusterProducer_{iConfig.getParameter<std::string>("correctedHybridSuperClusterProducer")},
      correctedHybridSuperClusterCollection_{
          iConfig.getParameter<std::string>("correctedHybridSuperClusterCollection")},
      correctedHybridSuperClusterToken_{consumes<reco::SuperClusterCollection>(
          edm::InputTag(correctedHybridSuperClusterProducer_, correctedHybridSuperClusterCollection_))} {}

miscalibExample::~miscalibExample() {}

//========================================================================
void miscalibExample::beginJob() {
  //========================================================================

  // Book histograms
  scEnergy = new TH1F("scEnergy", "SuperCluster energy", 100, 20., 80.);
  read_events = 0;
}

//========================================================================
void miscalibExample::endJob() {
  //========================================================================

  std::cout << "************* STATISTICS **************" << std::endl;
  std::cout << "Read Events: " << read_events << std::endl;

  /////////////////////////////////////////////////////////////////////////////

  TFile f(rootfile_.c_str(), "RECREATE");

  scEnergy->Write();
  f.Close();
  delete scEnergy;
}

//=================================================================================
void miscalibExample::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  //=================================================================================
  using namespace edm;
  using namespace std;

  read_events++;

  // Get hybrid super clusters after energy correction

  Handle<reco::SuperClusterCollection> pCorrectedHybridSuperClusters;

  iEvent.getByToken(correctedHybridSuperClusterToken_, pCorrectedHybridSuperClusters);

  if (!pCorrectedHybridSuperClusters.isValid()) {
    LogError("EgammaSimpleAnalyzer") << "Error! can't get collection with label "
                                     << correctedHybridSuperClusterCollection_.c_str();
  }
  const reco::SuperClusterCollection* correctedHybridSuperClusters = pCorrectedHybridSuperClusters.product();

  reco::SuperClusterCollection::const_iterator superClusterIt;
  for (superClusterIt = correctedHybridSuperClusters->begin(); superClusterIt != correctedHybridSuperClusters->end();
       superClusterIt++) {
    scEnergy->Fill(superClusterIt->energy());
  }
}
