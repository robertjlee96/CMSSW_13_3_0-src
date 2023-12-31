// -*- C++ -*-
//
// Package:    SiStripCommissioningRunTypeFilter
// Class:      SiStripCommissioningRunTypeFilter
//
/**\class SiStripCommissioningRunTypeFilter SiStripCommissioningRunTypeFilter.cc myTestArea/SiStripCommissioningRunTypeFilter/src/SiStripCommissioningRunTypeFilter.cc

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
#include <algorithm>
#include <vector>

// user include files
#include "DataFormats/SiStripCommon/interface/SiStripEnumsAndStrings.h"
#include "DataFormats/SiStripCommon/interface/SiStripEventSummary.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/InputTag.h"

//
// class declaration
//
class SiStripEventSummary;

class SiStripCommissioningRunTypeFilter : public edm::stream::EDFilter<> {
public:
  explicit SiStripCommissioningRunTypeFilter(const edm::ParameterSet&);
  ~SiStripCommissioningRunTypeFilter() override = default;

private:
  bool filter(edm::Event&, const edm::EventSetup&) override;

  // ----------member data ---------------------------
  //      edm::InputTag inputModuleLabel_;
  edm::EDGetTokenT<SiStripEventSummary> summaryToken_;
  std::vector<sistrip::RunType> runTypes_;
};

//
// constructors and destructor
//
SiStripCommissioningRunTypeFilter::SiStripCommissioningRunTypeFilter(const edm::ParameterSet& iConfig) {
  //now do what ever initialization is needed
  summaryToken_ = consumes<SiStripEventSummary>(iConfig.getParameter<edm::InputTag>("InputModuleLabel"));
  //   inputModuleLabel_ = iConfig.getParameter<edm::InputTag>( "InputModuleLabel" ) ;
  std::vector<std::string> runTypes = iConfig.getParameter<std::vector<std::string> >("runTypes");
  for (std::vector<std::string>::const_iterator run = runTypes.begin(); run != runTypes.end(); ++run) {
    runTypes_.push_back(SiStripEnumsAndStrings::runType(*run));
  }
}

//
// member functions
//

// ------------ method called on each new Event  ------------
bool SiStripCommissioningRunTypeFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  // Retrieve commissioning information from "event summary"
  edm::Handle<SiStripEventSummary> summary;
  iEvent.getByToken(summaryToken_, summary);
  return (std::find(runTypes_.begin(), runTypes_.end(), summary->runType()) != runTypes_.end());
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(SiStripCommissioningRunTypeFilter);
