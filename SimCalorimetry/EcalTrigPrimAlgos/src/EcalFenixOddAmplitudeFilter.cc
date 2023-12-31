#include "CondFormats/EcalObjects/interface/EcalTPGGroups.h"
#include "CondFormats/EcalObjects/interface/EcalTPGOddWeightGroup.h"
#include "CondFormats/EcalObjects/interface/EcalTPGOddWeightIdMap.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <SimCalorimetry/EcalTrigPrimAlgos/interface/EcalFenixOddAmplitudeFilter.h>
#include <iostream>
#include <string>
#include <fstream>

EcalFenixOddAmplitudeFilter::EcalFenixOddAmplitudeFilter(bool tpInfoPrintout)
    : inputsAlreadyIn_(0), stripid_{0}, shift_(6), tpInfoPrintout_(tpInfoPrintout) {}

EcalFenixOddAmplitudeFilter::~EcalFenixOddAmplitudeFilter() {}

int EcalFenixOddAmplitudeFilter::setInput(int input) {
  if (input > 0X3FFFF) {
    edm::LogError("EcalTPG") << "ERROR IN INPUT OF ODD AMPLITUDE FILTER";
    return -1;
  }
  if (inputsAlreadyIn_ < 5) {
    buffer_[inputsAlreadyIn_] = input;
    inputsAlreadyIn_++;
  } else {
    for (int i = 0; i < 4; i++) {
      buffer_[i] = buffer_[i + 1];
    }
    buffer_[4] = input;
  }
  return 1;
}

void EcalFenixOddAmplitudeFilter::process(std::vector<int> &addout, std::vector<int> &output) {
  inputsAlreadyIn_ = 0;
  for (unsigned int i = 0; i < 5; i++) {
    buffer_[i] = 0;
  }

  for (unsigned int i = 0; i < addout.size(); i++) {
    setInput(addout[i]);
    process();
    if (tpInfoPrintout_) {
      if (i >= 4) {
        edm::LogVerbatim("EcalTPG") << i << " " << stripid_ << " " << weights_[0] << " " << weights_[1] << " "
                                    << weights_[2] << " " << weights_[3] << " " << weights_[4] << " "
                                    << weights_[0] / 64.0 << " " << weights_[1] / 64.0 << " " << weights_[2] / 64.0
                                    << " " << weights_[3] / 64.0 << " " << weights_[4] / 64.0 << " [" << buffer_[0]
                                    << ", " << buffer_[1] << ", " << buffer_[2] << ", " << buffer_[3] << ", "
                                    << buffer_[4] << "]"
                                    << " --> output: " << processedOutput_ << " ODD";
      }
    }
    output[i] = processedOutput_;
  }
  // shift the result by 1!
  for (unsigned int i = 0; i < (output.size()); i++) {
    if (i != output.size() - 1) {
      output[i] = output[i + 1];
    } else {
      output[i] = 0;
    }
  }
  return;
}

void EcalFenixOddAmplitudeFilter::process() {
  processedOutput_ = 0;
  if (inputsAlreadyIn_ < 5)  // 5 digis required to produce first ET value
    return;
  int output = 0;

  for (int i = 0; i < 5; i++) {
    output += (weights_[i] * buffer_[i]) >> shift_;
  }

  if (output < 0)
    output = 0;
  if (output > 0X3FFFF)
    output = 0X3FFFF;
  processedOutput_ = output;
}

void EcalFenixOddAmplitudeFilter::setParameters(uint32_t raw,
                                                const EcalTPGOddWeightIdMap *ecaltpgOddWeightMap,
                                                const EcalTPGOddWeightGroup *ecaltpgOddWeightGroup) {
  stripid_ = raw;
  uint32_t params_[5];
  const EcalTPGGroups::EcalTPGGroupsMap &groupmap = ecaltpgOddWeightGroup->getMap();
  EcalTPGGroups::EcalTPGGroupsMapItr it = groupmap.find(raw);
  if (it != groupmap.end()) {
    uint32_t weightid = (*it).second;
    const EcalTPGOddWeightIdMap::EcalTPGWeightMap &weightmap = ecaltpgOddWeightMap->getMap();
    EcalTPGOddWeightIdMap::EcalTPGWeightMapItr itw = weightmap.find(weightid);
    (*itw).second.getValues(params_[0], params_[1], params_[2], params_[3], params_[4]);

    for (int i = 0; i < 5; ++i) {
      weights_[i] = (params_[i] & 0x40) ? (int)(params_[i] | 0xffffffc0) : (int)(params_[i]);
    }
  } else
    edm::LogWarning("EcalTPG") << " could not find EcalTPGGroupsMap entry for " << raw;
}
