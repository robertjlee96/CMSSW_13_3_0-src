// -*- C++ -*-
//
// Package:    L1TriggerKeyOnlineProd
// Class:      L1TriggerKeyOnlineProd
//
/**\class L1TriggerKeyOnlineProd L1TriggerKeyOnlineProd.h CondTools/L1TriggerKeyOnlineProd/src/L1TriggerKeyOnlineProd.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Werner Man-Li Sun
//         Created:  Sun Mar  2 03:03:32 CET 2008
// $Id: L1TriggerKeyOnlineProd.cc,v 1.10 2008/10/13 01:49:48 wsun Exp $
//
//

// system include files

// user include files
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "CondTools/L1Trigger/plugins/L1TriggerKeyOnlineProd.h"

#include "CondTools/L1Trigger/interface/Exception.h"

#include "FWCore/Framework/interface/EventSetup.h"

//
// class declaration
//

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
L1TriggerKeyOnlineProd::L1TriggerKeyOnlineProd(const edm::ParameterSet& iConfig)
    : m_subsystemLabels(iConfig.getParameter<std::vector<std::string> >("subsystemLabels")) {
  //the following line is needed to tell the framework what
  // data is being produced
  auto cc = setWhatProduced(this);

  //now do what ever other initialization is needed
  l1TriggerKeyToken_ = cc.consumes(edm::ESInputTag("", "SubsystemKeysOnly"));
  for (const auto& mlabel : m_subsystemLabels) {
    l1TriggerKeyTokenVec_.emplace_back(cc.consumes(edm::ESInputTag("", mlabel)));
  }
}

L1TriggerKeyOnlineProd::~L1TriggerKeyOnlineProd() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}

//
// member functions
//

// ------------ method called to produce the data  ------------
L1TriggerKeyOnlineProd::ReturnType L1TriggerKeyOnlineProd::produce(const L1TriggerKeyRcd& iRecord) {
  // Start with "SubsystemKeysOnly"
  edm::ESHandle<L1TriggerKey> subsystemKeys;
  try {
    subsystemKeys = iRecord.getHandle(l1TriggerKeyToken_);
  } catch (l1t::DataAlreadyPresentException& ex) {
    throw ex;
  }

  std::unique_ptr<L1TriggerKey> pL1TriggerKey = std::make_unique<L1TriggerKey>(*subsystemKeys);

  // Collate object keys
  for (const auto& l1token : l1TriggerKeyTokenVec_) {
    edm::ESHandle<L1TriggerKey> objectKeys;
    try {
      objectKeys = iRecord.getHandle(l1token);
    } catch (l1t::DataAlreadyPresentException& ex) {
      throw ex;
    }

    pL1TriggerKey->add(objectKeys->recordToKeyMap());
  }

  return pL1TriggerKey;
}

//define this as a plug-in
//DEFINE_FWK_EVENTSETUP_MODULE(L1TriggerKeyOnlineProd);
