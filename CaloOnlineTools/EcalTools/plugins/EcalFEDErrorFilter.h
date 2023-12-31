// -*- C++ -*-
//
// Package:    EcalFEDErrorFilter
// Class:      EcalFEDErrorFilter
//
/**\class EcalFEDErrorFilter EcalFEDErrorFilter.cc filter/EcalFEDErrorFilter/src/EcalFEDErrorFilter.cc

Description: <one line class summary>

Implementation:
<Notes on implementation>
*/
//
// Original Author:  Giovanni FRANZONI
//         Created:  Tue Jan 22 13:55:00 CET 2008
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "HLTrigger/HLTcore/interface/HLTFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include <string>
#include <iostream>
#include <vector>
#include <iomanip>

#include <DataFormats/FEDRawData/interface/FEDRawData.h>
#include <DataFormats/FEDRawData/interface/FEDRawDataCollection.h>
#include <DataFormats/FEDRawData/interface/FEDNumbering.h>
//
// class declaration
//

class EcalFEDErrorFilter : public HLTFilter {
public:
  explicit EcalFEDErrorFilter(const edm::ParameterSet&);
  ~EcalFEDErrorFilter() override;

private:
  bool hltFilter(edm::Event&,
                 const edm::EventSetup&,
                 trigger::TriggerFilterObjectWithRefs& filterproduct) const override;

  // ----------member data ---------------------------

  edm::EDGetTokenT<FEDRawDataCollection> dataToken_;
  std::vector<int> fedUnpackList_;
};
