// -*- C++ -*-
//
// Package:    EventSetupIntProductAnalyzer
// Class:      EventSetupIntProductAnalyzer
//
/**\class EventSetupIntProductAnalyzer EventSetupIntProductAnalyzer.cc test/EventSetupIntProductAnalyzer/src/EventSetupIntProductAnalyzer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Chris Jones
//         Created:  Fri Jun 24 19:13:25 EDT 2005
//
//

// system include files
#include <memory>
#include <iostream>
#include <vector>

// user include files
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/TestObjects/interface/ToyProducts.h"
#include "PhysicsTools/CondLiteIO/test/IntProductRecord.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/Exception.h"

//
// class decleration
//

namespace edmtest {

  class EventSetupIntProductAnalyzer : public edm::one::EDAnalyzer<> {
  public:
    explicit EventSetupIntProductAnalyzer(const edm::ParameterSet&);
    ~EventSetupIntProductAnalyzer();

    virtual void analyze(const edm::Event&, const edm::EventSetup&);

  private:
    // ----------member data ---------------------------
    std::vector<int> expectedValues_;
    unsigned int index_;
    edm::ESGetToken<edmtest::IntProduct, IntProductRecord> token_;
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
  EventSetupIntProductAnalyzer::EventSetupIntProductAnalyzer(const edm::ParameterSet& iConfig)
      : expectedValues_(iConfig.getUntrackedParameter<std::vector<int> >("expectedValues", std::vector<int>())),
        index_(0),
        token_(esConsumes()) {
    //now do what ever initialization is needed
  }

  EventSetupIntProductAnalyzer::~EventSetupIntProductAnalyzer() {
    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)
  }

  //
  // member functions
  //

  // ------------ method called to produce the data  ------------
  void EventSetupIntProductAnalyzer::analyze(const edm::Event& /*iEvent*/, const edm::EventSetup& iSetup) {
    using namespace edm;
    auto const& setup = iSetup.getData(token_);

    std::cout << "edmtest::IntProduct " << setup.value << std::endl;
    if (!expectedValues_.empty()) {
      if (expectedValues_.at(index_) != setup.value) {
        throw cms::Exception("TestFail") << "expected value " << expectedValues_[index_] << " but was got "
                                         << setup.value;
      }
      ++index_;
    }
  }
}  // namespace edmtest
using namespace edmtest;
//define this as a plug-in
DEFINE_FWK_MODULE(EventSetupIntProductAnalyzer);
