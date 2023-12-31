// system include files
#include <iostream>
#include <cstdio>
#include <sys/time.h>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "CondFormats/SiStripObjects/interface/SiStripNoises.h"
#include "CondFormats/DataRecord/interface/SiStripNoisesRcd.h"

class SiStripNoisesReader : public edm::one::EDAnalyzer<> {
public:
  explicit SiStripNoisesReader(const edm::ParameterSet&);
  ~SiStripNoisesReader() override;

  void analyze(const edm::Event&, const edm::EventSetup&) override;

private:
  const uint32_t printdebug_;
  const edm::ESGetToken<SiStripNoises, SiStripNoisesRcd> noisesToken_;
};

using namespace cms;

SiStripNoisesReader::SiStripNoisesReader(const edm::ParameterSet& iConfig)
    : printdebug_(iConfig.getUntrackedParameter<uint32_t>("printDebug", 1)), noisesToken_(esConsumes()) {}

SiStripNoisesReader::~SiStripNoisesReader() = default;

void SiStripNoisesReader::analyze(const edm::Event& e, const edm::EventSetup& iSetup) {
  const auto& stripNoises = iSetup.getData(noisesToken_);
  edm::LogInfo("SiStripNoisesReader") << "[SiStripNoisesReader::analyze] End Reading SiStripNoises" << std::endl;

  std::vector<uint32_t> detid;
  stripNoises.getDetIds(detid);
  edm::LogInfo("Number of detids ") << detid.size() << std::endl;

  if (printdebug_)
    for (size_t id = 0; id < detid.size() && id < printdebug_; id++) {
      SiStripNoises::Range range = stripNoises.getRange(detid[id]);

      int strip = 0;
      for (int it = 0; it < (range.second - range.first) * 8 / 9; it++) {
        edm::LogInfo("SiStripNoisesReader") << "detid " << detid[id] << " \t"
                                            << " strip " << strip++ << " \t" << stripNoises.getNoise(it, range)
                                            << " \t"
                                            //<< stripNoises.getDisable(it,range)   << " \t"
                                            << std::endl;
      }
    }
}

#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(SiStripNoisesReader);
