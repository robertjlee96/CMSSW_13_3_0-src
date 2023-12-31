#ifndef CondTools_Ecal_StoreESCondition_h
#define CondTools_Ecal_StoreESCondition_h

#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include <string>
#include <map>
#include <memory>
#include <fstream>
#include <cstdio>
#include <typeinfo>
#include <sstream>

#include "CondFormats/ESObjects/interface/ESTimeSampleWeights.h"
#include "CondFormats/ESObjects/interface/ESChannelStatus.h"
#include "CondFormats/ESObjects/interface/ESGain.h"
#include "CondFormats/ESObjects/interface/ESThresholds.h"
#include "CondFormats/ESObjects/interface/ESPedestals.h"
#include "CondFormats/ESObjects/interface/ESRecHitRatioCuts.h"
#include "CondFormats/ESObjects/interface/ESIntercalibConstants.h"
#include "CondFormats/ESObjects/interface/ESMissingEnergyCalibration.h"
#include "CondFormats/ESObjects/interface/ESEEIntercalibConstants.h"
#include "DataFormats/EcalDetId/interface/ESDetId.h"

namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}  // namespace edm

class StoreESCondition : public edm::one::EDAnalyzer<> {
public:
  std::shared_ptr<ESThresholds> readESThresholdsFromFile(const char*);
  std::shared_ptr<ESPedestals> readESPedestalsFromFile(const char*);
  std::shared_ptr<ESRecHitRatioCuts> readESRecHitRatioCutsFromFile(const char*);
  std::shared_ptr<ESGain> readESGainFromFile(const char*);
  std::shared_ptr<ESTimeSampleWeights> readESTimeSampleWeightsFromFile(const char*);
  std::shared_ptr<ESChannelStatus> readESChannelStatusFromFile(const char*);
  std::shared_ptr<ESIntercalibConstants> readESIntercalibConstantsFromFile(const char*);
  std::shared_ptr<ESMissingEnergyCalibration> readESMissingEnergyFromFile(const char*);
  std::shared_ptr<ESEEIntercalibConstants> readESEEIntercalibConstantsFromFile(const char*);
  void writeToLogFile(std::string, std::string, unsigned long long);
  void writeToLogFileResults(char*);

  explicit StoreESCondition(const edm::ParameterSet& iConfig);
  ~StoreESCondition() override;

  void analyze(const edm::Event& evt, const edm::EventSetup& evtSetup) override;
  void endJob() override;

private:
  void fillHeader(char*);

  std::vector<std::string> objectName_;
  std::vector<std::string> inpFileName_;
  std::vector<std::string> inpFileNameEE_;
  std::string prog_name_;
  std::vector<unsigned long long> since_;  // beginning IOV for objects
  std::string logfile_;

  unsigned int esgain_;

  std::string to_string(char value[]) {
    std::ostringstream streamOut;
    streamOut << value;
    return streamOut.str();
  }
};

#endif
