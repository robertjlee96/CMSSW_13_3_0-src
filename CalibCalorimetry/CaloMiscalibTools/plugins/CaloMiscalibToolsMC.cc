// -*- C++ -*-
//
// Package:    CaloMiscalibToolsMC
// Class:      CaloMiscalibToolsMC
//
/**\class CaloMiscalibToolsMC CaloMiscalibToolsMC.h CalibCalorimetry/CaloMiscalibToolsMC/interface/CaloMiscalibToolsMC.h

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Lorenzo AGOSTINO
//         Created:  Wed May 31 10:37:45 CEST 2006
//
// Modified       : Luca Malgeri
// Date:          : 11/09/2006
// Reason         : split class definition (.h) from source code (.cc)
//
//

// system include files

// user include files
#include "CalibCalorimetry/CaloMiscalibTools/interface/CaloMiscalibToolsMC.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "CalibCalorimetry/CaloMiscalibTools/interface/MiscalibReaderFromXMLEcalBarrel.h"
#include "CalibCalorimetry/CaloMiscalibTools/interface/MiscalibReaderFromXMLEcalEndcap.h"

//
// constructors and destructor
//
CaloMiscalibToolsMC::CaloMiscalibToolsMC(const edm::ParameterSet& iConfig) {
  //the following line is needed to tell the framework what
  // data is being produced

  barrelfileinpath_ = iConfig.getUntrackedParameter<std::string>("fileNameBarrel", "");
  endcapfileinpath_ = iConfig.getUntrackedParameter<std::string>("fileNameEndcap", "");

  edm::FileInPath barrelfiletmp("CalibCalorimetry/CaloMiscalibTools/data/" + barrelfileinpath_);
  edm::FileInPath endcapfiletmp("CalibCalorimetry/CaloMiscalibTools/data/" + endcapfileinpath_);

  barrelfile_ = barrelfiletmp.fullPath();
  endcapfile_ = endcapfiletmp.fullPath();

  edm::LogVerbatim("CaloMiscalibToolsMC") << "Barrel file is:" << barrelfile_;
  edm::LogVerbatim("CaloMiscalibToolsMC") << "endcap file is:" << endcapfile_;

  // added by Zhen (changed since 1_2_0)
  setWhatProduced(this, &CaloMiscalibToolsMC::produce);
  findingRecord<EcalIntercalibConstantsMCRcd>();
  //now do what ever other initialization is needed
}

CaloMiscalibToolsMC::~CaloMiscalibToolsMC() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}

//
// member functions
//

// ------------ method called to produce the data  ------------
CaloMiscalibToolsMC::ReturnType CaloMiscalibToolsMC::produce(const EcalIntercalibConstantsMCRcd& iRecord) {
  CaloMiscalibMapEcal map;
  map.prefillMap();
  MiscalibReaderFromXMLEcalBarrel barrelreader_(map);
  MiscalibReaderFromXMLEcalEndcap endcapreader_(map);
  if (!barrelfile_.empty())
    barrelreader_.parseXMLMiscalibFile(barrelfile_);
  if (!endcapfile_.empty())
    endcapreader_.parseXMLMiscalibFile(endcapfile_);
  map.print();
  // Added by Zhen, need a new object so to not be deleted at exit
  CaloMiscalibToolsMC::ReturnType mydata = std::make_unique<EcalIntercalibConstantsMC>(map.get());
  return mydata;
}

void CaloMiscalibToolsMC::setIntervalFor(const edm::eventsetup::EventSetupRecordKey&,
                                         const edm::IOVSyncValue&,
                                         edm::ValidityInterval& oValidity) {
  oValidity = edm::ValidityInterval(edm::IOVSyncValue::beginOfTime(), edm::IOVSyncValue::endOfTime());
}
