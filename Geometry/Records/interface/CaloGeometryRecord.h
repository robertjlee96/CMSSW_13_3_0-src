#ifndef RECORDS_CALOGEOMETRYRECORD_H
#define RECORDS_CALOGEOMETRYRECORD_H
// -*- C++ -*-
//
// Package:     Records
// Class  :     CaloGeometryRecord
//
//
// Author:      Brian Heltsley
// Created:     Tue April 1, 2008
//

#include "FWCore/Framework/interface/EventSetupRecordImplementation.h"
#include "FWCore/Framework/interface/DependentRecordImplementation.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/Records/interface/EcalBarrelGeometryRecord.h"
#include "Geometry/Records/interface/EcalEndcapGeometryRecord.h"
#include "Geometry/Records/interface/EcalPreshowerGeometryRecord.h"
#include "Geometry/Records/interface/HcalParametersRcd.h"
#include "Geometry/Records/interface/HcalSimNumberingRecord.h"
#include "Geometry/Records/interface/HcalRecNumberingRecord.h"
#include "Geometry/Records/interface/HcalGeometryRecord.h"
#include "Geometry/Records/interface/CaloTowerGeometryRecord.h"
#include "Geometry/Records/interface/ZDCGeometryRecord.h"
#include "Geometry/Records/interface/CastorGeometryRecord.h"
#include "Geometry/Records/interface/HGCalGeometryRecord.h"
#include "FWCore/Utilities/interface/mplVector.h"

class CaloGeometryRecord
    : public edm::eventsetup::DependentRecordImplementation<CaloGeometryRecord,
                                                            edm::mpl::Vector<IdealGeometryRecord,
                                                                             EcalBarrelGeometryRecord,
                                                                             EcalEndcapGeometryRecord,
                                                                             EcalPreshowerGeometryRecord,
                                                                             HcalParametersRcd,
                                                                             HcalSimNumberingRecord,
                                                                             HcalRecNumberingRecord,
                                                                             HcalGeometryRecord,
                                                                             HGCalGeometryRecord,
                                                                             CaloTowerGeometryRecord,
                                                                             CastorGeometryRecord,
                                                                             ZDCGeometryRecord> > {};

#endif /* RECORDS_CALOGEOMETRYRECORD_H */
