#ifndef DTSegment_DTRecSegment2DProducer_h
#define DTSegment_DTRecSegment2DProducer_h

/** \class DTRecSegment2DProducer
 *
 * Producer for DT segment in one projection.
 *  
 * \author Stefano Lacaprara - INFN Legnaro <stefano.lacaprara@pd.infn.it>
 * \author Riccardo Bellan - INFN TO <riccardo.bellan@cern.ch>
 *
 */

/* Base Class Headers */
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "DataFormats/DTRecHit/interface/DTRecHitCollection.h"

namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}  // namespace edm

class DTRecSegment2DBaseAlgo;
class DTGeometry;
class MuonGeometryRecord;

/* C++ Headers */

/* ====================================================================== */

/* Class DTRecSegment2DProducer Interface */

class DTRecSegment2DProducer : public edm::stream::EDProducer<> {
public:
  /// Constructor
  DTRecSegment2DProducer(const edm::ParameterSet&);

  /// Destructor
  ~DTRecSegment2DProducer() override;

  // Operations

  /// The method which produces the 2D-segments
  void produce(edm::Event& event, const edm::EventSetup& setup) override;

protected:
private:
  // Switch on verbosity
  bool debug;

  // The 2D-segments reconstruction algorithm
  std::unique_ptr<DTRecSegment2DBaseAlgo> theAlgo;

  edm::EDGetTokenT<DTRecHitCollection> recHits1DToken_;
  const edm::ESGetToken<DTGeometry, MuonGeometryRecord> dtGeomToken_;
};
#endif  // DTRecHit_DTRecSegment2DProducer_h
