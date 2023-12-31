#ifndef HcalAutoPedestalValidator_h
#define HcalAutoPedestalValidator_h

// S.Won
// Code to check pedestals to compare to previous pedestals

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondFormats/HcalObjects/interface/HcalPedestals.h"
#include "CondFormats/HcalObjects/interface/HcalPedestalWidths.h"
#include "CondFormats/HcalObjects/interface/HcalGains.h"
#include "CondFormats/HcalObjects/interface/HcalGainWidths.h"
#include "CondFormats/HcalObjects/interface/HcalElectronicsMap.h"

#include "DataFormats/HcalDetId/interface/HcalGenericDetId.h"
#include "DataFormats/DetId/interface/DetId.h"

#include "CondFormats/DataRecord/interface/HcalPedestalsRcd.h"
#include "CondFormats/DataRecord/interface/HcalPedestalWidthsRcd.h"
#include "CondFormats/DataRecord/interface/HcalGainsRcd.h"
#include "CondFormats/DataRecord/interface/HcalGainWidthsRcd.h"
#include "CondFormats/DataRecord/interface/HcalElectronicsMapRcd.h"

#include "CalibCalorimetry/HcalAlgos/interface/HcalDbASCIIIO.h"
#include "Geometry/HcalTowerAlgo/interface/HcalGeometry.h"

class HcalAutoPedestalValidator : public edm::one::EDAnalyzer<> {
public:
  HcalAutoPedestalValidator(edm::ParameterSet const& ps);

  ~HcalAutoPedestalValidator() override;

  void analyze(const edm::Event& ev, const edm::EventSetup& es) override;

private:
  std::string outfile;
  double epsilon;
  edm::ESGetToken<HcalPedestals, HcalPedestalsRcd> m_tok1;
  edm::ESGetToken<HcalPedestals, HcalPedestalsRcd> m_tok2;
  //  vecDetId getMissingDetIds(std::vector<HcalPedestals> &);
};
#endif
