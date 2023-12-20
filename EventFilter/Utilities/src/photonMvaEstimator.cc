#include "EventFilter/Utilities/interface/photonMvaEstimator.h"

#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "CommonTools/MVAUtils/interface/GBRForestTools.h"

photonMvaEstimator::photonMvaEstimator(const edm::FileInPath& weightsfile){
  gbrForest_ = createGBRForest(weightsfile);
}

photonMvaEstimator::~photonMvaEstimator() {}

namespace {
  enum inputIndexes {
      rawEnergy,         // 0
      r9,                // 1
      sigmaIEtaIEta,     // 2
      etaWidth,          // 3
      phiWidth,          // 4
      s4,                // 5
      eta,               // 6
      hOvrE,             // 7
      ecalPFIso,         // 8
  };
}  // namespace

double photonMvaEstimator::computeMva(float rawEnergyIn, float r9In, float sigmaIEtaIEtaIn, float etaWidthIn, float phiWidthIn, float s4In, float etaIn, float hOvrEIn, float ecalPFIsoIn) const {
    float var[9];

    var[rawEnergy] = rawEnergyIn;
    var[r9]= r9In;
    var[sigmaIEtaIEta] = sigmaIEtaIEtaIn;
    var[etaWidth] = etaWidthIn;
    var[phiWidth] = phiWidthIn;
    var[s4] = s4In;
    var[eta] = etaIn;
    var[hOvrE] = hOvrEIn;
    var[ecalPFIso] = ecalPFIsoIn;
    
  return gbrForest_->GetResponse(var);
}
