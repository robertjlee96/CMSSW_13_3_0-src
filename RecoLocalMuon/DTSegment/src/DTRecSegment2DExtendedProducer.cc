/** \file
 *
 * \author Stefano Lacaprara - INFN Legnaro <stefano.lacaprara@pd.infn.it>
 */

/* This Class Header */
#include "RecoLocalMuon/DTSegment/src/DTRecSegment2DExtendedProducer.h"

/* Collaborating Class Header */
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
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

#include "RecoLocalMuon/DTSegment/src/DTCombinatorialExtendedPatternReco.h"

/* C++ Headers */
#include <string>
using namespace std;

/* ====================================================================== */

/// Constructor
DTRecSegment2DExtendedProducer::DTRecSegment2DExtendedProducer(const edm::ParameterSet& pset)
    : dtGeomToken_(esConsumes()) {
  // Set verbose output
  debug = pset.getUntrackedParameter<bool>("debug");

  // the name of the 1D rec hits collection
  recHits1DToken_ = consumes<DTRecHitCollection>(pset.getParameter<InputTag>("recHits1DLabel"));
  recClusToken_ = consumes<DTRecClusterCollection>(pset.getParameter<InputTag>("recClusLabel"));

  if (debug)
    cout << "[DTRecSegment2DExtendedProducer] Constructor called" << endl;

  produces<DTRecSegment2DCollection>();

  // Get the concrete reconstruction algo from the factory
  theAlgo =
      new DTCombinatorialExtendedPatternReco(pset.getParameter<ParameterSet>("Reco2DAlgoConfig"), consumesCollector());
}

/// Destructor
DTRecSegment2DExtendedProducer::~DTRecSegment2DExtendedProducer() {
  if (debug)
    cout << "[DTRecSegment2DExtendedProducer] Destructor called" << endl;
  delete theAlgo;
}

/* Operations */
void DTRecSegment2DExtendedProducer::produce(edm::Event& event, const edm::EventSetup& setup) {
  if (debug)
    cout << "[DTRecSegment2DExtendedProducer] produce called" << endl;
  // Get the DT Geometry
  const DTGeometry& dtGeom = setup.getData(dtGeomToken_);

  theAlgo->setES(setup);

  // Get the 1D rechits from the event
  Handle<DTRecHitCollection> allHits;
  event.getByToken(recHits1DToken_, allHits);

  // Get the 1D clusters from the event
  Handle<DTRecClusterCollection> dtClusters;
  event.getByToken(recClusToken_, dtClusters);
  theAlgo->setClusters(vector<DTSLRecCluster>(dtClusters->begin(), dtClusters->end()));

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

    //if(debug) cout << "Start the 2D-segments Reco "<< endl;
    OwnVector<DTSLRecSegment2D> segs = theAlgo->reconstruct(sl, pairs);
    if (debug) {
      cout << "Number of Reconstructed segments: " << segs.size() << endl;
      copy(segs.begin(), segs.end(), ostream_iterator<DTSLRecSegment2D>(cout, "\n"));
    }

    if (!segs.empty())
      segments->put(SLId, segs.begin(), segs.end());
  }
  event.put(std::move(segments));
}
