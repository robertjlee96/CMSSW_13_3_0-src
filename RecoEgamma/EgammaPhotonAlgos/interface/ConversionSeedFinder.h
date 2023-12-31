#ifndef RecoEGAMMA_ConversionSeed_ConversionSeedFinder_h
#define RecoEGAMMA_ConversionSeed_ConversionSeedFinder_h
/** \class ConversionSeedFinder
 **  
 **
 **  \author Nancy Marinelli, U. of Notre Dame, US
 **
 ***/
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/TrajectorySeed/interface/PropagationDirection.h"
#include "DataFormats/TrajectorySeed/interface/TrajectorySeedCollection.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/Handle.h"
#include "TrackingTools/MaterialEffects/interface/PropagatorWithMaterial.h"
#include "TrackingTools/GeomPropagators/interface/StraightLinePropagator.h"
#include "TrackingTools/KalmanUpdators/interface/KFUpdator.h"
#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"
#include "TrackingTools/PatternTools/interface/TrajectoryMeasurement.h"
#include "RecoTracker/TkDetLayers/interface/GeometricSearchTracker.h"
#include "RecoTracker/MeasurementDet/interface/MeasurementTracker.h"
#include "RecoTracker/MeasurementDet/interface/MeasurementTrackerEvent.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "DataFormats/Math/interface/Vector3D.h"
#include "DataFormats/Math/interface/Point3D.h"

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"

// C/C++ headers
#include <string>
#include <vector>

//
namespace edm {
  class ConsumesCollector;
}

class DetLayer;
class FreeTrajectoryState;
class TrajectoryStateOnSurface;
class NavigationSchool;
class TrackingGeometry;
class IdealMagneticFieldRecord;
class TrackerRecoGeometryRecord;
class CkfComponentsRecord;
class TrackingComponentsRecord;

class ConversionSeedFinder {
public:
  ConversionSeedFinder();
  ConversionSeedFinder(const edm::ParameterSet& config, edm::ConsumesCollector& iC);

  virtual ~ConversionSeedFinder() {}

  virtual void makeSeeds(const edm::Handle<edm::View<reco::CaloCluster> >& allBc) = 0;

  TrajectorySeedCollection& seeds() { return theSeeds_; }
  virtual void setCandidate(float e, GlobalPoint pos) {
    theSCenergy_ = e;
    theSCPosition_ = pos;
  }
  std::vector<const DetLayer*> const& layerList() const { return theLayerList_; }

  void setMeasurementTracker(const MeasurementTracker* tracker) const { ; }
  const MeasurementTracker* getMeasurementTracker() const { return theMeasurementTracker_; }

  /// Initialize EventSetup objects at each event
  void setEventSetup(const edm::EventSetup& es);
  void setNavigationSchool(const NavigationSchool* navigation) { theNavigationSchool_ = navigation; }
  void setEvent(const edm::Event& e);

  void clear() { theSeeds_.clear(); }

protected:
  //edm::ParameterSet conf_; found this to be completely unused
  void findLayers();
  void findLayers(const FreeTrajectoryState& fts);

  FreeTrajectoryState trackStateFromClusters(int aCharge,
                                             const GlobalPoint& gpOrigine,
                                             PropagationDirection dir,
                                             float scaleFactor) const;

  void printLayer(int i) const;

  TrajectorySeedCollection theSeeds_;
  GlobalPoint theSCPosition_;

  const MeasurementTracker* theMeasurementTracker_;
  const TrackingGeometry* theTrackerGeom_;
  const NavigationSchool* theNavigationSchool_ = nullptr;

  edm::ESHandle<MagneticField> theMF_;
  edm::ESHandle<GeometricSearchTracker> theGeomSearchTracker_;

  edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> theMFToken_;
  edm::ESGetToken<GeometricSearchTracker, TrackerRecoGeometryRecord> theGeomSearchTrackerToken_;
  edm::ESGetToken<MeasurementTracker, CkfComponentsRecord> theMeasurementTrackerToken_;
  edm::ESGetToken<Propagator, TrackingComponentsRecord> thePropagatorAlongMomentumToken_;
  edm::ESGetToken<Propagator, TrackingComponentsRecord> thePropagatorOppositeToMomentumToken_;

  edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;
  edm::EDGetTokenT<MeasurementTrackerEvent> measurementTrkToken_;

  KFUpdator theUpdator_;
  PropagationDirection dir_;
  reco::CaloCluster* theSC_;
  float theSCenergy_;

  std::vector<const DetLayer*> theLayerList_;

  GlobalPoint theBCPosition_;
  float theBCEnergy_;

  const Propagator* thePropagatorAlongMomentum_;
  const Propagator* thePropagatorOppositeToMomentum_;

  reco::BeamSpot theBeamSpot_;
  edm::Handle<MeasurementTrackerEvent> theTrackerData_;
};

#endif
