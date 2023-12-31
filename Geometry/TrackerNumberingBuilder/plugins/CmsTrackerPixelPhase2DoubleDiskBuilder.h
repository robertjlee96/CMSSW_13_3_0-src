#ifndef Geometry_TrackerNumberingBuilder_CmsTrackerPixelPhase2DoubleDiskBuilder_H
#define Geometry_TrackerNumberingBuilder_CmsTrackerPixelPhase2DoubleDiskBuilder_H

#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerLevelBuilder.h"
#include "FWCore/ParameterSet/interface/types.h"
#include <string>

/**
 * Class which contructs Phase2 Pixel Tracker/Discs.
 */
template <class FilteredView>
class CmsTrackerPixelPhase2DoubleDiskBuilder : public CmsTrackerLevelBuilder<FilteredView> {
private:
  void sortNS(FilteredView&, GeometricDet*) override;
  void buildComponent(FilteredView&, GeometricDet*, const std::string&) override;
};

#endif
