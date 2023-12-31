#ifndef CalibMuon_DTTTrigCorrectionFactory_H
#define CalibMuon_DTTTrigCorrectionFactory_H

/** \class DTTTrigCorrectionFactory
 *  Factory of seal plugins for TTrig DB corrections.
 *  The plugins are concrete implementations of DTTTrigBaseCorrection case class.
 *
 *  \author A. Vilela Pereira
 */
#include "FWCore/PluginManager/interface/PluginFactory.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

namespace edm {
  class ParameterSet;
  class ConsumesCollector;
}  // namespace edm
namespace dtCalibration {
  class DTTTrigBaseCorrection;
}

typedef edmplugin::PluginFactory<dtCalibration::DTTTrigBaseCorrection *(const edm::ParameterSet &,
                                                                        edm::ConsumesCollector)>
    DTTTrigCorrectionFactory;
#endif
