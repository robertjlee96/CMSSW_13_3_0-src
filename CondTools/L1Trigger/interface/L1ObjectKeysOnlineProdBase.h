#ifndef CondTools_L1Trigger_L1ObjectKeysOnlineProdBase_h
#define CondTools_L1Trigger_L1ObjectKeysOnlineProdBase_h
// -*- C++ -*-
//
// Package:     L1Trigger
// Class  :     L1ObjectKeysOnlineProdBase
//
/**\class L1ObjectKeysOnlineProdBase L1ObjectKeysOnlineProdBase.h CondTools/L1Trigger/interface/L1ObjectKeysOnlineProdBase.h

 Description: Abstract base class for producers that navigate OMDS to get
 object keys for a given subsystem key.  Each base class should be configured
 (via ParameterSet) with a different subsystem label.

 Usage:
    <usage>

*/
//
// Original Author:  Werner Sun
//         Created:  Fri Aug 22 19:47:58 CEST 2008
// $Id$
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "CondFormats/L1TObjects/interface/L1TriggerKey.h"
#include "CondFormats/DataRecord/interface/L1TriggerKeyRcd.h"

#include "CondTools/L1Trigger/interface/OMDSReader.h"

// forward declarations

class L1ObjectKeysOnlineProdBase : public edm::ESProducer {
public:
  L1ObjectKeysOnlineProdBase(const edm::ParameterSet&);
  ~L1ObjectKeysOnlineProdBase() override;

  typedef std::unique_ptr<L1TriggerKey> ReturnType;
  typedef std::unique_ptr<L1TriggerKey>::pointer FillType;

  ReturnType produce(const L1TriggerKeyRcd&);

  virtual void fillObjectKeys(FillType) = 0;

private:
  // ----------member data ---------------------------
protected:
  l1t::OMDSReader m_omdsReader;
  edm::ESGetToken<L1TriggerKey, L1TriggerKeyRcd> l1TriggerKeyToken_;
};

#endif
