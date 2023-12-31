// -*- C++ -*-
//
// Package:    SiStripTools
// Class:      LargeEvents
//
/**\class LargeEvents LargeEvents.cc DPGAnalysis/SiStripTools/LargeEvents.cc

 Description: templated EDFilter to select events with large number of SiStripDigi or SiStripCluster

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Andrea Venturi
//         Created:  Tue Oct 21 20:55:22 CEST 2008
//
//

// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESWatcher.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/SiStripDigi/interface/SiStripDigi.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/Common/interface/DetSet.h"

#include "CalibFormats/SiStripObjects/interface/SiStripQuality.h"
#include "CalibTracker/Records/interface/SiStripQualityRcd.h"

//
// class declaration
//

template <class T>
class LargeEvents : public edm::global::EDFilter<> {
public:
  explicit LargeEvents(const edm::ParameterSet&);
  ~LargeEvents() override;

private:
  bool filter(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;

  // ----------member data ---------------------------

  const edm::EDGetTokenT<T> _collectionToken;
  const int _absthr;
  const int _modthr;
  const bool _useQuality;
  const edm::ESGetToken<SiStripQuality, SiStripQualityRcd> _qualityToken;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
template <class T>
LargeEvents<T>::LargeEvents(const edm::ParameterSet& iConfig)
    : _collectionToken(consumes<T>(iConfig.getParameter<edm::InputTag>("collectionName"))),
      _absthr(iConfig.getUntrackedParameter<int>("absoluteThreshold")),
      _modthr(iConfig.getUntrackedParameter<int>("moduleThreshold")),
      _useQuality(iConfig.getUntrackedParameter<bool>("useQuality", false)),
      _qualityToken(esConsumes(edm::ESInputTag{"", iConfig.getUntrackedParameter<std::string>("qualityLabel", "")})) {
  //now do what ever initialization is needed
}

template <class T>
LargeEvents<T>::~LargeEvents() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}

//
// member functions
//

// ------------ method called on each new Event  ------------
template <class T>
bool LargeEvents<T>::filter(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
  using namespace edm;

  const SiStripQuality* _qualityHandle = nullptr;

  if (_useQuality) {
    _qualityHandle = &iSetup.getData(_qualityToken);
    LogDebug("SiStripQualityUpdated") << "SiStripQuality has changed and it will be updated";
  }

  Handle<T> digis;
  iEvent.getByToken(_collectionToken, digis);

  int ndigitot = 0;
  for (typename T::const_iterator it = digis->begin(); it != digis->end(); it++) {
    if (!_useQuality || !_qualityHandle->IsModuleBad(it->detId())) {
      if (_modthr < 0 || int(it->size()) < _modthr) {
        ndigitot += it->size();
      }
    }
  }

  if (ndigitot > _absthr) {
    LogDebug("LargeEventSelected") << "event with " << ndigitot << " digi/cluster selected";
    return true;
  }

  return false;
}

//define this as a plug-in
typedef LargeEvents<edm::DetSetVector<SiStripDigi> > LargeSiStripDigiEvents;
typedef LargeEvents<edmNew::DetSetVector<SiStripCluster> > LargeSiStripClusterEvents;
typedef LargeEvents<edmNew::DetSetVector<SiPixelCluster> > LargeSiPixelClusterEvents;

DEFINE_FWK_MODULE(LargeSiStripDigiEvents);
DEFINE_FWK_MODULE(LargeSiStripClusterEvents);
DEFINE_FWK_MODULE(LargeSiPixelClusterEvents);
