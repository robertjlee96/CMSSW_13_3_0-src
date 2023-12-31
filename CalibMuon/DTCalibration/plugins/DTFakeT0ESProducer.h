#ifndef _DTFAKET0_H
#define _DTFAKET0_H

/*
 *  See header file for a description of this class.
 *
 *  \author S. Bolognesi - INFN Torino
 */

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/SourceFactory.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DetectorDescription/Core/interface/DDCompactView.h"
#include "DetectorDescription/DDCMS/interface/DDCompactView.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/MuonNumbering/interface/MuonGeometryConstants.h"

//#include <pair>
#include <map>

class DTT0;
class DTT0Rcd;
class DTLayerId;

class DTFakeT0ESProducer : public edm::ESProducer, public edm::EventSetupRecordIntervalFinder {
public:
  DTFakeT0ESProducer(const edm::ParameterSet& pset);

  ~DTFakeT0ESProducer() override;

  std::unique_ptr<DTT0> produce(const DTT0Rcd& iRecord);

private:
  void parseDDD(const DTT0Rcd& iRecord);

  void setIntervalFor(const edm::eventsetup::EventSetupRecordKey&,
                      const edm::IOVSyncValue&,
                      edm::ValidityInterval& oValidity) override;

  std::map<DTLayerId, std::pair<unsigned int, unsigned int> > theLayerIdWiresMap;

  //t0 mean and sigma values read from cfg
  double t0Mean;
  double t0Sigma;

  edm::ESGetToken<DDCompactView, IdealGeometryRecord> cpvTokenDDD_;
  edm::ESGetToken<MuonGeometryConstants, IdealGeometryRecord> mdcToken_;
};
#endif
