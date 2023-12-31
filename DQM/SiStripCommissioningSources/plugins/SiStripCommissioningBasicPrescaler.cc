// -*- C++ -*-
//
// Package:    SiStripCommissioningBasicPrescaler
// Class:      SiStripCommissioningBasicPrescaler
//
/**\class SiStripCommissioningBasicPrescaler SiStripCommissioningBasicPrescaler.cc myTestArea/SiStripCommissioningBasicPrescaler/src/SiStripCommissioningBasicPrescaler.cc

 Description: simply filter acording to the run type

 Implementation:
     Uses information from SiStripEventSummary, so it has to be called after Raw2Digi.
*/
//
// Original Author:  Christophe DELAERE
//         Created:  Fri Jan 18 12:17:46 CET 2008
//
//

// system include files
#include <memory>
#include <vector>

// user include files
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

//
// class declaration
//

class SiStripCommissioningBasicPrescaler : public edm::stream::EDFilter<> {
public:
  explicit SiStripCommissioningBasicPrescaler(const edm::ParameterSet&);
  ~SiStripCommissioningBasicPrescaler() override = default;

private:
  bool filter(edm::Event&, const edm::EventSetup&) override;

  // ----------member data ---------------------------
  uint32_t factor_;
};

//
// constructors and destructor
//
SiStripCommissioningBasicPrescaler::SiStripCommissioningBasicPrescaler(const edm::ParameterSet& iConfig) {
  //now do what ever initialization is needed
  factor_ = iConfig.getParameter<uint32_t>("ScaleFactor");
}

//
// member functions
//

// ------------ method called on each new Event  ------------
bool SiStripCommissioningBasicPrescaler::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  bool result = ((iEvent.id().event() % factor_) == 0);
  return result;
}

DEFINE_FWK_MODULE(SiStripCommissioningBasicPrescaler);
