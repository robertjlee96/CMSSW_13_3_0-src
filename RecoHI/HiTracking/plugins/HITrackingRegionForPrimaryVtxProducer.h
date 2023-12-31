#ifndef RecoHI_HiTracking_HITrackingRegionForPrimaryVtxProducer_H
#define RecoHI_HiTracking_HITrackingRegionForPrimaryVtxProducer _H

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"

#include "RecoTracker/TkTrackingRegions/interface/TrackingRegionProducer.h"
#include "RecoTracker/TkTrackingRegions/interface/GlobalTrackingRegion.h"
#include "RecoTracker/TkTrackingRegions/interface/RectangularEtaPhiTrackingRegion.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "RecoTracker/Record/interface/TrackerMultipleScatteringRecord.h"
#include "RecoTracker/TkMSParametrization/interface/MultipleScatteringParametrisationMaker.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"
#include "DataFormats/Common/interface/DetSetAlgorithm.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHitCollection.h"

#include "TMath.h"

class HITrackingRegionForPrimaryVtxProducer : public TrackingRegionProducer {
public:
  HITrackingRegionForPrimaryVtxProducer(const edm::ParameterSet& cfg, edm::ConsumesCollector&& iC)
      : theTtopoToken(iC.esConsumes()), theFieldToken(iC.esConsumes()) {
    edm::ParameterSet regionPSet = cfg.getParameter<edm::ParameterSet>("RegionPSet");
    thePtMin = regionPSet.getParameter<double>("ptMin");
    theOriginRadius = regionPSet.getParameter<double>("originRadius");
    theNSigmaZ = regionPSet.getParameter<double>("nSigmaZ");
    theBeamSpotTag = regionPSet.getParameter<edm::InputTag>("beamSpot");
    theBeamSpotToken = iC.consumes<reco::BeamSpot>(theBeamSpotTag);
    thePrecise = regionPSet.getParameter<bool>("precise");
    theSiPixelRecHits = regionPSet.getParameter<edm::InputTag>("siPixelRecHits");
    theSiPixelRecHitsToken = iC.consumes<SiPixelRecHitCollection>(theSiPixelRecHits);
    doVariablePtMin = regionPSet.getParameter<bool>("doVariablePtMin");
    double xDir = regionPSet.getParameter<double>("directionXCoord");
    double yDir = regionPSet.getParameter<double>("directionYCoord");
    double zDir = regionPSet.getParameter<double>("directionZCoord");
    theDirection = GlobalVector(xDir, yDir, zDir);

    // for using vertex instead of beamspot
    theSigmaZVertex = regionPSet.getParameter<double>("sigmaZVertex");
    theFixedError = regionPSet.getParameter<double>("fixedError");
    theUseFoundVertices = regionPSet.getParameter<bool>("useFoundVertices");
    theUseFixedError = regionPSet.getParameter<bool>("useFixedError");
    vertexCollName = regionPSet.getParameter<edm::InputTag>("VertexCollection");
    vertexCollToken = iC.consumes<reco::VertexCollection>(vertexCollName);

    if (thePrecise) {
      theMSMakerToken = iC.esConsumes();
    }
  }

  ~HITrackingRegionForPrimaryVtxProducer() override = default;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    desc.add<double>("ptMin", 0.7);
    desc.add<bool>("doVariablePtMin", true);
    desc.add<double>("originRadius", 0.2);
    desc.add<double>("nSigmaZ", 3.0);
    desc.add<edm::InputTag>("beamSpot", edm::InputTag("offlineBeamSpot"));
    desc.add<bool>("precise", true);
    desc.add<bool>("useMultipleScattering", false);
    desc.add<bool>("useFakeVertices", false);
    desc.add<edm::InputTag>("siPixelRecHits", edm::InputTag("siPixelRecHits"));
    desc.add<double>("directionXCoord", 1.0);
    desc.add<double>("directionYCoord", 1.0);
    desc.add<double>("directionZCoord", 0.0);
    desc.add<bool>("useFoundVertices", true);
    desc.add<edm::InputTag>("VertexCollection", edm::InputTag("hiPixelClusterVertex"));
    desc.add<bool>("useFixedError", true);
    desc.add<double>("fixedError", 3.0);
    desc.add<double>("sigmaZVertex", 3.0);

    // Only for backwards-compatibility
    edm::ParameterSetDescription descRegion;
    descRegion.add<edm::ParameterSetDescription>("RegionPSet", desc);

    descriptions.add("hiTrackingRegionFromClusterVtx", descRegion);
  }

  int estimateMultiplicity(const edm::Event& ev, const edm::EventSetup& es) const {
    //rechits
    edm::Handle<SiPixelRecHitCollection> recHitColl;
    ev.getByToken(theSiPixelRecHitsToken, recHitColl);

    auto const& ttopo = es.getData(theTtopoToken);

    std::vector<const TrackingRecHit*> theChosenHits;
    edmNew::copyDetSetRange(*recHitColl, theChosenHits, ttopo.pxbDetIdLayerComparator(1));
    return theChosenHits.size();
  }

  std::vector<std::unique_ptr<TrackingRegion> > regions(const edm::Event& ev,
                                                        const edm::EventSetup& es) const override {
    int estMult = estimateMultiplicity(ev, es);

    // from MC relating first layer pixel hits to "findable" sim tracks with pt>1 GeV
    float cc = -38.6447;
    float bb = 0.0581765;
    float aa = 1.34306e-06;

    float estTracks = aa * estMult * estMult + bb * estMult + cc;

    LogTrace("heavyIonHLTVertexing") << "[HIVertexing]";
    LogTrace("heavyIonHLTVertexing") << " [HIVertexing: hits in the 1. layer:" << estMult << "]";
    LogTrace("heavyIonHLTVertexing") << " [HIVertexing: estimated number of tracks:" << estTracks << "]";

    float regTracking = 60.;  //if we have more tracks -> regional tracking
    float etaB = 10.;
    float phiB = TMath::Pi() / 2.;

    float decEta = estTracks / 90.;
    etaB = 2.5 / decEta;

    if (estTracks > regTracking) {
      LogTrace("heavyIonHLTVertexing") << " [HIVertexing: Regional Tracking]";
      LogTrace("heavyIonHLTVertexing") << "  [Regional Tracking: eta range: -" << etaB << ", " << etaB << "]";
      LogTrace("heavyIonHLTVertexing") << "  [Regional Tracking: phi range: -" << phiB << ", " << phiB << "]";
      LogTrace("heavyIonHLTVertexing") << "  [Regional Tracking: factor of decrease: " << decEta * 2.
                                       << "]";  // 2:from phi
    }

    float minpt = thePtMin;
    float varPtCutoff = 1500;  //cutoff
    if (doVariablePtMin && estMult < varPtCutoff) {
      minpt = 0.075;
      if (estMult > 0)
        minpt += estMult * (thePtMin - 0.075) / varPtCutoff;  // lower ptMin linearly with pixel hit multiplicity
    }

    // tracking region selection
    std::vector<std::unique_ptr<TrackingRegion> > result;
    double halflength;
    GlobalPoint origin;
    edm::Handle<reco::BeamSpot> bsHandle;
    ev.getByToken(theBeamSpotToken, bsHandle);
    if (bsHandle.isValid()) {
      const reco::BeamSpot& bs = *bsHandle;
      origin = GlobalPoint(bs.x0(), bs.y0(), bs.z0());
      halflength = theNSigmaZ * bs.sigmaZ();

      if (theUseFoundVertices) {
        edm::Handle<reco::VertexCollection> vertexCollection;
        ev.getByToken(vertexCollToken, vertexCollection);

        for (reco::VertexCollection::const_iterator iV = vertexCollection->begin(); iV != vertexCollection->end();
             iV++) {
          if (iV->isFake() || !iV->isValid())
            continue;
          origin = GlobalPoint(bs.x0(), bs.y0(), iV->z());
          halflength = (theUseFixedError ? theFixedError : (iV->zError()) * theSigmaZVertex);
        }
      }

      if (estTracks > regTracking) {  // regional tracking
        const auto& field = es.getData(theFieldToken);
        const MultipleScatteringParametrisationMaker* msmaker = nullptr;
        if (thePrecise) {
          msmaker = &es.getData(theMSMakerToken);
        }

        result.push_back(std::make_unique<RectangularEtaPhiTrackingRegion>(
            theDirection, origin, thePtMin, theOriginRadius, halflength, etaB, phiB, field, msmaker, thePrecise));
      } else {  // global tracking
        LogTrace("heavyIonHLTVertexing") << " [HIVertexing: Global Tracking]";
        result.push_back(
            std::make_unique<GlobalTrackingRegion>(minpt, origin, theOriginRadius, halflength, thePrecise));
      }
    }
    return result;
  }

private:
  double thePtMin;
  double theOriginRadius;
  double theNSigmaZ;
  edm::InputTag theBeamSpotTag;
  edm::EDGetTokenT<reco::BeamSpot> theBeamSpotToken;
  bool thePrecise;
  GlobalVector theDirection;
  edm::InputTag theSiPixelRecHits;
  edm::EDGetTokenT<SiPixelRecHitCollection> theSiPixelRecHitsToken;
  bool doVariablePtMin;

  double theSigmaZVertex;
  double theFixedError;
  bool theUseFoundVertices;
  bool theUseFixedError;
  edm::InputTag vertexCollName;
  edm::EDGetTokenT<reco::VertexCollection> vertexCollToken;
  edm::ESGetToken<TrackerTopology, TrackerTopologyRcd> theTtopoToken;
  edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> theFieldToken;
  edm::ESGetToken<MultipleScatteringParametrisationMaker, TrackerMultipleScatteringRecord> theMSMakerToken;
};

#endif
