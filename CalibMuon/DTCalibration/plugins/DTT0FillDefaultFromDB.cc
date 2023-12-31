/*
 *  See header file for a description of this class.
 */

#include "DTT0FillDefaultFromDB.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/MuonDetId/interface/DTWireId.h"
#include "CondFormats/DTObjects/interface/DTT0.h"
#include "CondFormats/DataRecord/interface/DTT0Rcd.h"

#include <string>

using namespace std;
using namespace edm;

namespace dtCalibration {

  DTT0FillDefaultFromDB::DTT0FillDefaultFromDB(const ParameterSet& pset, edm::ConsumesCollector cc)
      : t0Token_(cc.esConsumes<edm::Transition::BeginRun>()),
        t0RefToken_(
            cc.esConsumes<edm::Transition::BeginRun>(edm::ESInputTag("", pset.getParameter<string>("dbLabelRef")))) {}

  DTT0FillDefaultFromDB::~DTT0FillDefaultFromDB() {}

  void DTT0FillDefaultFromDB::setES(const EventSetup& setup) {
    // Get t0 record from DB
    ESHandle<DTT0> t0H = setup.getHandle(t0Token_);
    t0Map_ = &*t0H;
    LogVerbatim("Calibration") << "[DTT0FillDefaultFromDB] T0 version: " << t0H->version();

    // Get reference t0 DB
    ESHandle<DTT0> t0RefH = setup.getHandle(t0RefToken_);
    t0MapRef_ = &*t0RefH;
    LogVerbatim("Calibration") << "[DTT0FillDefaultFromDB] Reference T0 version: " << t0RefH->version();
  }

  DTT0Data DTT0FillDefaultFromDB::correction(const DTWireId& wireId) {
    // Try to access value in default DB
    // If it does not exist return value from reference DB
    // If it does not exist in reference DB, throw exception
    // Could also set to default zero value
    float t0Mean, t0RMS;
    int status = t0Map_->get(wireId, t0Mean, t0RMS, DTTimeUnits::counts);
    if (!status) {
      return DTT0Data(t0Mean, t0RMS);
    } else {
      // Now access reference DB
      float t0MeanRef, t0RMSRef;
      int statusRef = t0MapRef_->get(wireId, t0MeanRef, t0RMSRef, DTTimeUnits::counts);
      if (!statusRef) {
        return DTT0Data(t0MeanRef, t0RMSRef);
      } else {
        //...
        throw cms::Exception("[DTT0FillDefaultFromDB]")
            << "Could not find t0 entry in reference DB for" << wireId << endl;
      }
    }
  }

}  // namespace dtCalibration
