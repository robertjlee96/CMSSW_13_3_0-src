#ifndef Calibration_EcalAlCaRecoProducers_SelectedElectronFEDListProducer_h
#define Calibration_EcalAlCaRecoProducers_SelectedElectronFEDListProducer_h

#include "DataFormats/BeamSpot/interface/BeamSpot.h"
// egamma objects
#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"

#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"
// Geometry
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/EcalMapping/interface/EcalMappingRcd.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
// strip geometry
#include "CalibFormats/SiStripObjects/interface/SiStripRegionCabling.h"
#include "CalibTracker/Records/interface/SiStripDetCablingRcd.h"
#include "CalibTracker/Records/interface/SiStripRegionCablingRcd.h"
// Strip and pixel
#include "CondFormats/DataRecord/interface/SiPixelFedCablingMapRcd.h"
#include "CondFormats/SiPixelObjects/interface/SiPixelFedCablingMap.h"
// Hcal objects
#include "CalibFormats/HcalObjects/interface/HcalDbService.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"

// Math
#include "DataFormats/Math/interface/normalizedPhi.h"

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/EventSetup.h"
// Message logger
#include "FWCore/ServiceRegistry/interface/Service.h"

class InputTag;

class FEDRawDataCollection;

class SiPixelFedCablingTree;
class SiStripFedCabling;

class CaloSubdetectorGeometry;
class HcalElectronicsMap;

// Hcal rec hit: this is a Fwd file defining typedefs
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"

// Pixel region class
class PixelRegion {
public:
  PixelRegion(math::XYZVector& momentum, float dphi = 0.5, float deta = 0.5, float maxz = 24.0) {
    vector = momentum;
    dPhi = dphi;
    dEta = deta;
    maxZ = maxz;
    cosphi = vector.x() / vector.rho();
    sinphi = vector.y() / vector.rho();
    atantheta = vector.z() / vector.rho();
  }

  math::XYZVector vector;
  float dPhi, dEta, maxZ;
  float cosphi, sinphi, atantheta;
};

// Pixel module class
class PixelModule {
public:
  PixelModule() {}
  PixelModule(float phi, float eta) : Phi(phi), Eta(eta), x(0.), y(0.), z(0.), DetId(0), Fed(0) {}
  bool operator<(const PixelModule& m) const {
    if (Phi < m.Phi)
      return true;
    if (Phi == m.Phi && Eta < m.Eta)
      return true;
    if (Phi == m.Phi && Eta == m.Eta && DetId < m.DetId)
      return true;
    return false;
  }

  float Phi, Eta;
  float x, y, z;
  unsigned int DetId;
  unsigned int Fed;
};

// main class
template <typename TEle, typename TCand>
class SelectedElectronFEDListProducer : public edm::stream::EDProducer<> {
public:
  explicit SelectedElectronFEDListProducer(const edm::ParameterSet&);
  ~SelectedElectronFEDListProducer() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

protected:
  virtual void beginJob();
  virtual void endJob();
  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  typedef std::vector<TEle> TEleColl;
  typedef std::vector<TCand> TCandColl;

public:
  void pixelFedDump(std::vector<PixelModule>::const_iterator& itDn,
                    std::vector<PixelModule>::const_iterator& itUp,
                    const PixelRegion& region);

private:
  // input parameter of the producer
  std::vector<edm::InputTag> recoEcalCandidateTags_;
  std::vector<edm::InputTag> electronTags_;
  edm::InputTag beamSpotTag_;
  edm::InputTag rawDataTag_;

  edm::InputTag HBHERecHitTag_;

  std::vector<int> isGsfElectronCollection_;
  std::vector<int> addThisSelectedFEDs_;

  edm::FileInPath ESLookupTable_;

  bool dumpSelectedEcalFed_;
  bool dumpSelectedSiStripFed_;
  bool dumpSelectedSiPixelFed_;
  bool dumpSelectedHCALFed_;
  bool dumpAllEcalFed_;
  bool dumpAllTrackerFed_;
  bool dumpAllHCALFed_;

  double dRStripRegion_;
  double dPhiPixelRegion_;
  double dEtaPixelRegion_;
  double maxZPixelRegion_;
  double dRHcalRegion_;

  std::string outputLabelModule_;

  // Token for the input collection
  edm::EDGetTokenT<FEDRawDataCollection> rawDataToken_;
  edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;
  edm::EDGetTokenT<HBHERecHitCollection> hbheRecHitToken_;
  std::vector<edm::EDGetTokenT<trigger::TriggerFilterObjectWithRefs> > recoEcalCandidateToken_;
  std::vector<edm::EDGetTokenT<TEleColl> > electronToken_;

  // Token for the input collection
  const edm::ESGetToken<HcalDbService, HcalDbRecord> hcalDbToken_;
  const edm::ESGetToken<EcalElectronicsMapping, EcalMappingRcd> ecalMappingToken_;
  const edm::ESGetToken<CaloGeometry, CaloGeometryRecord> caloGeometryToken_;
  const edm::ESGetToken<SiPixelFedCablingMap, SiPixelFedCablingMapRcd> siPixelFedCablingMapToken_;
  const edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> trackerGeometryToken_;
  const edm::ESGetToken<SiStripRegionCabling, SiStripRegionCablingRcd> siStripRegionCablingToken_;

  // used inside the producer
  math::XYZVector beamSpotPosition_;

  // internal info for ES geometry
  int ES_fedId_[2][2][40][40];

  // fed list and output raw data
  std::vector<uint32_t> fedList_;

  // get calo geomentry and electronic map
  const EcalElectronicsMapping* EcalMapping_;
  const CaloGeometry* GeometryCalo_;
  const CaloSubdetectorGeometry* GeometryES_;
  const SiStripRegionCabling* StripRegionCabling_;
  const HcalElectronicsMap* HcalReadoutMap_;

  // get pixel geometry and electronic map
  std::unique_ptr<SiPixelFedCablingTree> PixelCabling_;
  std::vector<PixelModule> pixelModuleVector_;

  // get strip geometry and electronic map
  std::pair<double, double> regionDimension_;
};

#endif  // SelectedElectronFEDListProducer_h
