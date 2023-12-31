#ifndef HIPixelClusterVtxProducer_H
#define HIPixelClusterVtxProducer_H

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHitCollection.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

namespace edm {
  class Run;
  class Event;
  class EventSetup;
}  // namespace edm

class TrackerGeometry;

class HIPixelClusterVtxProducer : public edm::stream::EDProducer<> {
public:
  explicit HIPixelClusterVtxProducer(const edm::ParameterSet& ps);
  ~HIPixelClusterVtxProducer() override;

private:
  struct VertexHit {
    float z;
    float r;
    float w;
  };

  void produce(edm::Event& ev, const edm::EventSetup& es) override;
  int getContainedHits(const std::vector<VertexHit>& hits, double z0, double& chi);

  std::string srcPixelsString_;  //pixel rec hits
  edm::EDGetTokenT<SiPixelRecHitCollection> srcPixels_;
  edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> trackerToken_;

  double minZ_;
  double maxZ_;
  double zStep_;
};
#endif
