#ifndef RecoParticleFlow_PFTracking_ConvBremHeavyObjectCache_H
#define RecoParticleFlow_PFTracking_ConvBremHeavyObjectCache_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "CondFormats/GBRForest/interface/GBRForest.h"
#include "RecoParticleFlow/PFClusterTools/interface/PFEnergyCalibration.h"
#include <memory>

namespace convbremhelpers {
  class HeavyObjectCache {
  public:
    HeavyObjectCache(const edm::ParameterSet&);
    std::unique_ptr<const GBRForest> gbrBarrelLowPt_;
    std::unique_ptr<const GBRForest> gbrBarrelHighPt_;
    std::unique_ptr<const GBRForest> gbrEndcapsLowPt_;
    std::unique_ptr<const GBRForest> gbrEndcapsHighPt_;
    std::unique_ptr<const PFEnergyCalibration> pfcalib_;
  };
}  // namespace convbremhelpers

#endif
