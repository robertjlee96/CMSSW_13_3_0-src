/** \file
 *
 * \author Stefano Lacaprara - INFN Legnaro <stefano.lacaprara@pd.infn.it>
 * \author Riccardo Bellan - INFN TO <riccardo.bellan@cern.ch>
 */

/* This Class Header */
#include "RecoLocalMuon/DTSegment/src/DTRecSegment2DProducer.h"

/* Collaborating Class Header */
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
using namespace edm;

#include "Geometry/DTGeometry/interface/DTLayer.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "DataFormats/DTRecHit/interface/DTRecHit1DPair.h"
#include "DataFormats/DTRecHit/interface/DTRecHit1D.h"
#include "DataFormats/DTRecHit/interface/DTSLRecSegment2D.h"
#include "DataFormats/DTRecHit/interface/DTRecSegment2DCollection.h"
#include "DataFormats/DTRecHit/interface/DTRangeMapAccessor.h"

#include "RecoLocalMuon/DTSegment/src/DTRecSegment2DAlgoFactory.h"

/* C++ Headers */
#include <string>
using namespace std;

/* ====================================================================== */

/// Constructor
DTRecSegment2DProducer::DTRecSegment2DProducer(const edm::ParameterSet& pset)
    : theAlgo{DTRecSegment2DAlgoFactory::get()->create(pset.getParameter<string>("Reco2DAlgoName"),
                                                       pset.getParameter<ParameterSet>("Reco2DAlgoConfig"),
                                                       consumesCollector())},
      dtGeomToken_{esConsumes()} {
  // Set verbose output
  debug = pset.getUntrackedParameter<bool>("debug");

  // the name of the 1D rec hits collection
  recHits1DToken_ = consumes<DTRecHitCollection>(pset.getParameter<InputTag>("recHits1DLabel"));

  if (debug)
    cout << "[DTRecSegment2DProducer] Constructor called" << endl;

  produces<DTRecSegment2DCollection>();

  // Get the concrete reconstruction algo from the factory
  if (debug)
    cout << "the Reco2D AlgoName is " << pset.getParameter<string>("Reco2DAlgoName") << endl;
}

/// Destructor
DTRecSegment2DProducer::~DTRecSegment2DProducer() {
  if (debug)
    cout << "[DTRecSegment2DProducer] Destructor called" << endl;
}

/* Operations */
void DTRecSegment2DProducer::produce(edm::Event& event, const edm::EventSetup& setup) {
  if (debug)
    cout << "[DTRecSegment2DProducer] produce called" << endl;
  // Get the DT Geometry
  DTGeometry const& dtGeom = setup.getData(dtGeomToken_);

  theAlgo->setES(setup);

  // Get the 1D rechits from the event
  Handle<DTRecHitCollection> allHits;
  event.getByToken(recHits1DToken_, allHits);

  // Create the pointer to the collection which will store the rechits
  auto segments = std::make_unique<DTRecSegment2DCollection>();

  // Iterate through all hit collections ordered by LayerId
  DTRecHitCollection::id_iterator dtLayerIt;
  DTSuperLayerId oldSlId;
  for (dtLayerIt = allHits->id_begin(); dtLayerIt != allHits->id_end(); ++dtLayerIt) {
    // The layerId
    DTLayerId layerId = (*dtLayerIt);
    const DTSuperLayerId SLId = layerId.superlayerId();
    if (SLId == oldSlId)
      continue;  // I'm on the same SL as before
    oldSlId = SLId;

    if (debug)
      cout << "Reconstructing the 2D segments in " << SLId << endl;

    const DTSuperLayer* sl = dtGeom.superLayer(SLId);

    // Get all the rec hit in the same superLayer in which layerId relies
    DTRecHitCollection::range range = allHits->get(DTRangeMapAccessor::layersBySuperLayer(SLId));

    // Fill the vector with the 1D RecHit
    vector<DTRecHit1DPair> pairs(range.first, range.second);

    if (debug)
      cout << "Number of 1D-RecHit pairs " << pairs.size() << endl;

    if (debug)
      cout << "Start the 2D-segments Reco " << endl;
    OwnVector<DTSLRecSegment2D> segs = theAlgo->reconstruct(sl, pairs);
    if (debug)
      cout << "Number of Reconstructed segments: " << segs.size() << endl;

    if (!segs.empty())
      segments->put(SLId, segs.begin(), segs.end());
  }
  event.put(std::move(segments));
}
